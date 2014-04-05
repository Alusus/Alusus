/**
 * @file Core/Parser/State.cpp
 * Contains the implementation of Parser::State.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Parser
{

//==============================================================================
// Constructor

State::State() :
  trunkState(0),
  tempTrunkTermStackIndex(-1),
  tempTrunkProdStackIndex(-1),
  topTermLevelCache(0),
  topProdLevelCache(0),
  trunkSharedBuildMsgCount(0),
  grammarHelper(static_cast<Data::Provider*>(&grammarContext)),
  processingStatus(ProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ProcessingStatus::IN_PROGRESS),
  tokensToLive(-1)
{
}


State::State(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
             Word reservedVarCount, Word reservedVarLevelCount, Data::GrammarModule *rootModule) :
  trunkState(0),
  tempTrunkTermStackIndex(-1),
  tempTrunkProdStackIndex(-1),
  topTermLevelCache(0),
  topProdLevelCache(0),
  trunkSharedBuildMsgCount(0),
  termStack(reservedTermLevelCount),
  prodStack(reservedProdLevelCount),
  variableStack(maxVarNameLength, reservedVarCount, reservedVarLevelCount),
  grammarHelper(static_cast<Data::Provider*>(&grammarContext)),
  processingStatus(ProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ProcessingStatus::IN_PROGRESS),
  tokensToLive(-1)
{
  this->termStack.resize(0);
  this->prodStack.resize(0);
  this->grammarContext.setRootModule(rootModule);
  this->grammarContext.setVariableStack(&this->variableStack);
}


State::State(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
             Word reservedVarCount, Word reservedVarLevelCount, const Data::ParsingGrammarContext *context) :
  trunkState(0),
  tempTrunkTermStackIndex(-1),
  tempTrunkProdStackIndex(-1),
  topTermLevelCache(0),
  topProdLevelCache(0),
  trunkSharedBuildMsgCount(0),
  termStack(reservedTermLevelCount),
  prodStack(reservedProdLevelCount),
  variableStack(maxVarNameLength, reservedVarCount, reservedVarLevelCount),
  grammarHelper(static_cast<Data::Provider*>(&grammarContext)),
  processingStatus(ProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ProcessingStatus::IN_PROGRESS),
  tokensToLive(-1)
{
  this->termStack.resize(0);
  this->prodStack.resize(0);
  this->grammarContext.copyFrom(context);
  this->grammarContext.setVariableStack(&this->variableStack);
}


//==============================================================================
// Term Stack Member Functions

void State::initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                       Word reservedVarCount, Word reservedVarLevelCount, Data::GrammarModule *rootModule)
{
  ASSERT(reservedTermLevelCount > 0);
  ASSERT(reservedProdLevelCount > 0);
  ASSERT(maxVarNameLength > 1);
  ASSERT(reservedVarCount > 0);
  ASSERT(reservedVarLevelCount > 0);

  this->termStack.reserve(reservedTermLevelCount);
  this->prodStack.reserve(reservedProdLevelCount);
  this->variableStack.initialize(maxVarNameLength, reservedVarCount, reservedVarLevelCount);
  this->grammarContext.setRootModule(rootModule);
  this->grammarContext.setVariableStack(&this->variableStack);
}


void State::initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                       Word reservedVarCount, Word reservedVarLevelCount,
                       const Data::ParsingGrammarContext *context)
{
  ASSERT(reservedTermLevelCount > 0);
  ASSERT(reservedProdLevelCount > 0);
  ASSERT(maxVarNameLength > 1);
  ASSERT(reservedVarCount > 0);
  ASSERT(reservedVarLevelCount > 0);

  this->termStack.reserve(reservedTermLevelCount);
  this->prodStack.reserve(reservedProdLevelCount);
  this->variableStack.initialize(maxVarNameLength, reservedVarCount, reservedVarLevelCount);
  this->grammarContext.copyFrom(context);
  this->grammarContext.setVariableStack(&this->variableStack);
}


/**
 * Delete all state levels and reset branching information.
 */
void State::reset()
{
  this->processingStatus = ProcessingStatus::IN_PROGRESS;
  this->tokensToLive = -1;
  this->termStack.clear();
  this->prodStack.clear();
  this->variableStack.clear();
  this->buildMsgs.clear();
  this->trunkState = 0;
  this->tempTrunkTermStackIndex = -1;
  this->tempTrunkProdStackIndex = -1;
  this->trunkSharedBuildMsgCount = 0;
  this->topTermLevelCache = 0;
  this->topProdLevelCache = 0;
}


/**
 * References an entry in the hierarchy term stack after checking whether this
 * state is a branch from another state. If the state is a branch, the function
 * will put into consideration whether the entry is in the trunk state's level
 * stack.
 *
 * @param i The index of the entry to retrieve. If this is a branch state, then
 *          the function will put into consideration that this object's stack is
 *          a continuation of the trunk state's term stack, and thus will
 *          consider an index value smaller than the size of the trunk state to
 *          belong to that state rather than this state. If this value is
 *          negative, counting will happen from the top of the stack instead. So
 *          if i is -1 the top level in the stack will be returned. If it's -2
 *          the level below the top will be returned and so on.
 * @return Returns a reference to the retrieved entry. The caller should avoid
 *         keeping a pointer to this entry because the entry can be moved in
 *         memory as a result of a push or pop operation.
 */
TermLevel& State::refTermLevel(Int i)
{
  // Validate i and convert it into positive indexing if it's currently negative.
  if (i >= 0) {
    if (i >= this->getTermLevelCount()) {
      throw GeneralException(STR("This state has an empty term stack, or i is out of range."),
                             STR("Core::Parser::State::refTermLevel"));
    }
  } else {
    if (-(i) > this->getTermLevelCount()) {
      throw GeneralException(STR("Given state has an empty term stack, or i is out of range."),
                             STR("Core::Parser::State::refTermLevel"));
    }
    i = this->getTermLevelCount() + i;
  }
  // Retrieve the level.
  if (this->tempTrunkTermStackIndex >= 0) {
    ASSERT(this->trunkState != 0);
    if (i <= this->tempTrunkTermStackIndex) {
      return this->trunkState->refTermLevel(i);
    } else {
      return this->termStack[i-(this->tempTrunkTermStackIndex+1)];
    }
  } else {
    return this->termStack[i];
  }
}


/**
 * Get the number of state levels in the top produciton in this state. The
 * level count includes the production root's level.
 */
Int State::getTopprodTermLevelCount() const
{
  // The first level does not belong to any production, so we need at least 2 levels.
  if (this->getTermLevelCount() <= 1) {
    throw GeneralException(STR("This state has an empty term stack."),
                           STR("Core::Parser::State::getTopprodTermLevelCount"));
  }
  ASSERT(this->getProdLevelCount() > 0);
  // Find the production root, then get its data.
  return this->getTermLevelCount() - this->refTopProdLevel().getTermStackIndex();
}


/**
 * Push a new level into the state term stack and initialize its checksum
 * values.
 */
void State::pushTermLevel(Data::Term *term)
{
  this->termStack.push_back(TermLevel());
  this->topTermLevelCache = &this->refTermLevel(-1);

  // If we don't have any term to set, then we also won't have any related info to cache.
  if (term == 0) return;

  // Set level info.
  this->topTermLevelCache->setTerm(term);
  // Cache term parameters for faster access later.
  if (term->isA<Data::TokenTerm>()) {
    Data::TokenTerm *tokenTerm = static_cast<Data::TokenTerm*>(term);
    IdentifiableObject *param;
    param = this->grammarHelper.getTokenTermId(tokenTerm);
    this->topTermLevelCache->setParam1(param);
    param = this->grammarHelper.getTokenTermText(tokenTerm);
    this->topTermLevelCache->setParam2(param);
  } else if (term->isA<Data::ListTerm>()) {
    Data::ListTerm *list_term = static_cast<Data::ListTerm*>(term);
    IdentifiableObject *param = this->grammarHelper.getListTermData(list_term);
    this->topTermLevelCache->setParam1(param);
  } else if (term->isA<Data::MultiplyTerm>()) {
    Data::MultiplyTerm *multiplyTerm = static_cast<Data::MultiplyTerm*>(term);
    IdentifiableObject *param;
    param = this->grammarHelper.getMultiplyTermMax(multiplyTerm);
    this->topTermLevelCache->setParam1(param);
    param = this->grammarHelper.getMultiplyTermMin(multiplyTerm);
    this->topTermLevelCache->setParam2(param);
    param = this->grammarHelper.getMultiplyTermPriority(multiplyTerm);
    this->topTermLevelCache->setParam3(param);
  }
}


/**
 * Remove a state level from the top of the term stack. This function puts into
 * consideration branching information and adjusts them if needed. If this
 * state's term stack is empty and this state has branched from another state,
 * the trunk level index will be decremented.
 */
void State::popTermLevel()
{
  ASSERT(!this->isAtProdRoot());
  if (this->termStack.size() > 0) {
    this->termStack.pop_back();
  } else {
    if (this->tempTrunkTermStackIndex >= 0) {
      this->tempTrunkTermStackIndex--;
    } else {
      // This should never be reached.
      ASSERT(false);
    }
  }
  if (this->getTermLevelCount() > 0) this->topTermLevelCache = &this->refTermLevel(-1);
  else this->topTermLevelCache = 0;
}


Int State::findProdLevel(Int termIndex) const
{
  if (termIndex < 0) termIndex += this->getTermLevelCount();
  if (this->prodStack.size() == 0) return -1;
  if (termIndex < this->refProdLevel(0).getTermStackIndex()) return -1;
  if (termIndex >= this->refTopProdLevel().getTermStackIndex()) {
    return this->getProdLevelCount()-1;
  }
  return this->_findProdLevel(termIndex, 0, this->getProdLevelCount()-2);
}


Int State::_findProdLevel(Int termIndex, Int start, Int end) const
{
  if (this->refProdLevel(start).getTermStackIndex() == termIndex) return start;
  else if (this->refProdLevel(end).getTermStackIndex() >= termIndex) return end;
  else if (end-start < 2) return start;
  else {
    Int mid = (start+end)/2;
    if (this->refProdLevel(mid).getTermStackIndex() > termIndex) {
      return this->_findProdLevel(termIndex, start, mid);
    } else {
      return this->_findProdLevel(termIndex, mid, end);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
// Production Stack Member Functions

ProdLevel& State::refProdLevel(Int i)
{
  // Validate i and convert it into positive indexing if it's currently negative.
  if (i >= 0) {
    if (i >= this->getProdLevelCount()) {
      throw GeneralException(STR("This state has an empty production stack, or i is out of range."),
                             STR("Core::Parser::State::refProdLevel"));
    }
  } else {
    if (-(i) > this->getTermLevelCount()) {
      throw GeneralException(STR("Given state has an empty production stack, or i is out of range."),
                             STR("Core::Parser::State::refProdLevel"));
    }
    i = this->getProdLevelCount() + i;
  }
  // Retrieve the level.
  if (this->tempTrunkProdStackIndex >= 0) {
    ASSERT(this->trunkState != 0);
    if (i <= this->tempTrunkProdStackIndex) {
      return this->trunkState->refProdLevel(i);
    } else {
      return this->prodStack[i-(this->tempTrunkProdStackIndex+1)];
    }
  } else {
    return this->prodStack[i];
  }
}


void State::pushProdLevel(Data::Module *module, Data::SymbolDefinition *prod)
{
  this->prodStack.push_back(ProdLevel());
  this->topProdLevelCache = &this->prodStack.back();
  this->topProdLevelCache->setModule(module);
  this->topProdLevelCache->setProd(prod);
  this->topProdLevelCache->setTermStackIndex(this->getTermLevelCount());
  this->variableStack.pushLevel();
  this->grammarContext.switchCurrentModule(module);
  this->grammarContext.setCurrentArgumentList(this->grammarHelper.getSymbolVars(prod));
  this->pushTermLevel(this->grammarHelper.getSymbolTerm(prod));
}


void State::popProdLevel()
{
  // We only allow popping production levels when only the root term level is remaining.
  ASSERT(this->isAtProdRoot());

  // Pop the associated variable level, if any.
  this->variableStack.popLevel();

  // Pop the production level itself.
  if (this->prodStack.size() > 0) {
    this->prodStack.pop_back();
  } else {
    if (this->tempTrunkProdStackIndex >= 0) {
      this->tempTrunkProdStackIndex--;
    } else {
      // This should never be reached.
      ASSERT(false);
    }
  }

  if (this->getProdLevelCount() > 0) {
    this->topProdLevelCache = &this->refProdLevel(-1);
    this->grammarContext.switchCurrentModule(this->topProdLevelCache->getModule());
    Data::Map *vars = this->grammarHelper.getSymbolVars(this->topProdLevelCache->getProd());
    this->grammarContext.setCurrentArgumentList(vars);
  }
  else {
    this->topProdLevelCache = 0;
  }

  this->popTermLevel();
}


////////////////////////////////////////////////////////////////////////////////
// Term Helper Functions

Word State::getListTermChildCount(Int levelOffset) const
{
  const TermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isDerivedFrom<Data::ListTerm>());
  return this->grammarHelper.getListTermChildCount(static_cast<Data::ListTerm*>(level->getTerm()),
                                                   level->getParam1());
}


Data::Term* State::useListTermChild(Int index, Int levelOffset)
{
  TermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isDerivedFrom<Data::ListTerm>());
  return this->grammarHelper.useListTermChild(&this->grammarContext,
                                              static_cast<Data::ListTerm*>(level->getTerm()),
                                              index, level->getParam1());
}


Data::Integer* State::getTokenTermId(Int levelOffset) const
{
  const TermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::TokenTerm>());
  return static_cast<Data::Integer*>(level->getParam1());
}


IdentifiableObject* State::getTokenTermText(Int levelOffset) const
{
  const TermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::TokenTerm>());
  return level->getParam2();
}


void State::getReferencedDefinition(Data::Module *&module, Data::SymbolDefinition *&definition,
                                    Int levelOffset) const
{
  Data::Reference *ref;
  if (levelOffset == -1) {
    ASSERT(this->refTopTermLevel().getTerm()->isA<Data::ReferenceTerm>());
    ref = static_cast<Data::ReferenceTerm*>(this->refTopTermLevel().getTerm())->getReference().get();
  } else {
    ASSERT(this->refTermLevel(levelOffset).getTerm()->isA<Data::ReferenceTerm>());
    ref = static_cast<Data::ReferenceTerm*>(this->refTermLevel(levelOffset).getTerm())->getReference().get();
  }
  return this->grammarHelper.getReferencedDefinition(ref, module, definition);
}


Data::Integer* State::getMultiplyTermMax(Int levelOffset) const
{
  const TermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::MultiplyTerm>());
  return static_cast<Data::Integer*>(level->getParam1());
}


Data::Integer* State::getMultiplyTermMin(Int levelOffset) const
{
  const TermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::MultiplyTerm>());
  return static_cast<Data::Integer*>(level->getParam2());
}


Data::Integer* State::getMultiplyTermPriority(Int levelOffset) const
{
  const TermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::MultiplyTerm>());
  return static_cast<Data::Integer*>(level->getParam3());
}


////////////////////////////////////////////////////////////////////////////////
// Other Member Functions

/**
 * Set the branching info to make this state a branch of another state.
 *
 * @param ts Trunk state. The state from which this state branched.
 * @param ttl Tokens to live. The number of tokens to be recieved before the
 *            state is to be killed if branching didn't resolve by itself.
 * @param tsi Term Stack index. The term level index within the trunk state from
 *            which branching started. The trunk term level referred to by this
 *            variable is included in the list of levels shared by the two
 *            states.
 * @param psi Prod Stack index. The prod level index within the trunk state from
 *            which branching started. The trunk prod level referred to by this
 *            variable is included in the list of levels shared by the two
 *            states.
 *
 * @note The tsi variable is a temporary variable used for the purpose of
 *       speeding up the process of creating branches when we are not yet
 *       sure whether the branch will be retained or not. If the branch is
 *       to be retained, we'll need to copy the trunk's data into this
 *       state. This is necessary because we're not sure if the original
 *       state will live longer than this state. The same applies to psi and
 *       vsi.
 */
void State::setBranchingInfo(State *ts, Int ttl, Int tsi, Int psi)
{
  ASSERT(ts != 0 || (ttl == -1 && tsi == -1 && psi == -1));
  ASSERT(ttl >= -1 && tsi >= -1 && psi >= -1);
  this->reset();
  this->trunkState = ts;
  this->tempTrunkTermStackIndex = tsi;
  this->tempTrunkProdStackIndex = psi;
  if (tsi >= 0) {
    this->topTermLevelCache = &this->refTermLevel(tsi);
  }
  if (psi >= 0) {
    this->topProdLevelCache = &this->refProdLevel(psi);
    this->variableStack.setBranchingInfo(ts->getVariableStack(), psi);
    if (ts->getProdLevelCount() == psi+1) {
      this->grammarContext.copyFrom(ts->getGrammarContext());
      this->grammarContext.setVariableStack(&this->variableStack);
    } else {
      this->grammarContext.switchCurrentModule(this->topProdLevelCache->getModule());
      Data::Map *vars = this->grammarHelper.getSymbolVars(this->topProdLevelCache->getProd());
      this->grammarContext.setCurrentArgumentList(vars);
    }
  }
  this->tokensToLive = ttl;
}


/**
 * Decrement the tokens-to-live counter, if this state branched from another
 * state.
 *
 * @return Returns true if the counter reaches 0, false otherwise.
 */
bool State::decrementTokensToLive()
{
  if (this->tokensToLive == -1) {
    return false;
  } else if (this->tokensToLive > 1) {
    this->tokensToLive--;
    return false;
  } else {
    this->tokensToLive = 0;
    return true;
  }
}


void State::ownTopTermLevel()
{
  ASSERT(this->getTermLevelCount() > 0);
  if (this->termStack.size() > 0) return;
  ASSERT(this->trunkState != 0);
  ASSERT(this->tempTrunkTermStackIndex >= 0);
  TermLevel &srcLevel = this->trunkState->refTermLevel(this->tempTrunkTermStackIndex);
  this->tempTrunkTermStackIndex--;
  this->termStack.push_back(TermLevel());
  this->topTermLevelCache = &this->refTermLevel(-1);
  this->refTopTermLevel().setPosId(srcLevel.getPosId());
  this->refTopTermLevel().setTerm(srcLevel.getTerm());
  this->refTopTermLevel().setParam1(srcLevel.getParam1());
  this->refTopTermLevel().setParam2(srcLevel.getParam2());
  this->refTopTermLevel().setParam3(srcLevel.getParam3());
}


void State::ownTopProdLevel()
{
  ASSERT(this->getProdLevelCount() > 0);
  if (this->prodStack.size() > 0) return;
  ASSERT(this->trunkState != 0);
  ASSERT(this->tempTrunkProdStackIndex >= 0);
  ProdLevel &srcLevel = this->trunkState->refProdLevel(this->tempTrunkProdStackIndex);
  this->tempTrunkProdStackIndex--;
  this->prodStack.push_back(ProdLevel());
  this->topProdLevelCache = &this->refProdLevel(-1);
  this->refTopProdLevel().setModule(srcLevel.getModule());
  this->refTopProdLevel().setProd(srcLevel.getProd());
  this->refTopProdLevel().setTermStackIndex(srcLevel.getTermStackIndex());
}


void State::ownTopLevel()
{
  if (this->isAtProdRoot()) {
    this->ownTopProdLevel();
  }
  this->ownTopTermLevel();
}


void State::copyProdLevel(State *src, Int offset)
{
  ProdLevel &srcLevel = src->refProdLevel(offset);
  this->prodStack.push_back(ProdLevel());
  this->topProdLevelCache = &this->refProdLevel(-1);
  this->topProdLevelCache->setModule(srcLevel.getModule());
  this->topProdLevelCache->setProd(srcLevel.getProd());
  this->topProdLevelCache->setTermStackIndex(this->getTermLevelCount());
  this->variableStack.copyLevel(src->getVariableStack(), offset);
  this->grammarContext.switchCurrentModule(srcLevel.getModule());
  Data::Map *vars = this->grammarHelper.getSymbolVars(srcLevel.getProd());
  this->grammarContext.setCurrentArgumentList(vars);
  this->copyTermLevel(src, srcLevel.getTermStackIndex());
}


void State::copyTermLevel(State *src, Int offset)
{
  this->termStack.push_back(TermLevel());
  this->topTermLevelCache = &this->refTermLevel(-1);
  this->topTermLevelCache->copyFrom(&src->refTermLevel(offset));
}


/**
 * This value is used for optimization purposes. It's used to delay the copying
 * of build msg objects as much as possible after a state branch. This can be
 * useful because in many cases we might not need to copy those build msgs at
 * all (build msgs gets flushed from the trunk, or this branched state dies).
 */
void State::setTrunkSharedBuildMsgCount(Int count)
{
  if (this->trunkState == 0) {
    throw GeneralException(STR("No trunk state set for this state."),
                           STR("Core::Parser::State::setTrunkSharedBuildMsgCount"));
  }
  if (count < 0 || count > this->trunkState->getBuildMsgCount()) {
    throw InvalidArgumentException(STR("count"),
                                   STR("Core::Parser::State::setTrunkSharedBuildMsgCount"),
                                   STR("Out of range."), count);
  }
  this->trunkSharedBuildMsgCount = count;
}


/**
 * Copy the shared build msgs from the trunk state into this object then
 * reset the trunk shared build msg count.
 *
 * @sa setTrunkSharedBuildMsgCount
 */
void State::copyTrunkSharedBuildMsgs()
{
  if (this->trunkState == 0) {
    throw GeneralException(STR("No trunk state set for this state."),
                           STR("Core::Parser::State::copyTrunkSharedBuildMsgs"));
  }
  ASSERT(this->trunkSharedBuildMsgCount >= 0 &&
         this->trunkSharedBuildMsgCount <= this->trunkState->getTrunkSharedBuildMsgCount());
  this->buildMsgs.insert(this->buildMsgs.begin(),
                          this->trunkState->buildMsgs.begin(),
                          this->trunkState->buildMsgs.begin()+this->trunkSharedBuildMsgCount);
  this->trunkSharedBuildMsgCount = 0;
}

} } // namespace

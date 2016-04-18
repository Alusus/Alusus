/**
 * @file Core/Processing/ParserState.cpp
 * Contains the implementation of Processing::ParserState.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

//==============================================================================
// Constructor

ParserState::ParserState() :
  trunkState(0),
  tempTrunkTermStackIndex(-1),
  tempTrunkProdStackIndex(-1),
  topTermLevelCache(0),
  topProdLevelCache(0),
  processingStatus(ParserProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ParserProcessingStatus::IN_PROGRESS),
  tokensToLive(-1),
  errorSyncBlockPairs(0),
  decisionNodeIndex(-1)
{
  this->grammarContext.setRoot(0);
  this->grammarContext.setStack(&this->variableStack);
  this->dataStack.setOwningEnabled(true);
}


ParserState::ParserState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
             Word reservedVarCount, Word reservedVarLevelCount, Data::GrammarModule *rootModule) :
  trunkState(0),
  tempTrunkTermStackIndex(-1),
  tempTrunkProdStackIndex(-1),
  topTermLevelCache(0),
  topProdLevelCache(0),
  termStack(reservedTermLevelCount),
  dataStack(maxVarNameLength, reservedTermLevelCount),
  prodStack(reservedProdLevelCount),
  variableStack(maxVarNameLength, reservedVarCount, reservedVarLevelCount),
  processingStatus(ParserProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ParserProcessingStatus::IN_PROGRESS),
  tokensToLive(-1),
  errorSyncBlockPairs(0),
  decisionNodeIndex(-1)
{
  this->termStack.resize(0);
  this->prodStack.resize(0);
  this->grammarContext.setRoot(rootModule);
  this->grammarContext.setStack(&this->variableStack);
  this->dataStack.setOwningEnabled(true);
}


ParserState::ParserState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
             Word reservedVarCount, Word reservedVarLevelCount, const Data::GrammarContext *context) :
  trunkState(0),
  tempTrunkTermStackIndex(-1),
  tempTrunkProdStackIndex(-1),
  topTermLevelCache(0),
  topProdLevelCache(0),
  termStack(reservedTermLevelCount),
  dataStack(maxVarNameLength, reservedTermLevelCount),
  prodStack(reservedProdLevelCount),
  variableStack(maxVarNameLength, reservedVarCount, reservedVarLevelCount),
  processingStatus(ParserProcessingStatus::IN_PROGRESS),
  prevProcessingStatus(ParserProcessingStatus::IN_PROGRESS),
  tokensToLive(-1),
  errorSyncBlockPairs(0),
  decisionNodeIndex(-1)
{
  this->termStack.resize(0);
  this->prodStack.resize(0);
  this->grammarContext.copyFrom(context);
  this->grammarContext.setStack(&this->variableStack);
  this->dataStack.setOwningEnabled(true);
}


//==============================================================================
// Term Stack Member Functions

void ParserState::initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                       Word reservedVarCount, Word reservedVarLevelCount, Data::GrammarModule *rootModule)
{
  ASSERT(reservedTermLevelCount > 0);
  ASSERT(reservedProdLevelCount > 0);
  ASSERT(maxVarNameLength > 1);
  ASSERT(reservedVarCount > 0);
  ASSERT(reservedVarLevelCount > 0);

  this->termStack.reserve(reservedTermLevelCount);
  this->prodStack.reserve(reservedProdLevelCount);
  this->dataStack.initialize(maxVarNameLength, reservedTermLevelCount);
  this->termStack.clear();
  this->dataStack.clear();
  this->prodStack.clear();
  this->errorSyncBlockStack.clear();
  this->variableStack.initialize(maxVarNameLength, reservedVarCount, reservedVarLevelCount);
  this->grammarContext.setRoot(rootModule);
  this->grammarContext.setStack(&this->variableStack);
  this->grammarContext.setArgs(0);
}


void ParserState::initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                       Word reservedVarCount, Word reservedVarLevelCount,
                       const Data::GrammarContext *context)
{
  ASSERT(reservedTermLevelCount > 0);
  ASSERT(reservedProdLevelCount > 0);
  ASSERT(maxVarNameLength > 1);
  ASSERT(reservedVarCount > 0);
  ASSERT(reservedVarLevelCount > 0);

  this->termStack.reserve(reservedTermLevelCount);
  this->prodStack.reserve(reservedProdLevelCount);
  this->dataStack.initialize(maxVarNameLength, reservedTermLevelCount);
  this->termStack.clear();
  this->dataStack.clear();
  this->prodStack.clear();
  this->errorSyncBlockStack.clear();
  this->variableStack.initialize(maxVarNameLength, reservedVarCount, reservedVarLevelCount);
  this->grammarContext.copyFrom(context);
  this->grammarContext.setStack(&this->variableStack);
}


/**
 * Delete all state levels and reset branching information.
 */
void ParserState::reset()
{
  this->processingStatus = ParserProcessingStatus::IN_PROGRESS;
  this->tokensToLive = -1;
  this->termStack.clear();
  this->dataStack.clear();
  this->prodStack.clear();
  this->variableStack.clear();
  this->buildMsgStore.clear();
  this->trunkState = 0;
  this->tempTrunkTermStackIndex = -1;
  this->tempTrunkProdStackIndex = -1;
  this->topTermLevelCache = 0;
  this->topProdLevelCache = 0;
  this->errorSyncBlockPairs = 0;
  this->errorSyncBlockStack.clear();
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
ParserTermLevel& ParserState::refTermLevel(Int i)
{
  // Validate i and convert it into positive indexing if it's currently negative.
  if (i >= 0) {
    if (i >= this->getTermLevelCount()) {
      throw EXCEPTION(GenericException, STR("This state has an empty term stack, or i is out of range."));
    }
  } else {
    if (-(i) > this->getTermLevelCount()) {
      throw EXCEPTION(GenericException, STR("Given state has an empty term stack, or i is out of range."));
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
Int ParserState::getTopprodTermLevelCount() const
{
  // The first level does not belong to any production, so we need at least 2 levels.
  if (this->getTermLevelCount() <= 1) {
    throw EXCEPTION(GenericException, STR("This state has an empty term stack."));
  }
  ASSERT(this->getProdLevelCount() > 0);
  // Find the production root, then get its data.
  return this->getTermLevelCount() - this->refTopProdLevel().getTermStackIndex();
}


/**
 * Push a new level into the state term stack and initialize its checksum
 * values.
 */
void ParserState::pushTermLevel(Data::Term *term)
{
  this->termStack.push_back(ParserTermLevel());
  this->dataStack.pushLevel(SharedPtr<IdentifiableObject>());
  this->topTermLevelCache = &this->refTermLevel(-1);

  // If we don't have any term to set, then we also won't have any related info to cache.
  if (term == 0) return;

  // Set level info.
  this->topTermLevelCache->setTerm(term);
  // Cache term parameters for faster access later.
  if (term->isA<Data::TokenTerm>()) {
    Data::TokenTerm *tokenTerm = static_cast<Data::TokenTerm*>(term);
    this->topTermLevelCache->getParam1()->object = this->grammarContext.getTokenTermId(tokenTerm);
    this->topTermLevelCache->getParam2()->object = this->grammarContext.getTokenTermText(tokenTerm);
  } else if (term->isDerivedFrom<Data::ListTerm>()) {
    Data::ListTerm *list_term = static_cast<Data::ListTerm*>(term);
    this->grammarContext.getListTermData(list_term, *(this->topTermLevelCache->getParam1()));
  } else if (term->isA<Data::MultiplyTerm>()) {
    Data::MultiplyTerm *multiplyTerm = static_cast<Data::MultiplyTerm*>(term);
    this->topTermLevelCache->getParam1()->object = this->grammarContext.getMultiplyTermMax(multiplyTerm);
    this->topTermLevelCache->getParam2()->object = this->grammarContext.getMultiplyTermMin(multiplyTerm);
    this->topTermLevelCache->getParam3()->object = this->grammarContext.getMultiplyTermPriority(multiplyTerm);
  }
}


/**
 * Remove a state level from the top of the term stack. This function puts into
 * consideration branching information and adjusts them if needed. If this
 * state's term stack is empty and this state has branched from another state,
 * the trunk level index will be decremented.
 */
void ParserState::popTermLevel()
{
  ASSERT(!this->isAtProdRoot());
  if (this->termStack.size() > 0) {
    this->termStack.pop_back();
    ASSERT(this->dataStack.getLevelCount() > 0);
    this->dataStack.popLevel();
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


Int ParserState::findProdLevel(Int termIndex) const
{
  if (termIndex < 0) termIndex += this->getTermLevelCount();
  if (this->prodStack.size() == 0) return -1;
  if (termIndex < this->refProdLevel(0).getTermStackIndex()) return -1;
  if (termIndex >= this->refTopProdLevel().getTermStackIndex()) {
    return this->getProdLevelCount()-1;
  }
  return this->_findProdLevel(termIndex, 0, this->getProdLevelCount()-2);
}


Int ParserState::_findProdLevel(Int termIndex, Int start, Int end) const
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


//==============================================================================
// Production Stack Member Functions

ParserProdLevel& ParserState::refProdLevel(Int i)
{
  // Validate i and convert it into positive indexing if it's currently negative.
  if (i >= 0) {
    if (i >= this->getProdLevelCount()) {
      throw EXCEPTION(GenericException, STR("This state has an empty production stack, or i is out of range."));
    }
  } else {
    if (-(i) > this->getProdLevelCount()) {
      throw EXCEPTION(GenericException, STR("Given state has an empty production stack, or i is out of range."));
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


void ParserState::pushProdLevel(Data::Module *module, Data::SymbolDefinition *prod)
{
  // TODO: Once we switch to exclusively using GrammarModule in the grammar, change the following
  //       code to update the error sync block pairs everytime the module changes and the new
  //       module has a different value for errorSyncBlockPairsRef.
  if (this->topProdLevelCache == 0) {
    this->errorSyncBlockPairs = this->grammarContext.getAssociatedErrorSyncBlockPairs(module);
  }

  this->prodStack.push_back(ParserProdLevel());
  this->topProdLevelCache = &this->prodStack.back();
  this->topProdLevelCache->setModule(module);
  this->topProdLevelCache->setProd(prod);
  this->topProdLevelCache->setTermStackIndex(this->getTermLevelCount());
  this->variableStack.pushLevel();
  this->grammarContext.setModule(module);
  this->grammarContext.setArgs(this->grammarContext.getSymbolVars(prod));
  this->pushTermLevel(this->grammarContext.getSymbolTerm(prod));
}


void ParserState::popProdLevel()
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
    this->grammarContext.setModule(this->topProdLevelCache->getModule());
    Data::SharedMap *vars = this->grammarContext.getSymbolVars(this->topProdLevelCache->getProd());
    this->grammarContext.setArgs(vars);
  }
  else {
    this->topProdLevelCache = 0;
  }

  this->popTermLevel();
}


//==============================================================================
// Term Helper Functions

Word ParserState::getListTermChildCount(Int levelOffset) const
{
  const ParserTermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isDerivedFrom<Data::ListTerm>());
  return this->grammarContext.getListTermChildCount(static_cast<Data::ListTerm*>(level->getTerm()),
                                                    *(level->getParam1()));
}


Data::Term* ParserState::useListTermChild(Int index, Int levelOffset)
{
  ParserTermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isDerivedFrom<Data::ListTerm>());
  Data::Term *term;
  this->grammarContext.useListTermChild(static_cast<Data::ListTerm*>(level->getTerm()),
                                        index, *(level->getParam1()), term, level->getParam2());
  return term;
}


Data::Integer* ParserState::getTokenTermId(Int levelOffset) const
{
  const ParserTermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::TokenTerm>());
  return static_cast<Data::Integer*>(level->getParam1()->object);
}


IdentifiableObject* ParserState::getTokenTermText(Int levelOffset) const
{
  const ParserTermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::TokenTerm>());
  return level->getParam2()->object;
}


void ParserState::getReferencedSymbol(Data::Module *&module, Data::SymbolDefinition *&definition,
                                      Int levelOffset)
{
  Data::Reference *ref;
  if (levelOffset == -1) {
    ASSERT(this->refTopTermLevel().getTerm()->isA<Data::ReferenceTerm>());
    ref = static_cast<Data::ReferenceTerm*>(this->refTopTermLevel().getTerm())->getReference().get();
  } else {
    ASSERT(this->refTermLevel(levelOffset).getTerm()->isA<Data::ReferenceTerm>());
    ref = static_cast<Data::ReferenceTerm*>(this->refTermLevel(levelOffset).getTerm())->getReference().get();
  }
  this->grammarContext.getReferencedSymbol(ref, module, definition);
}


Data::Integer* ParserState::getMultiplyTermMax(Int levelOffset) const
{
  const ParserTermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::MultiplyTerm>());
  return static_cast<Data::Integer*>(level->getParam1()->object);
}


Data::Integer* ParserState::getMultiplyTermMin(Int levelOffset) const
{
  const ParserTermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::MultiplyTerm>());
  return static_cast<Data::Integer*>(level->getParam2()->object);
}


Data::Integer* ParserState::getMultiplyTermPriority(Int levelOffset) const
{
  const ParserTermLevel *level;
  if (levelOffset == -1) level = &this->refTopTermLevel();
  else level = &this->refTermLevel(levelOffset);
  ASSERT(level->getTerm()->isA<Data::MultiplyTerm>());
  return static_cast<Data::Integer*>(level->getParam3()->object);
}


//==============================================================================
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
void ParserState::setBranchingInfo(ParserState *ts, Int ttl, Int tsi, Int psi)
{
  ASSERT(ts != 0 || (ttl == -1 && tsi == -1 && psi == -1));
  ASSERT(ttl >= -1 && tsi >= -1 && psi >= -1);
  this->reset();
  this->trunkState = ts;
  this->tempTrunkTermStackIndex = tsi;
  this->tempTrunkProdStackIndex = psi;
  if (tsi >= 0) {
    this->topTermLevelCache = &this->refTermLevel(tsi);
    this->dataStack.setBranchingInfo(ts->getDataStack(), tsi);
  } else {
    this->dataStack.setBranchingInfo(0, -1);
  }
  if (psi >= 0) {
    this->topProdLevelCache = &this->refProdLevel(psi);
    this->variableStack.setBranchingInfo(ts->getVariableStack(), psi);
    if (ts->getProdLevelCount() == psi+1) {
      this->grammarContext.copyFrom(ts->getGrammarContext());
      this->grammarContext.setStack(&this->variableStack);
    } else {
      this->grammarContext.setModule(this->topProdLevelCache->getModule());
      Data::SharedMap *vars = this->grammarContext.getSymbolVars(this->topProdLevelCache->getProd());
      this->grammarContext.setArgs(vars);
    }
  } else {
    this->variableStack.setBranchingInfo(0, -1);
  }
  this->buildMsgStore.setTrunkStore(ts->getBuildMsgStore());
  this->tokensToLive = ttl;
}


/**
 * Decrement the tokens-to-live counter, if this state branched from another
 * state.
 *
 * @return Returns true if the counter reaches 0, false otherwise.
 */
bool ParserState::decrementTokensToLive()
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


void ParserState::ownTopTermLevel()
{
  ASSERT(this->getTermLevelCount() > 0);
  if (this->termStack.size() > 0) return;
  ASSERT(this->trunkState != 0);
  ASSERT(this->tempTrunkTermStackIndex >= 0);
  if (static_cast<Int>(this->trunkState->getTermLevelCount()) <= this->tempTrunkTermStackIndex) {
    throw EXCEPTION(GenericException, STR("Trunk state has been modified."));
  }
  ParserTermLevel &srcLevel = this->trunkState->refTermLevel(this->tempTrunkTermStackIndex);
  this->tempTrunkTermStackIndex--;
  this->termStack.push_back(ParserTermLevel());
  this->topTermLevelCache = &this->refTermLevel(-1);
  this->refTopTermLevel().copyFrom(&srcLevel);
}


void ParserState::ownTopProdLevel()
{
  ASSERT(this->getProdLevelCount() > 0);
  if (this->prodStack.size() > 0) return;
  ASSERT(this->trunkState != 0);
  ASSERT(this->tempTrunkProdStackIndex >= 0);
  ParserProdLevel &srcLevel = this->trunkState->refProdLevel(this->tempTrunkProdStackIndex);
  this->tempTrunkProdStackIndex--;
  this->prodStack.push_back(ParserProdLevel());
  this->topProdLevelCache = &this->refProdLevel(-1);
  this->refTopProdLevel().setModule(srcLevel.getModule());
  this->refTopProdLevel().setProd(srcLevel.getProd());
  this->refTopProdLevel().setTermStackIndex(srcLevel.getTermStackIndex());
}


void ParserState::ownTopLevel()
{
  if (this->isAtProdRoot()) {
    this->ownTopProdLevel();
  }
  this->ownTopTermLevel();
}


void ParserState::copyProdLevel(ParserState *src, Int offset)
{
  ParserProdLevel &srcLevel = src->refProdLevel(offset);
  this->prodStack.push_back(ParserProdLevel());
  this->topProdLevelCache = &this->refProdLevel(-1);
  this->topProdLevelCache->setModule(srcLevel.getModule());
  this->topProdLevelCache->setProd(srcLevel.getProd());
  this->topProdLevelCache->setTermStackIndex(this->getTermLevelCount());
  this->variableStack.copyLevel(src->getVariableStack(), offset);
  this->grammarContext.setModule(srcLevel.getModule());
  Data::SharedMap *vars = this->grammarContext.getSymbolVars(srcLevel.getProd());
  this->grammarContext.setArgs(vars);
  this->copyTermLevel(src, srcLevel.getTermStackIndex());
}


void ParserState::copyTermLevel(ParserState *src, Int offset)
{
  this->termStack.push_back(ParserTermLevel());
  this->dataStack.pushLevel(src->getData(offset));
  this->topTermLevelCache = &this->refTermLevel(-1);
  this->topTermLevelCache->copyFrom(&src->refTermLevel(offset));
}

} } // namespace

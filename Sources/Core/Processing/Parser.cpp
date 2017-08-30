/**
 * @file Core/Processing/Parser.cpp
 * Contains the implementation of class Core::Processing::Parser.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

// TODO: Use the cached term parameters from term levels.

//==============================================================================
// Member Functions

void Parser::initialize(Data::GrammarRepository *grammarRepo, SharedPtr<Data::Ast::Scope> rootScope)
{
  // Before we can change the production list, we need to make sure we have no outstanding
  // states.
  if (!this->states.empty()) {
    throw EXCEPTION(GenericException, STR("Grammar manager can't be changed while being in use."));
  }

  // TODO: If we currently have a grammar manager, we need to unset it first.
  //if (this->production_definitions != 0) {
  //    this->production_definitions->production_in_use_inquirer.unconnect(this, &Parser::is_production_in_use);
  //}

  // Set the repositories.
  this->grammarRepository = grammarRepo;
  this->rootScope = rootScope;

  // TODO: If we have a new manager, we need to set the production_in_use_inquirer signal.
  //if (this->production_definitions != 0) {
  //    this->production_definitions->production_in_use_inquirer.connect(this, &Parser::is_production_in_use);
  //}

  Data::GrammarModule *root = this->grammarRepository->getPlainRoot();
  if (root == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("mgr"),
                    STR("Provided manager doesn't contain a GrammarModule root."));
  }

  this->tempState.initialize(RESERVED_PARSER_TERM_LEVEL_COUNT, RESERVED_PARSER_PRODUCTION_LEVEL_COUNT,
                             VARIABLE_NAME_MAX_LENGTH, RESERVED_VARIABLE_COUNT, RESERVED_VARIABLE_LEVEL_COUNT,
                             root);
}


/**
 * Create a state with the main state level then add a level pointing to the
 * program root production (the parsing tree's root). The new state will be
 * created in the states stack. The states stack will be cleared before creating
 * the new state.
 *
 * @note The main state level is used by the parsing handlers to store their
 *       data. This level is needed because the data eventually need to be
 *       stored at a level that doesn't get released even when parsing folds
 *       out of the grammar tree. The data can't be processed and released at
 *       that point until the state machine makes sure no other state is lying
 *       around since only one state should be considered at the end.
 *
 * @sa clear()
 */
void Parser::beginParsing()
{
  // Validation.
  if (this->grammarRepository == 0) {
    throw EXCEPTION(GenericException, STR("Grammar repository is not set."));
  }
  Data::GrammarModule *rootModule = this->grammarRepository->getPlainRoot();
  if (rootModule == 0) {
    throw EXCEPTION(GenericException, STR("No root grammar module is found."));
  }
  if (rootModule->getStartRef() == 0) {
    throw EXCEPTION(GenericException, STR("Grammar root production is not set."));
  }

  // Delete all states, if any.
  this->clear();

  // Create the new state.
  StateIterator si = this->createState();
  (*si)->setPrevProcessingStatus(ParserProcessingStatus::COMPLETE);
  // Initialize the main level.
  (*si)->pushTermLevel(0);
  // Initialize the program root level.
  Data::Module *module;
  Data::SymbolDefinition *prod;
  (*si)->getGrammarContext()->getReferencedSymbol(rootModule->getStartRef().get(), module, prod);
  if (!prod->isA<Data::SymbolDefinition>()) {
    throw EXCEPTION(GenericException, STR("Reference term is pointing to a target of a wrong type."));
  }
  if (prod->getTerm() == 0) {
    throw EXCEPTION(GenericException, STR("Formula of root production isn't set yet."));
  }
  this->pushStateProdLevel(*si, module, static_cast<Data::SymbolDefinition*>(prod), 0);

  this->unexpectedTokenNoticeRaised = false;
}


/**
 * Fold available states out of the grammar tree and select the highest priority
 * one among them. Folding out is done by poping out levels whose terms'
 * conditions are met. For example, a concat term is considered pass only if its
 * position is pointing past the end element. Duplicate terms is considered pass
 * if their min occurances condition is met. If min occurances is -1 it'll be
 * considered pass since -1 means: infinite until program completion.
 * Selecting which state to take is done as follows:<br>
 * -If there is one or more complete state (states that successfully folded out
 *  of the grammar tree) we'll take the highest priority state among them.<br>
 * -If there is no complete states, but waiting states (states that weren't in
 *  an error state caused by receiving a wrong token, but couldn't fold out of
 *  the grammar tree), we'll take the one with the highest priority.<br>
 * -If there is only error states, we'll take the one with the highest priority.
 * Once we are left with only one state, we'll try to force it out of the
 * grammar tree by raising syntax errors. If a state was in a waiting status
 * (no error token, but couldn't fold out successfully) we'll still consider it
 * an error state. So we raise an error and fold out the state to an error sync
 * position, then try to fold out again. If we still couldn't fold out of the
 * grammar tree completely, we repeat the cycle until we fold out completely.
 *
 * @return Returns a pointer to the parsing data object remaining after the
 *         parsing is complete. This can be 0 if the parsing handlers chose
 *         to remove all parsing data upon level/production end.
 */
SharedPtr<TiObject> Parser::endParsing()
{
  // Validation.
  if (this->states.empty()) {
    throw EXCEPTION(GenericException, STR("There isn't any state currently."));
  }

  // Fold out the levels stack of existing states.
  StateIterator si;
  bool anyWaitingState = false;
  bool anyCompleteState = false;
  for (si = this->states.begin(); si != this->states.end(); ++si) {
    this->tryCompleteFoldout(si);
    ASSERT((*si)->getProcessingStatus() != ParserProcessingStatus::IN_PROGRESS);
    if ((*si)->getProcessingStatus() != ParserProcessingStatus::ERROR) {
      if ((*si)->getTermLevelCount() == 1) anyCompleteState = true;
      else anyWaitingState = true;
    }
  }
  // Select only one of the available states.
  if (anyCompleteState) {
    // If there is any complete states we should take the first of them.
    while (this->states.front()->getProcessingStatus() != ParserProcessingStatus::COMPLETE ||
           this->states.front()->getTermLevelCount() != 1) {
      this->deleteState(this->states.begin(), ParserStateTerminationCause::NOT_NEEDED_ANYMORE);
    }
  } else if (anyWaitingState) {
    // If there is any waiting state and no complete states, we should take the first of the waiting states.
    while (this->states.front()->getProcessingStatus() != ParserProcessingStatus::COMPLETE)
      this->deleteState(this->states.begin(), ParserStateTerminationCause::NOT_NEEDED_ANYMORE);
  }
  // Now the first state in the list is the one we need, so we can delete the rest.
  si = this->states.end();
  --si;
  while (si != this->states.begin()) {
    this->deleteState(si, ParserStateTerminationCause::NOT_NEEDED_ANYMORE);
    si = this->states.end();
    --si;
  }

  // Now we are left with only one state. Is this an error state?
  si = this->states.begin();
  if ((*si)->getProcessingStatus() != ParserProcessingStatus::COMPLETE || (*si)->getTermLevelCount() != 1) {
    // Even if the state was waiting we'll consider it to fail since a successful state should
    // fold out successfully.
    (*si)->setProcessingStatus(ParserProcessingStatus::ERROR);
    // We'll keep looping and raising errors until we fold out of the grammar tree completely.
    do {
      // Raise error notification event, but only if state has been waiting for more tokens.
      if ((*si)->getTermLevelCount() > 1) {
        if (!this->getTopParsingHandler(*si)->onErrorToken(this, *si, 0)) {
          (*si)->addNotice(SharedPtr<Data::Notice>(new UnexpectedEofNotice()));
        }
      }
      // First, move the state to an error sync position.
      // Note that here we'll move to the position immediately after the error sync position
      // because we dont want to wait for the sync token.
      while ((*si)->getTermLevelCount() > 1) {
        if ((*si)->refTopTermLevel().getTerm()->isA<Data::ConcatTerm>()) {
          Data::ConcatTerm *concatTerm =
              static_cast<Data::ConcatTerm*>((*si)->refTopTermLevel().getTerm());
          Int errorSyncPosId = concatTerm->getErrorSyncPosId();
          if (errorSyncPosId >= static_cast<Int>((*si)->refTopTermLevel().getPosId())) {
            // Here there is no point in waiting for tokens, so we'll jump to the end of the concat list
            // regardless of the sync pos id.
            (*si)->setTopTermPosId((*si)->getListTermChildCount());
            break;
          } else {
            Data::Integer *flags = (*si)->getTermFlags();
            if ((flags == 0 ? 0 : flags->get()) & Data::TermFlags::ERROR_SYNC_TERM) {
              break;
            }
          }
        }
        this->popStateLevel(*si, false);
      }
      // Now let's try again to fold out of the grammar tree.
      this->tryCompleteFoldout(si);
    } while ((*si)->getTermLevelCount() > 1);
  }

  // Finally, we are left with only one chosen state which has folded out of the grammar tree.

  // Flush build messages.
  this->flushApprovedNotices();

  // Return remaining parsing data after clearing the remaining state.
  SharedPtr<TiObject> data = (*this->states.begin())->getData();
  this->clear();
  return data;
}


/**
 * Try to take the upper route and ignore any optional inner routes until we hit
 * an incomplete level or fold out completely.
 */
void Parser::tryCompleteFoldout(StateIterator si)
{
  if ((*si)->getTermLevelCount() <= 1) return;
  Data::Token token;
  token.setId(this->EOF_TOKEN);
  this->tempState.reset();
  this->tempState.setBranchingInfo(*si, -1);
  this->testState(&token, &this->tempState);
  if (this->tempState.getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
    while ((*si)->getTermLevelCount() > this->tempState.getTermLevelCount()) {
      this->popStateLevel(*si, true);
    }
  }
}


/**
 * Process a new token by updating all the states in the states stack according
 * to the given token. This function is invoked for every token passed by the
 * lexer.<br>
 * State dropping and selection procedures:<br>
 * -If there is error state(s) and at least a single successful state, we'll
 *  drop all error states.<br>
 * -If a state folds out of the grammar tree without having other successful
 *  states ahead of it (higher priority), we'll take that state and consider
 *  parsing complete. If more than one state fold out completely, the first one
 *  (highest priority) will be considered.<br>
 * -If a state folds out of the grammar tree while other successful states exist
 *  with higher priority, the state that folded out will be dropped.<br>
 * -If a state branches, the branch with the lower priority will be given a TTL
 *  (time to live) value. This value is in tokens. If that number of tokens gets
 *  processed without any of the two branches erroring out, the lower priority
 *  branch will be dropped.<br>
 * -If two states meet at exactly the same position (same position id along the
 *  entire level stack) the lower priority one will be dropped.<br>
 * -If we end up with error state(s), we'll raise an error event and start
 *  popping out levels until we reach a concat term level that has an error sync
 *  position and we'll stop at that position. This will allow us to wait for a
 *  sync character (like a ; for example) to complete parsing.
 *
 * @param token A pointer to the token object to process.
 */
void Parser::handleNewToken(Data::Token const *token)
{
  // Validation.
  if (token == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("token"), STR("token is null."));
  }

  // The caller should call beginParsing before calling handleNewToken, and should not call handleNewToken
  // after parsing folds out of the production tree.
  if (this->states.empty() || this->states.front()->getTermLevelCount() == 0)
    throw EXCEPTION(GenericException, STR("Parser::beginParsing should be called before calling handleNewToken"));
  else if (this->states.front()->getTermLevelCount() == 1) {
    // Raise an unexpected token error.
    if (!this->unexpectedTokenNoticeRaised) {
      this->states.front()->addNotice(
            SharedPtr<Data::Notice>(new UnexpectedTokenNotice(*token->getSourceLocation())));
      this->unexpectedTokenNoticeRaised = true;
    }
    return;
  }

  // Reset the processing status of all states.
  StateIterator si;
  for (si = this->states.begin(); si != this->states.end(); si++) {
    (*si)->setPrevProcessingStatus((*si)->getProcessingStatus());
    (*si)->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
  }

  this->decisionNodes.clear();

  // Process the states.
  bool anyWaitingState = false;
  bool anyCompleteState = false;
  bool anyErrorState = false;
  for (si = this->states.begin(); si != this->states.end(); si++) {
    // If this state is already processed, skip.
    if ((*si)->getProcessingStatus() != ParserProcessingStatus::IN_PROGRESS) continue;
    // Process the token.
    this->processState(token, si);
    // Did we find any successful state?
    if ((*si)->getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
      if ((*si)->getTermLevelCount() == 1) anyCompleteState = true;
      else anyWaitingState = true;
    } else
      anyErrorState = true;
  }

  // If there is at least one successful state, then we should delete all error states.
  if ((anyWaitingState || anyCompleteState) && anyErrorState) {
    for (si = this->states.begin(); si != this->states.end(); ++si) {
      if (si == this->states.begin()) continue;
      if ((*si)->getProcessingStatus() == ParserProcessingStatus::ERROR) {
        StateIterator si2 = si;
        --si;
        this->deleteState(si2, ParserStateTerminationCause::SYNTAX_ERROR);
        LOG(LogLevel::PARSER_MID, STR("Deleting unneeded error state."));
      }
    }
    if (this->states.front()->getProcessingStatus() == ParserProcessingStatus::ERROR) {
      this->deleteState(this->states.begin(), ParserStateTerminationCause::SYNTAX_ERROR);
    }
  }

  // Do we have any complete states to consider or drop?
  if (anyCompleteState) {
    if (this->states.front()->getTermLevelCount() == 1) {
      // The highest priority state has folded out of the parsing tree, so
      // we'll take that as the final state and drop all others.
      for (si = this->states.begin(); si != this->states.end(); ++si) {
        if (si == this->states.begin()) continue;
        StateIterator si2 = si;
        --si;
        this->deleteState(si2, ParserStateTerminationCause::NOT_NEEDED_ANYMORE);
        LOG(LogLevel::PARSER_MID, STR("Deleting unneeded successful state."));
      }
    } else {
      // The highest priority state is still in processing, so we don't
      // need the state(s) that has folded out of the parsing tree.
      for (si = this->states.begin(); si != this->states.end(); ++si) {
        if (si == this->states.begin()) continue;
        if ((*si)->getTermLevelCount() == 1) {
          StateIterator si2 = si;
          --si;
          this->deleteState(si2, ParserStateTerminationCause::FOLDED_OUT_TOO_SOON);
          LOG(LogLevel::PARSER_MID, STR("Deleting unneeded successful state."));
        }
      }
    }
  }

  // Drop states that outlived it's tokensToLive value.
  for (si = this->states.begin(); si != this->states.end(); ++si) {
    if (si == this->states.begin()) continue;
    if ((*si)->decrementTokensToLive()) {
      StateIterator si2 = si;
      --si;
      this->deleteState(si2, ParserStateTerminationCause::CONSUMED_TOKENS_TO_LIVE);
      LOG(LogLevel::PARSER_MID, STR("Deleting expired successful state."));
    }
  }

  // Drop states if they are at the same exact position.
  for (si = this->states.begin(); si != this->states.end(); ++si) {
    StateIterator si2 = si;
    si2++;
    for (; si2 != this->states.end(); ++si2) {
      if (this->compareStates(*si, *si2)) {
        // The two states are at the exact position, so delete the one with the lower
        // priority, which is si2.
        StateIterator si3 = si2;
        --si2;
        this->deleteState(si3, ParserStateTerminationCause::MERGED_WITH_HIGHER_PRIORITY_STATE);
        LOG(LogLevel::PARSER_MID, STR("Deleting duplicate successful state."));
      }
    }
  }

  // Flush any approved build messages.
  this->flushApprovedNotices();

  // Did we reach the end of parsing?
  si = this->states.begin();
  if ((*si)->getProcessingStatus() == ParserProcessingStatus::COMPLETE && (*si)->getTermLevelCount() == 1) {
    // At this point there should be only one state, which is the complete state.
    ASSERT(this->states.size() == 1);
    // Fire a parsing completion signal.
    this->parsingCompleted.emit();
  }
}


/**
 * Approved build messages are messages in the root state that are shared by
 * all other states (if any). If there is only one state, all messages in that
 * state are approved. Messages that are generated after a branch happens will
 * wait until all other branches are cancelled before they are considered
 * approved.
 */
void Parser::flushApprovedNotices()
{
  ASSERT(this->states.size() > 0);

  // Find the number of approved messages.
  StateIterator si = this->states.begin();
  Int count = (*si)->getNoticeStore()->getCount();
  for (++si; si != this->states.end() && count > 0; ++si) {
    if ((*si)->getTrunkState() == this->states.front()) {
      if ((*si)->getNoticeStore()->getTrunkSharedCount() < count) {
        count = (*si)->getNoticeStore()->getTrunkSharedCount();
      }
    }
  }
  if (count == 0) return;

  // Now emit the messages.
  for (Int i = 0; i < count; ++i) {
    this->noticeSignal.emit(this->states.front()->getNoticeStore()->get(i));
  }
  this->states.front()->getNoticeStore()->flush(count);

  // Update the shared message count.
  si = this->states.begin();
  for (++si; si != this->states.end(); ++si) {
    if ((*si)->getTrunkState() == this->states.front()) {
      (*si)->getNoticeStore()->setTrunkSharedCount((*si)->getNoticeStore()->getTrunkSharedCount()-count);
    }
  }
}


/**
 * Apply the given token to the given state until we can no longer advance
 * within that state. Applying the token is done by continuously calling
 * process_state_level to process the deepest level in the level stack
 * until the state is no longer in IN_PROGRESS status.
 *
 * @param token A pointer to the token to apply on the state.
 * @param si The iterator of the state to process.
 */
void Parser::processState(const Data::Token * token, StateIterator si)
{
  // Have this state folded out to completion already? This can happen if the resulted from a branch at the root
  // production.
  if ((*si)->getTermLevelCount() == 1) {
    (*si)->setProcessingStatus(ParserProcessingStatus::COMPLETE);
    LOG(LogLevel::PARSER_MID, STR("Start Processing State: State already folded out to completion."));
    return;
  }

  LOG(LogLevel::PARSER_MID, STR("Start Processing State: At production ") <<
      ID_GENERATOR->getDesc((*si)->refTopProdLevel().getProd()->getId()) <<
      STR(", level ") << (*si)->getTopprodTermLevelCount());

  Bool error = false;

  // Process the given token.
  while ((*si)->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS) {
    ASSERT((*si)->getTermLevelCount() > 0);
    // Did we fold out to completion?
    if ((*si)->getTermLevelCount() == 1) {
      (*si)->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      break;
    }
    // If the previous state was an error, it means we are waiting to sync. But we'll need to check
    // block pairs so that we only sync when we are at the same block level rather than inside a
    // deeper level.
    if ((*si)->getPrevProcessingStatus() == ParserProcessingStatus::ERROR &&
        this->matchErrorSyncBlockPairs(*si, token)) {
      // We are in a deeper block (or just left that deeper block) so we'll assume we are still in
      // error.
      (*si)->setProcessingStatus(ParserProcessingStatus::ERROR);
    } else {
      // We are not waiting to sync, or we are waiting to sync but we are already on the same block
      // level, so we'll proceed with matching the token normally.
      Data::Term *term = (*si)->refTopTermLevel().getTerm();
      if (term->isA<Data::TokenTerm>()) {
        this->processTokenTerm(token, si);
      } else if (term->isA<Data::MultiplyTerm>()) {
        this->processMultiplyTerm(token, si);
      } else if (term->isA<Data::AlternateTerm>()) {
        this->processAlternateTerm(token, si);
      } else if (term->isA<Data::ConcatTerm>()) {
        this->processConcatTerm(token, si);
      } else if (term->isA<Data::ReferenceTerm>()) {
        this->processReferenceTerm(token, si);
      } else {
        // Invalid state type.
        StrStream stream;
        stream << STR("Invalid term type while processing ")
               << ID_GENERATOR->getDesc((*si)->refTopProdLevel().getProd()->getId())
               << STR(". Found Term Type: ")
               << term->getTypeInfo()->getTypeName();
        throw EXCEPTION(GenericException, stream.str().c_str());
      }
    }
    // Did the state error out?
    if ((*si)->getProcessingStatus() == ParserProcessingStatus::ERROR && error == false) {
      error = true;
      // Raise error notification event if we haven't already.
      if ((*si)->getPrevProcessingStatus() != ParserProcessingStatus::ERROR) {
        if (!this->getTopParsingHandler(*si)->onErrorToken(this, *si, token)) {
          (*si)->addNotice(
            SharedPtr<Data::Notice>(new SyntaxErrorNotice(*token->getSourceLocation()))
          );
        }
        // Move the state to an error sync position.
        while ((*si)->getTermLevelCount() > 1) {
          if ((*si)->refTopTermLevel().getTerm()->isA<Data::ConcatTerm>()) {
            Int errorSyncPosId =
                static_cast<Data::ConcatTerm*>
                ((*si)->refTopTermLevel().getTerm())->getErrorSyncPosId();
            if (errorSyncPosId >= static_cast<Int>((*si)->refTopTermLevel().getPosId())) {
              Int cnt = (*si)->getListTermChildCount();
              if (errorSyncPosId > cnt) errorSyncPosId = cnt;
              (*si)->setTopTermPosId(errorSyncPosId);
              break;
            }
          } else {
            Data::Integer *flags = (*si)->getTermFlags();
            if ((flags == 0 ? 0 : flags->get()) & Data::TermFlags::ERROR_SYNC_TERM) {
              break;
            }
          }
          this->popStateLevel(*si, false);
        }
        // We need to match the error block pairs in case the error token itself is the opening of
        // a block (a bracket for example).
        this->matchErrorSyncBlockPairs(*si, token);
      }
      // Set the status back to IN_PROGRESS for now (we'll be setting it back to ERROR later).
      (*si)->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
    }
  }

  // Set the status back to error in case we overrode that during the attempt to sync with error position.
  if (error) {
    (*si)->setProcessingStatus(ParserProcessingStatus::ERROR);
  }

  // Reset any THIS_PROCESSING_PASS flag found in term level indexes.
  for (Int i = 0; i < (*si)->getTermLevelCount(); i++) {
    ParserTermLevel &termLevel = (*si)->refTermLevel(i);
    if (termLevel.getPosId() & THIS_PROCESSING_PASS) {
      (*si)->setTermPosId(termLevel.getPosId()&(~THIS_PROCESSING_PASS), i);
    }
  }

  // Close out levels that are complete.
  if ((*si)->getProcessingStatus() == ParserProcessingStatus::COMPLETE && (*si)->getTermLevelCount() > 1) {
    while ((*si)->getTermLevelCount() > 1) {
      // Determine if this level is complete.
      bool levelComplete = false;
      Data::Term *term = (*si)->refTopTermLevel().getTerm();
      if (term->isA<Data::ConcatTerm>()) {
        if (static_cast<Int>((*si)->refTopTermLevel().getPosId()) >=
            (*si)->getListTermChildCount()) {
          levelComplete = true;
        }
      } else if (term->isA<Data::MultiplyTerm>()) {
        Data::Integer *maxOccurances = (*si)->getMultiplyTermMax();
        if (maxOccurances != 0 &&
            static_cast<Int>((*si)->refTopTermLevel().getPosId()) >= maxOccurances->get()) {
          levelComplete = true;
        }
      } else {
        if ((*si)->refTopTermLevel().getPosId() != 0) levelComplete = true;
      }
      // Close the complete level or quit loop.
      if (levelComplete)
        this->popStateLevel(*si, true);
      else
        break;
    }
  }
}


/**
 * Apply the given token on a token term within the given state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * If the position id value is 1, that means the token has already been
 * completed so we will remove this level and move up, otherwise we will compare
 * the token and if the comparison is successful we'll set the status to COMPLETE
 * and the position id to 1, otherwise we will set the status to ERROR.
 *
 * @param token A pointer to the token to apply to the state.
 * @param si The iterator of the state being processed.
 */
void Parser::processTokenTerm(const Data::Token * token, StateIterator si)
{
  ASSERT((*si)->refTopTermLevel().getTerm()->isA<Data::TokenTerm>());
  // Are we starting with this token, or are we done with it?
  if ((*si)->refTopTermLevel().getPosId() == 0) {
    // Make sure we have some token id.
    Word matchId = (*si)->getTokenTermId()->get();
    TiObject *matchText = (*si)->getTokenTermText();
    if (matchId == UNKNOWN_ID && matchText == 0) {
      throw EXCEPTION(GenericException, STR("Token term's match id isn't assigned yet."));
    }
    // Match this token with the token term.
    Bool matched = this->matchToken(matchId, matchText, token);
    if (matched) {
      // Fire parsing handler event.
      this->getTopParsingHandler(*si)->onNewToken(this, *si, token);
      // Processing of this state is complete.
      (*si)->setTopTermPosId(1|THIS_PROCESSING_PASS);
      (*si)->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      LOG(LogLevel::PARSER_MID, STR("Process State: Token accepted (") <<
          ID_GENERATOR->getDesc(matchId) << STR(":") <<
          token->getText() << STR(")"));
    }
    if (!matched) {
      // Processing of this state has errored out.
      (*si)->setProcessingStatus(ParserProcessingStatus::ERROR);
      #ifdef USE_LOGS
        Data::String *matchStr = tio_cast<Data::String>(matchText);
      #endif
      LOG(LogLevel::PARSER_MID, STR("Process State: Token failed (") <<
          ID_GENERATOR->getDesc(matchId) << STR(":") <<
          (matchStr==0?"":matchStr->get()) << STR(") -- Received (") <<
          ID_GENERATOR->getDesc(token->getId()) << STR(":") <<
          token->getText() << STR(")"));
    }
  } else {
    // We are already done with this token, so move back to the upper level after
    // removing the current state level.
    this->popStateLevel(*si, true);
  }
}


/**
 * Apply the received token on a duplicate term within the given state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * Call the computePossibleMultiplyRoutes to determine the possible routes
 * to take. If no route was found to match the given token, the processing
 * status will be set to error. If one is found, the route will be followed
 * immediately. If two routes are found, the state will be duplicated with each
 * one of them set to follow one of the routes (one to to deeper and one to go
 * up).
 *
 * @param token A pointer to the token to apply to the state.
 * @param si The iterator of the state being processed.
 */
void Parser::processMultiplyTerm(const Data::Token * token, StateIterator si)
{
  ASSERT((*si)->refTopTermLevel().getTerm()->isA<Data::MultiplyTerm>());
  Data::MultiplyTerm *multiplyTerm =
      static_cast<Data::MultiplyTerm*>((*si)->refTopTermLevel().getTerm());
  Data::Integer *priority = (*si)->getMultiplyTermPriority();
  // Make sure we have a child term.
  Data::Term *childTerm = multiplyTerm->getTerm().get();
  if (childTerm == 0) {
    throw EXCEPTION(GenericException, STR("Multiply term's child term isn't set yet."));
  }
  // Compute the possible routes to take from here.
  this->computePossibleMultiplyRoutes(token, *si);
  Int successCount = 0;
  Int decisionIndex1 = (*si)->getDecisionNodeIndex();
  Int decisionIndex2 = -1;
  if (decisionIndex1 != -1) {
    ++successCount;
    decisionIndex2 = this->decisionNodes.getSiblingIndex(decisionIndex1);
    if (decisionIndex2 != -1) ++ successCount;
  }

  if (successCount == 0) {
    // Processing of this state has errored out.
    (*si)->setProcessingStatus(ParserProcessingStatus::ERROR);
    LOG(LogLevel::PARSER_MID, STR("Process State: No possible routes at multiply term."));
  } else if (successCount == 1 || (*si)->getTermLevelCount() == 2) {
    // If we are at the root level and we have more than one possible route, we only need to take the higher
    // priority one since the other route would be dropped at the handleNewToken() function anyway.
    if (decisionIndex2 != -1 && priority != 0 && priority->get() == this->decisionNodes.getPosId(decisionIndex2)) {
      decisionIndex1 = decisionIndex2;
    }
    // Fire parsing handler event.
    Int successRoute = this->decisionNodes.getPosId(decisionIndex1);
    this->getTopParsingHandler(*si)->onMultiplyRouteDecision(this, *si, successRoute, token);
    // Take the selected route.
    if (successRoute == 1) {
      // Increment the level index (iteration count).
      (*si)->setTopTermPosId(((*si)->refTopTermLevel().getPosId()+1)|THIS_PROCESSING_PASS);
      // Take the inner route.
      this->pushStateTermLevel(*si, childTerm, 0, token);
      LOG(LogLevel::PARSER_MID, STR("Process State: Taking multiply route."));
    } else {
      // Take the up route.
      this->popStateLevel(*si, true);
      LOG(LogLevel::PARSER_MID, STR("Process State: Rejecting multiply route."));
    }
    (*si)->setDecisionNodeIndex(this->decisionNodes.getChildIndex(decisionIndex1));
  } else if (successCount == 2) {
    // Fire a branching parsing handler event.
    ParsingHandler *parsingHandler = this->getTopParsingHandler(*si);
    parsingHandler->onBranching(this, *si, token);
    // Duplicate the state.
    (*si)->addNotice(
      SharedPtr<Data::Notice>(new AmbiguityNotice(*token->getSourceLocation()))
    );
    // TODO: Grab the value of tokensToLive from the grammer instead of always using the
    //       default value.
    StateIterator si2 = this->duplicateState(si, token, DEFAULT_TOKENS_TO_LIVE);
    // Fire a branched parsing handler event.
    // TODO: Should this event be raised now, or after the two states are updated?
    parsingHandler->onBranched(this, *si, *si2, token);
    // Determine which of the two states will go in and which will go out.
    StateIterator inSi, outSi;
    if (this->decisionNodes.getPosId(decisionIndex1) == 0) {
      outSi = si;
      inSi = si2;
    } else {
      inSi = si;
      outSi = si2;
    }
    // Increment the level index (iteration count) of the current state.
    (*inSi)->setTopTermPosId(((*inSi)->refTopTermLevel().getPosId()+1)|THIS_PROCESSING_PASS);
    // Fire parsing handler event for the current state.
    parsingHandler->onMultiplyRouteDecision(this, *inSi, 1, token);
    // Set the current state to take the inner route.
    this->pushStateTermLevel(*inSi, childTerm, 0, token);
    // Fire parsing handler event for the duplicate state.
    parsingHandler->onMultiplyRouteDecision(this, *outSi, 0, token);
    // Set the duplicate state to take the up route.
    this->popStateLevel(*outSi, true);
    // Update the decision nodes.
    (*si)->setDecisionNodeIndex(this->decisionNodes.getChildIndex(decisionIndex1));
    (*si2)->setDecisionNodeIndex(this->decisionNodes.getChildIndex(decisionIndex2));
    LOG(LogLevel::PARSER_MID, STR("Process State: Taking both inner and outer routes for multiply term."));
  } else {
    // There shouldn't be more than 2 routes at all.
    ASSERT(false);
  }
}


/**
 * Apply the received token on an alternative term within the given state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * When parsing reaches the alternative term for the first time, the
 * computePossibleAlternativeRoutes function will be called to determine the
 * possible routes to take. If no route was found to match the given token, the
 * processing status will be set to error. If one is found, the route will be
 * followed immediately. If more than one route is found, the state will be
 * duplicated with each instance set to follow one of the routes. If it's not
 * the first time to reach this alternative term, then we have finished going
 * through one of the alternate routes and will simply pass execution to the
 * upper level.
 *
 * @param token A pointer to the token to apply to the state.
 * @param si The iterator of the state being processed.
 */
void Parser::processAlternateTerm(Data::Token const *token, StateIterator si)
{
  ASSERT((*si)->refTopTermLevel().getTerm()->isA<Data::AlternateTerm>());
  // Make sure we have some child terms attached.
  if ((*si)->getListTermChildCount() < 1) {
    throw EXCEPTION(GenericException, STR("Alternate term doesn't have any branches yet (zero children)."));
  }
  if ((*si)->refTopTermLevel().getPosId() == 0) {
    // We are entering the term for the first time.
    this->computePossibleAlternativeRoutes(token, *si);
    Int decisionIndex = (*si)->getDecisionNodeIndex();
    Int successRoute = decisionIndex == -1 ? -1 : this->decisionNodes.getPosId(decisionIndex);
    Int nextDecisionIndex = this->decisionNodes.getSiblingIndex(decisionIndex);

    if (successRoute == -1) {
      // Processing of this state has errored out.
      (*si)->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MID, STR("Process State: No possible routes at alternate term."));
    } else if (nextDecisionIndex == -1) {
      // Fire parsing handler event.
      this->getTopParsingHandler(*si)->onAlternateRouteDecision(this, *si, successRoute-1, token);
      // Take the selected route.
      (*si)->setTopTermPosId(successRoute|THIS_PROCESSING_PASS);
      Data::Term *childTerm = (*si)->useListTermChild(successRoute-1);
      this->pushStateTermLevel(*si, childTerm, 0, token);
      (*si)->setDecisionNodeIndex(this->decisionNodes.getChildIndex(decisionIndex));
      LOG(LogLevel::PARSER_MID, STR("Process State: Taking only one alternate route (") <<
          successRoute << STR(")."));
    } else {
      #ifdef USE_LOGS
        Int routeCount = 1;
        Int loopDecisionIndex = nextDecisionIndex;
        while (loopDecisionIndex != -1) {
          ++routeCount;
          loopDecisionIndex = this->decisionNodes.getSiblingIndex(loopDecisionIndex);
        }
        LOG(LogLevel::PARSER_MID, STR("Process State: Taking ") << routeCount <<
            STR(" alternate routes."));
      #endif
      // Grab the parsing handler.
      ParsingHandler *parsingHandler = this->getTopParsingHandler(*si);
      // Set the current state to take the first route, then start branching the other routes from it.
      StateIterator si2 = si;
      while (nextDecisionIndex != -1) {
        Int branchRoute = this->decisionNodes.getPosId(nextDecisionIndex);
        // Fire a branching parsing handler event.
        parsingHandler->onBranching(this, *si2, token);
        // Duplicate the state, without the top level since that one is for accessing the
        // other route.
        (*si2)->addNotice(
          SharedPtr<Data::Notice>(new AmbiguityNotice(*token->getSourceLocation()))
        );
        // TODO: Grab the value of tokensToLive from the grammer instead of always using the
        //       default value.
        StateIterator newSi = this->duplicateState(si2, token, DEFAULT_TOKENS_TO_LIVE, (*si2)->getTermLevelCount()-1);
        if ((*si2)->isAtProdRoot()) {
          this->pushStateProdLevel(*newSi, (*si2)->refTopProdLevel().getModule(),
                                   (*si2)->refTopProdLevel().getProd(), token);
        } else {
          this->pushStateTermLevel(*newSi, (*si2)->refTopTermLevel().getTerm(), 0, token);
        }
        // Fire a branched parsing handler event.
        // TODO: Should this event be raised now, or after the two states are updated?
        parsingHandler->onBranched(this, *si2, *newSi, token);
        // Fire parsing handler event for the new state.
        parsingHandler->onAlternateRouteDecision(this, *newSi, branchRoute-1, token);
        // Set the new state to take this route.
        (*newSi)->setTopTermPosId(branchRoute|THIS_PROCESSING_PASS);
        Data::Term *childTerm = (*si)->useListTermChild(branchRoute-1);
        this->pushStateTermLevel(*newSi, childTerm, 0, token);
        (*newSi)->setDecisionNodeIndex(this->decisionNodes.getChildIndex(nextDecisionIndex));
        // This state should be the trunk of the next one.
        si2 = newSi;
        LOG(LogLevel::PARSER_MID, STR("Process State: Alternate route (") <<
            branchRoute << STR(" )."));
        nextDecisionIndex = this->decisionNodes.getSiblingIndex(nextDecisionIndex);
      }
      // Fire parsing handler event for the current state.
      parsingHandler->onAlternateRouteDecision(this, *si, successRoute-1, token);
      // Set the current state to take the first route.
      (*si)->setTopTermPosId(successRoute|THIS_PROCESSING_PASS);
      Data::Term *childTerm = (*si)->useListTermChild(successRoute-1);
      this->pushStateTermLevel(*si, childTerm, 0, token);
      (*si)->setDecisionNodeIndex(this->decisionNodes.getChildIndex(decisionIndex));
    }
  } else {
    // We are already done with this term, so move back to the upper level after
    // removing the current state level.
    this->popStateLevel(*si, true);
    LOG(LogLevel::PARSER_MID, STR("Process State: Finished alternate term."));
  }
}


/**
 * Apply the received token on a concat term within the given state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * Simply increment the position id to point to the next term and move into that
 * term, until we run out of terms, then we'll go up.
 *
 * @param token A pointer to the token to apply to the state.
 * @param si The iterator of the state being processed.
 */
void Parser::processConcatTerm(Data::Token const *token, StateIterator si)
{
  ASSERT((*si)->refTopTermLevel().getTerm()->isA<Data::ConcatTerm>());
  // Make sure we have a child term.
  Word termCount = (*si)->getListTermChildCount();
  /*if (termCount == 0) {
    throw EXCEPTION(GenericException, STR("Concat term's child terms aren't set yet."));
  }*/
  Int posId = (*si)->refTopTermLevel().getPosId() & (~THIS_PROCESSING_PASS);
  // Did we finish all the terms in this list?
  if (static_cast<Word>(posId) < termCount) {
    // Fire parsing handler event.
    this->getTopParsingHandler(*si)->onConcatStep(this, *si, posId, token);
    // Increment the position id of this level.
    (*si)->setTopTermPosId((posId+1)|THIS_PROCESSING_PASS);
    // Move to the term pointed by position id.
    Data::Term *childTerm = (*si)->useListTermChild(posId);
    this->pushStateTermLevel(*si, childTerm, 0, token);
    LOG(LogLevel::PARSER_MID, STR("Process State: Processing concat term (") << (posId+1) << STR(")."));
  } else {
    // We are done with this list, so move to the upper level.
    this->popStateLevel(*si, true);
    LOG(LogLevel::PARSER_MID, STR("Process State: Finished concat term."));
  }
}


/**
 * Apply the received token on a reference term within the given state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * If we reach this for the first time (posId == 0), create the new level
 * refering to the reference term, otherwise (posId == 1) just move to the
 * upper level. If the reference head pointer is not set in the reference term,
 * we will search for it in the production definitions list and set it in the
 * reference term (the reference term has the id, not the pointer, and for the
 * sake of performance we store the pointer inside the reference term when
 * access it for the first time to avoid searching by id every time we access
 * the term).
 *
 * @param token A pointer to the token to apply to the state.
 * @param si The iterator of the state being processed.
 */
void Parser::processReferenceTerm(Data::Token const *token, StateIterator si)
{
  ASSERT((*si)->refTopTermLevel().getTerm()->isA<Data::ReferenceTerm>());
  // Are we starting with this term, or are we done with it?
  if ((*si)->refTopTermLevel().getPosId() == 0) {
    // We are starting with this token.
    // Get the referenced module and definition.
    Data::Module *module;
    Data::SymbolDefinition *definition;
    (*si)->getReferencedSymbol(module, definition);
    if (!definition->isA<Data::SymbolDefinition>()) {
      throw EXCEPTION(GenericException, STR("Reference term is pointing to a target of a wrong type."));
    }
    if (definition->getTerm() != 0) {
      // Update the current level's position id.
      (*si)->setTopTermPosId(1|THIS_PROCESSING_PASS);
      // Create the new state level.
      this->pushStateProdLevel(*si, module, static_cast<Data::SymbolDefinition*>(definition), token);
      LOG(LogLevel::PARSER_MID, STR("Process State: Processing referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << STR(")."));
    } else {
      // An empty production is encountered.
      (*si)->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MID, STR("Process State: Erroring on empty referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << STR(")."));
    }
  } else {
    // We are already done with this token, so move back to the upper level after
    // removing the current state level.
    this->popStateLevel(*si, true);
    LOG(LogLevel::PARSER_MID, STR("Process State: Done with referenced production. Back to (") <<
        ID_GENERATOR->getDesc((*si)->refTopProdLevel().getProd()->getId())
        << STR(")."));
  }
}


/**
 * Delete all states currently in the stack and release all their data. The
 * states will be gracefully deleted and the associated parsing handlers will
 * be informed.<br>
 * Production definitions will not be affected.
 */
void Parser::clear()
{
  while (!this->states.empty()) {
    this->deleteState(this->states.begin(), ParserStateTerminationCause::UNKNOWN);
  }
}


/**
 * Compute the list of possible routes to take at a duplicate term and store the
 * found routes in decisionNodes. Checking for a possible route is done by
 * creating a temporary state that branches from the given state and trying all
 * possible routes on that temp state. Trying a single route is done by setting
 * the state levels of the temp state to the given route and calling testState
 * to see whether the temp state ends up in COMPLETE or ERROR.
 *
 * @param token A pointer to the token against which to test the routes.
 * @param state A pointer to the state object from which to start testing the
 *              routes.
 */
void Parser::computePossibleMultiplyRoutes(Data::Token const *token, ParserState *state)
{
  // Get the multiply term.
  Data::MultiplyTerm *multiplyTerm =
      static_cast<Data::MultiplyTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(multiplyTerm->isA<Data::MultiplyTerm>());
  Data::Integer *minOccurances = state->getMultiplyTermMin();
  Data::Integer *maxOccurances = state->getMultiplyTermMax();
  Data::Integer *priority = state->getMultiplyTermPriority();
  Data::Integer *flags = state->getTermFlags();
  Bool oneRoute = ((flags == 0 ? 0 : flags->get()) & Data::TermFlags::ONE_ROUTE_TERM);

  // NOTE: An empty inner route is treated as an error. So, if the inner route
  //       was found empty and the minimum occurances haven't been met yet the
  //       result will be no possible routes to take. In other words, an empty
  //       inner route won't be counted as one occurance.

  Int testedRoute = -1;
  Int decisionIndex = state->getDecisionNodeIndex();
  if (decisionIndex != -1) {
    while (this->decisionNodes.getSiblingIndex(decisionIndex) != -1) {
      decisionIndex = this->decisionNodes.getSiblingIndex(decisionIndex);
    }
    testedRoute = this->decisionNodes.getPosId(decisionIndex);
  }

  if (testedRoute == 0) {
    return;
  } else if (priority != 0 && priority->get() == 0) {
    Bool success = false;
    if (minOccurances == 0 || static_cast<Int>(state->refTopTermLevel().getPosId()) >= minOccurances->get()) {
      success = this->computeOuterMultiplyRoute(token, state, multiplyTerm, priority);
    }
    if (success && oneRoute) return;
    if (testedRoute == -1) {
      if (maxOccurances == 0 || static_cast<Int>(state->refTopTermLevel().getPosId()) <  maxOccurances->get()) {
        this->computeInnerMultiplyRoute(token, state, multiplyTerm);
      }
    }
  } else {
    Bool success = false;
    if (testedRoute == -1) {
      if (maxOccurances == 0 || static_cast<Int>(state->refTopTermLevel().getPosId()) <  maxOccurances->get()) {
        success = this->computeInnerMultiplyRoute(token, state, multiplyTerm);
      }
    } else {
      success = true;
    }
    if (success & oneRoute) return;
    if (minOccurances == 0 || static_cast<Int>(state->refTopTermLevel().getPosId()) >= minOccurances->get()) {
      this->computeOuterMultiplyRoute(token, state, multiplyTerm, priority);
    }
  }
}


Bool Parser::computeInnerMultiplyRoute(Data::Token const *token, ParserState *state, Data::MultiplyTerm *multiplyTerm)
{
  // Initialize the temp state.
  this->tempState.reset();
  this->tempState.setBranchingInfo(state, -1);
  // Replace the current state level with a new one.
  this->tempState.ownTopLevel();
  this->tempState.setTopTermPosId(1|THIS_PROCESSING_PASS);
  // Create the deeper level.
  this->tempState.pushTermLevel(multiplyTerm->getTerm().get());
  // Test the temp state.
  Int decisionNodeIndex = this->testState(token, &this->tempState);
  // Store results.
  if (this->tempState.getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
    this->decisionNodes.addSiblingNode(state, 1, decisionNodeIndex);
    return true;
  } else {
    return false;
  }
}


Bool Parser::computeOuterMultiplyRoute(Data::Token const *token, ParserState *state, Data::MultiplyTerm *multiplyTerm, Data::Integer *priority)
{
  // Initialize the temp state.
  this->tempState.reset();
  this->tempState.setBranchingInfo(state, -1);
  // Move up.
  this->tempState.popLevel();
  // Test the temp state.
  Int decisionNodeIndex = this->testState(token, &this->tempState);
  // Store results.
  if (this->tempState.getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
    // Does the outer route lead to complete fold out of the parsing tree?
    if (this->tempState.getTermLevelCount() == 1) {
      // Outer route leads to completion, so if the priority is for the inner route then ignore this
      // successful status, otherwise take it and ignore the successful inner route if any.
      if (priority != 0 && priority->get() == 0) {
        state->setDecisionNodeIndex(-1);
        this->decisionNodes.addSiblingNode(state, 0, decisionNodeIndex);
        return true;
      } else {
        return false;
      }
    } else {
      this->decisionNodes.addSiblingNode(state, 0, decisionNodeIndex);
      return true;
    }
  } else {
    return false;
  }
}


/**
 * Compute the list of possible routes to take at an alternative term and store
 * the found routes in decisionNodes. Checking for a possible route is done by
 * creating a temporary state that branches from the given state and trying all
 * possible routes on that temp state. Trying a single route is done by setting
 * the state levels of the temp state to the given route and calling testState
 * to see whether the temp state ends up in COMPLETE or ERROR.
 *
 * @param token A pointer to the token against which to test the routes.
 * @param state A pointer to the state object from which to start testing the
 *              routes.
 */
void Parser::computePossibleAlternativeRoutes(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::AlternateTerm>());

  Data::Integer *flags = state->getTermFlags();
  Bool oneTerm = ((flags == 0 ? 0 : flags->get()) & Data::TermFlags::ONE_ROUTE_TERM);

  Int testedRoute = 0;
  Int decisionIndex = state->getDecisionNodeIndex();
  if (decisionIndex != -1) {
    while (this->decisionNodes.getSiblingIndex(decisionIndex) != -1) {
      decisionIndex = this->decisionNodes.getSiblingIndex(decisionIndex);
    }
    testedRoute = this->decisionNodes.getPosId(decisionIndex);
  }

  if (testedRoute != 0 && oneTerm) return;

  Word termCount = state->getListTermChildCount();
  if (termCount == 1) {
    // We only have one route, so we don't need any testing.
    if (testedRoute == 0) {
      this->decisionNodes.addSiblingNode(state, 1);
    }
  } else {
    // Loop on all terms in the list of alternative terms.
    for (Int i = testedRoute; static_cast<Word>(i) < termCount; i++) {
      // Initialize the temp state branching from the current one.
      this->tempState.reset();
      this->tempState.setBranchingInfo(state, -1);
      // Own the top level so we don't modify the trunk state's level.
      this->tempState.ownTopLevel();
      this->tempState.setTopTermPosId((i+1)|THIS_PROCESSING_PASS);
      // Create the deeper level.
      this->tempState.pushTermLevel(state->useListTermChild(i));
      // Test the temp state.
      Int decisionNodeIndex = this->testState(token, &this->tempState);
      // Store results.
      if (this->tempState.getProcessingStatus()==ParserProcessingStatus::COMPLETE) {
        this->decisionNodes.addSiblingNode(state, i+1, decisionNodeIndex);
        if (oneTerm) break;
      }
    }
  }
}


/**
 * Test the given token on the given state to see whether the token can be
 * accepted by the given state. Applying the token is done by continuously
 * calling testStateLevel to process the deepest level in the level stack
 * until the state is no longer in IN_PROGRESS status.
 * This function is used in places where a decision need to be made about
 * which route(s) the parser should take.
 *
 * @param token A pointer to the token to apply on the state.
 * @param state A pointer to the state to test.
 */
Int Parser::testState(Data::Token const *token, ParserState *state)
{
  ASSERT(state != 0);

  state->setDecisionNodeIndex(-1);

  // Process state levels.
  while (state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS) {
    // Did we fold out to completion?
    if (state->getTermLevelCount() == 1) {
      state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      break;
    }
    // Test the current state level.
    this->testStateLevel(token, state);
    // In case of errors, check if we can take another route somewhere up.
    while (state->getProcessingStatus() == ParserProcessingStatus::ERROR && state->getTermLevelCount() > 2) {
      // Remove the current level.
      state->popLevel();
      // Is this level a multi-route level that has been set during the test?
      if (state->refTopTermLevel().getPosId() & THIS_TESTING_PASS) {
        // This is an intersection set during this testing pass, so we can try a different
        // route here.
        this->testStateLevel(token, state);
      } else {
        // We got out of the testing chain of state levels, so continuing at this stage won't
        // lead us anywhere.
        return -1;
      }
    }
  }

  Int index = state->getDecisionNodeIndex();
  if (index != -1) {
    while (this->decisionNodes.getParentIndex(index) != -1) {
      index = this->decisionNodes.getParentIndex(index);
    }
  }
  return index;
}


/**
 * Test a single token during the route testing pass. This funciton is called
 * by testState to test whether a route taken by a single state is successful.
 * This function will be continuously called by testState until the state ends
 * up in either COMPLETE or ERROR.
 *
 * @param token A pointer to the token to test against the state.
 * @param state A pointer to the state being tested.
 */
void Parser::testStateLevel(Data::Token const *token, ParserState *state)
{
  ASSERT(state != 0);

  // check the term type
  Data::Term *term = state->refTopTermLevel().getTerm();
  if (term->isA<Data::TokenTerm>()) {
    this->testTokenTerm(token, state);
  } else if (term->isA<Data::MultiplyTerm>()) {
    this->testMultiplyTerm(token, state);
  } else if (term->isA<Data::AlternateTerm>()) {
    this->testAlternateTerm(token, state);
  } else if (term->isA<Data::ConcatTerm>()) {
    this->testConcatTerm(token, state);
  } else if (term->isA<Data::ReferenceTerm>()) {
    this->testReferenceTerm(token, state);
  } else {
    // Invalid state type.
    StrStream stream;
    stream << STR("Invalid term type while testing ")
           << ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId())
           << STR(". Found Term Type: ")
           << term->getTypeInfo()->getTypeName();
    throw EXCEPTION(GenericException, stream.str().c_str());
  }
}


/**
 * Test the given token against a token term within the test state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * If the position id value is 1, that means the token has already been
 * completed so we will remove this level and move up, otherwise we will compare
 * the token and if the comparison is successful we'll set the status to
 * COMPLETE and the position id to 1, otherwise we will set the status to ERROR.
 *
 * @param token A pointer to the token to test against the state.
 * @param state A pointer to the state being tested.
 */
void Parser::testTokenTerm(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::TokenTerm>());
  // Are we starting with this token, or are we done with it?
  if (state->refTopTermLevel().getPosId() == 0) {
    // We are checking this token.
    Word matchId = state->getTokenTermId()->get();
    TiObject *matchText = state->getTokenTermText();
    if (matchId == UNKNOWN_ID && matchText == 0) {
      throw EXCEPTION(GenericException, STR("Token term's match id isn't assigned yet."));
    }
    Bool matched = this->matchToken(matchId, matchText, token);
    if (matched) {
      // Processing of this state is complete.
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Matched for token (") <<
          ID_GENERATOR->getDesc(matchId) << STR(":") <<
          token->getText() << STR(")"));
    }
    if (!matched) {
      // Processing of this state has errored out.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      #ifdef USE_LOGS
        Data::String *matchStr = tio_cast<Data::String>(matchText);
      #endif
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Failed for token (") <<
          ID_GENERATOR->getDesc(matchId) << STR(":") <<
          (matchStr==0?"":matchStr->get()) << STR(") -- Received (") <<
          ID_GENERATOR->getDesc(token->getId()) << STR(":") <<
          token->getText() << STR(")"));
    }
  } else {
    // We are already done with this token, so move back to the upper level after
    // removing the current state level.
    state->popLevel();
  }
}


/**
 * Test against a duplicate term within the test state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * If we reach this again in the same testing pass or parsing pass we'll
 * consider the route to fail in order to avoid infinite loops. If term was
 * visited during the same testing pass then we can try the other route. If the
 * term is visited during the same parsing pass we'll not try the other route
 * because it has been tried already when it was first visited. If the term was
 * not visited during this pass, then we can try the duplicate term route again
 * (after checking the max occurances). If we do, we need to recreate the state
 * level (pop the level and then push a new one) in order to avoid modifying the
 * trunk state's data.
 *
 * @param token A pointer to the token to test against the state.
 * @param state A pointer to the state being tested.
 */
void Parser::testMultiplyTerm(Data::Token const *token, ParserState *state)
{
  Data::MultiplyTerm *multiplyTerm =
      static_cast<Data::MultiplyTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(multiplyTerm->isA<Data::MultiplyTerm>());
  Data::Integer *minOccurances = state->getMultiplyTermMin();
  Data::Integer *maxOccurances = state->getMultiplyTermMax();

  Int count;
  if (state->refTopTermLevel().getPosId() == 0) {
    // We are reaching this for the first time.
    ASSERT(maxOccurances == 0 || maxOccurances->get() >= 0);
    // Try moving deeper first.
    if (maxOccurances != 0 && maxOccurances->get() == 0) {
      state->popLevel();
      this->decisionNodes.addNode(state, 0);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Skipping disabled inner multiply route (max == 0)."));
    } else {
      Data::Term *childTerm = multiplyTerm->getTerm().get();
      ASSERT(childTerm != 0);
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      state->pushTermLevel(childTerm);
      this->decisionNodes.addNode(state, 1);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Trying inner multiply route (0 iterations so far)."));
    }
  } else if (state->refTopTermLevel().getPosId() & THIS_TESTING_PASS) {
    // We have reached this before during this testing pass, which
    // means the inner route ended without finding a successful
    // match. We'll consider the inner route to fail even if the
    // status is still IN_PROGRESS since we don't want infinite
    // loops.
    count = state->refTopTermLevel().getPosId() & (~THIS_TESTING_PASS);
    // Have we finished at least the minimum count (not counting the attempt that just failed)?
    if (minOccurances == 0 || count > minOccurances->get()) {
      // We can ignore the failing iteration since we have already
      // finished the minimum required.
      state->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
      state->popLevel();
      this->decisionNodes.removeNode(state);
      this->decisionNodes.addNode(state, 0);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Trying outer multiply route (") <<
          count << STR(" iterations completed)."));
    } else {
      // We haven't finished the minimum required iterations, so
      // we'll fail this state.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      this->decisionNodes.removeNode(state);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Failed for multiply term (") <<
          count << STR(" iterations completed)."));
    }
  } else if (state->refTopTermLevel().getPosId() & THIS_PROCESSING_PASS) {
    // We have reached this before during this parsing pass, which
    // means the other route has already been tried, so we'll
    // consider this to fail regardless of the status in order to
    // avoid infinite loops.
    state->setProcessingStatus(ParserProcessingStatus::ERROR);
    this->decisionNodes.removeNode(state);
    LOG(LogLevel::PARSER_MINOR, STR("Testing State: Failed for multiply term (already visited)."));
  } else {
    // We have reached this before, but not during this parsing pass
    // so we'll try running through the inner route again if
    // possible.
    // The status must be IN_PROGRESS here.
    ASSERT(state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS);
    // Have we finished the maximum count?
    ASSERT(maxOccurances == 0 || maxOccurances->get() >= 0);
    count = state->refTopTermLevel().getPosId();
    if (maxOccurances != 0 && count >= maxOccurances->get()) {
      // We reached the max count, so we'll move up instead of
      // trying the inner route again.
      state->popLevel();
      this->decisionNodes.addNode(state, 0);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Max iterations reached for multiply term (") <<
          count << STR(" iterations)."));
    } else {
      // We will need to own the top level so we don't modify the trunk state's level.
      Int posId = state->refTopTermLevel().getPosId();
      state->ownTopLevel();
      state->setTopTermPosId((posId+1)|THIS_TESTING_PASS);
      // Now create the deeper level.
      Data::Term *childTerm = multiplyTerm->getTerm().get();
      ASSERT(childTerm != 0);
      state->pushTermLevel(childTerm);
      this->decisionNodes.addNode(state, 1);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Trying inner multiply route (") <<
          count << STR(" iterations so far)."));
    }
  }
}


/**
 * Test against an alternative term within the test state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * If we reach this term for the first time we'll jump into the first route. If
 * we reach it again during this testing pass (either an error or an empty loop)
 * then we'll try the next route, or we'll move up if no other route is
 * available. If we found at least one empty route and no successful routes,
 * we'll set the processing status to IN_PROGRESS since an empty route can be
 * accepted. If we reach this term again, but not during the same pass, we'll
 * just treat it as a single route term (we can't try the other routes because
 * they have already been tried before) and we'll just pass control to the upper
 * level.
 *
 * @param token A pointer to the token to test against the state.
 * @param state A pointer to the state being tested.
 */
void Parser::testAlternateTerm(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::AlternateTerm>());
  if (state->refTopTermLevel().getPosId() == 0) {
    // If we don't have any terms we should treat this as a failed route.
    if (state->getListTermChildCount() == 0) {
      // Processing of this state has errored out.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Failing an empty alternate term."));
    } else {
      // We are entering this term for the first time, so try the
      // first route.
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      Data::Term *childTerm = state->useListTermChild(0);
      state->pushTermLevel(childTerm);
      this->decisionNodes.addNode(state, 1);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Trying alternate route (1)."));
    }
  } else if (state->refTopTermLevel().getPosId() & THIS_TESTING_PASS) {
    // We have visited this term during the testing pass, we'll try
    // the other routes first. If none of the routes is successful
    // and at least one of them finished with status=IN_PROGRESS
    // (empty route) then we'll return IN_PROGRESS.
    Int index = state->refTopTermLevel().getPosId() & (~THIS_TESTING_PASS);
    // Do we have an empty loop, whether this one or previous one?
    Bool emptyLoop = false;
    Word termCount = state->getListTermChildCount();
    ASSERT(termCount > 0);
    if (static_cast<Word>(index) > termCount) {
      emptyLoop = true;
      index -= termCount;
    }
    if (state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS) {
      emptyLoop = true;
    }
    // If this very attempt was an empty loop, we'll keep the decision made and branch,
    // otherwise we'll delete the decision and try a new one.
    if (state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS) {
      this->decisionNodes.moveToCurrentNode(state);
    } else {
      this->decisionNodes.removeNode(state);
    }
    if (static_cast<Word>(index) < termCount) {
      // Try the next route.
      // If we have had an empty loop before, mark that somehow in the index.
      state->setTopTermPosId(((emptyLoop?index+termCount:index)+1)|THIS_TESTING_PASS);
      Data::Term *childTerm = state->useListTermChild(index);
      state->pushTermLevel(childTerm);
      this->decisionNodes.addNode(state, index+1);
      // Return the status to IN_PROGRESS to give a chance for the other routes.
      state->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Trying alternate route (") << (index+1) << STR(")."));
    } else {
      // We are out of routes, so return to the upper level.
      // If we have had an emptyLoop, we should set the status to
      // IN_PROGRESS.
      if (emptyLoop) {
        state->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
        state->popLevel();
        ASSERT(this->decisionNodes.getChildIndex(state) != -1);
        state->setDecisionNodeIndex(this->decisionNodes.getChildIndex(state));
        LOG(LogLevel::PARSER_MINOR, STR("Testing State: Alternate route has an empty loop. Try upper route."));
      } else {
        LOG(LogLevel::PARSER_MINOR, STR("Testing State: Failed to match an alternate route."));
      }
    }
  } else {
    // We haven't visited this term during this test, so we don't need to try the other
    // routes since they are already tested before. So just pass control to the upper
    // level.
    ASSERT(state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS);
    state->popLevel();
    LOG(LogLevel::PARSER_MINOR, STR("Testing State: Exiting alternate term (not tested in this pass)."));
  }
}


/**
 * Test against a concat term within the test state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * If the THIS_TESTING_PASS flag is not set, then we'll need to pop this level
 * and push a new one to avoid modifying the trunk state's data.
 * Simply increment the position id to point to the next term and move into that
 * term, until we run out of terms, then we'll go up.
 *
 * @param token A pointer to the token to test against the state.
 * @param state A pointer to the state being tested.
 */
void Parser::testConcatTerm(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::ConcatTerm>());
  Word termCount = state->getListTermChildCount();
  //ASSERT(termCount > 0);
  Int index;
  // Have we reached this during the testing pass?
  if (state->refTopTermLevel().getPosId() & THIS_TESTING_PASS) {
    // This might get called during an ERROR status in an attempt to search the stack
    // for an alternative route.
    if (state->getProcessingStatus() == ParserProcessingStatus::ERROR) {
      // There are no alternative routes in a concat term.
      return;
    }
    index = state->refTopTermLevel().getPosId() & (~THIS_TESTING_PASS);
  } else {
    ASSERT(state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS);
    // Whether this was reach during this parsing pass or a previous one doesn't make a difference.
    index = state->refTopTermLevel().getPosId() & (~THIS_PROCESSING_PASS);
    // Do we need to replace the trunk state level with a new one (to avoid modifying the
    // trunk state's data)? If this is a new level (index == 0) or the level reached the end
    // of the concat chain (index >= count) we won't need to replace the state level.
    if (index > 0 && static_cast<Word>(index) < termCount) {
      // Replace the trunk state level with a new one.
      state->ownTopLevel();
    }
  }
  // Increment the index of this level.
  index++;
  // Did we finish all the terms in this list?
  if (static_cast<Word>(index) > termCount) {
    // We are done with this list, so move to the upper level.
    state->popLevel();
    LOG(LogLevel::PARSER_MINOR, STR("Testing State: Finished all concat terms (") << index << STR(")."));
  } else {
    // Move to the term pointed by level index.
    state->setTopTermPosId(index | THIS_TESTING_PASS);
    Data::Term *childTerm = state->useListTermChild(index-1);
    state->pushTermLevel(childTerm);
    LOG(LogLevel::PARSER_MINOR, STR("Testing State: Trying concat term (") << index << STR(")."));
  }
}


/**
 * Test against a reference term within the test state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * If we reach this for the first time (posId == 0), create the new level
 * refering to the reference term, otherwise (posId == 1) just move to the
 * upper level. If the reference head pointer is not set in the reference term,
 * we will search for it in the production definitions list and set it in the
 * reference term (the reference term has the id, not the pointer, and for the
 * sake of performance we store the pointer inside the reference term when
 * access it for the first time to avoid searching by id every time we access
 * the term).
 *
 * @param token A pointer to the token to test against the state.
 * @param state A pointer to the state being tested.
 */
void Parser::testReferenceTerm(Data::Token const *token, ParserState *state)
{
  // Are we starting with this term, or are we done with it?
  if (state->refTopTermLevel().getPosId() == 0) {
    // We are starting with this token.
    // Get the referenced module and definition.
    Data::Module *module;
    Data::SymbolDefinition *definition;
    state->getReferencedSymbol(module, definition);
    if (!definition->isA<Data::SymbolDefinition>()) {
      throw EXCEPTION(GenericException, STR("Reference term is pointing to a target of a wrong type."));
    }
    if (definition->getTerm() != 0) {
      // Update the current level's position id.
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      // Create the new state level.
      state->pushProdLevel(module, static_cast<Data::SymbolDefinition*>(definition));
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Trying referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << STR(")."));
    } else {
      // An empty production is encountered.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MINOR, STR("Testing State: Erroring on empty referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << STR(")."));
    }
  } else {
    // This might get called during an ERROR status in an attempt to search the stack
    // for an alternative route.
    if (state->getProcessingStatus() == ParserProcessingStatus::ERROR) {
      // There are no alternative routes in a concat term.
      return;
    }
    // We are already done with this token, so move back to the upper level after
    // removing the current state level.
    state->popLevel();
    LOG(LogLevel::PARSER_MINOR, STR("Testing State: Done with referenced production. Back to (") <<
        (
          state->getProdLevelCount()==0 ?
          STR("start") :
          ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId())
        )
        << STR(")."));
  }
}


/**
 * @details Creates a state machine instance on the states stack.
 * @return Returns the iterator of the new state on the stack.
 */
Parser::StateIterator Parser::createState()
{
  Data::GrammarModule *root = this->grammarRepository->getPlainRoot();
  ParserState *state = new ParserState(RESERVED_PARSER_TERM_LEVEL_COUNT, RESERVED_PARSER_PRODUCTION_LEVEL_COUNT,
                                       VARIABLE_NAME_MAX_LENGTH,
                                       RESERVED_VARIABLE_COUNT, RESERVED_VARIABLE_LEVEL_COUNT, root);
  if (state == 0) {
    throw EXCEPTION(GenericException, STR("Out of memory."));
  }
  StateIterator si = this->states.insert(this->states.end(), state);
  return si;
}


/**
 * Duplicate the given state object and increment the associated data reference
 * counts. The function will also fire the associated parsing handler event.
 *
 * @param si The iterator of the state to duplicate.
 * @param tokensToLive The ttl value to assign to the new token. See
 *                       ParserState::setBranchingInfo() for more details.
 * @param levelCount The number of levels to copy from the source state. If
 *                    this value is -1, all levels will be copied.
 * @return Returns the iterator of the new state on the stack.
 *
 * @sa ParserState::setBranchingInfo()
 */
Parser::StateIterator Parser::duplicateState(
  StateIterator si, Data::Token const *token, Int tokensToLive, Int levelCount
) {
  ASSERT(levelCount <= (*si)->getTermLevelCount());
  ASSERT((*si)->getProdLevelCount() > 0);
  LOG(LogLevel::PARSER_MID, STR("Duplicating state."));
  // Create the new state.
  StateIterator newSi = si;
  newSi++;
  newSi = this->states.insert(newSi, new ParserState());
  // Preallocate stacks to prevent frequent reallocation.
  (*newSi)->initialize((*si)->getReservedTermLevelCount(), (*si)->getReservedProdLevelCount(),
                        (*si)->getMaxVarNameLength(), (*si)->getReservedVariableCount(),
                        (*si)->getReservedVariableLevelCount(), (*si)->getGrammarContext());
  // Set the branching info for the new token.
  (*newSi)->setBranchingInfo(*si, tokensToLive, -1, -1);
  // Set build msg info.
  (*newSi)->getNoticeStore()->setTrunkSharedCount((*si)->getNoticeStore()->getCount());
  // Copy the processing status.
  (*newSi)->setProcessingStatus((*si)->getProcessingStatus());
  (*newSi)->setPrevProcessingStatus((*si)->getPrevProcessingStatus());
  // Copy the data of the source state.
  if (levelCount == -1) levelCount = (*si)->getTermLevelCount();
  Int pi = 0;
  // We are not going to modify the source state now, so it should be safe to hold a temporary pointer to the levels.
  ParserProdLevel *prodLevel = &(*si)->refProdLevel(pi);
  ParsingHandler *parsingHandler = 0;
  for (Int i = 0; i < levelCount; i++)
  {
    if (prodLevel != 0 && prodLevel->getTermStackIndex() == i) {
      // Duplicating a prod level.
      // There shouldn't be the case where i is 0 and at the same time we are at production level.
      ASSERT(i != 0);
      // Switch the current parsing handler.
      parsingHandler = prodLevel->getProd()->getBuildHandler().s_cast_get<ParsingHandler>();
      // Fire parsing handler event and copy.
      parsingHandler->onProdLevelDuplicating(this, *si, pi, i, token);
      (*newSi)->copyProdLevel(*si, pi);
      parsingHandler->onProdLevelDuplicated(this, *si, *newSi, pi, i, token);
      // Now change to the new anticipated production level, if any.
      ++pi;
      if (pi < (*si)->getProdLevelCount()) {
        prodLevel = &(*si)->refProdLevel(pi);
      } else {
        prodLevel = 0;
      }
    } else {
      // Duplicating a term level.
      // There shouldn't be the case where i is 0 and at the same time we have a parsing handler.
      ASSERT(!(i == 0 && parsingHandler != 0));
      // Fire parsing handler event.
      if (parsingHandler != 0) parsingHandler->onTermLevelDuplicating(this, *si, pi-1, i, token);
      (*newSi)->copyTermLevel(*si, i);
      // Fire parsing handler event.
      if (parsingHandler != 0) parsingHandler->onTermLevelDuplicated(this, *si, *newSi, pi-1, i, token);
    }
  }

  return newSi;
}


/**
 * Delete the state with the specified index from the states stack.
 *
 * @param si The iterator of the state to delete.
 * @param stc The state termination cause. This value will be passed to the
 *            parsing handler.
 */
void Parser::deleteState(StateIterator si, ParserStateTerminationCause stc)
{
  // Update states that branched from this one.
  StateIterator siLoop = si;
  siLoop++;
  StateIterator si2 = siLoop;
  for (; siLoop != this->states.end(); siLoop++) {
    if ((*siLoop)->getTrunkState() == *si) {
      if (stc == ParserStateTerminationCause::CONSUMED_TOKENS_TO_LIVE) {
        // Since this state consumed the time it was allowed to live, states branching from
        // this should also be deleted.
        StateIterator si3 = siLoop;
        siLoop--;
        this->deleteState(si3, stc);
      } else {
        // Reset branching information of the child state. The child will now take the place
        // of the trunk state, so it'll have its branching info. If the state we are deleting
        // is the root state, then we'll consider the one immediately below it to be the new
        // root state.
        if ((*si)->getTrunkState() == 0 && siLoop != si2) {
          // The state being deleted is the root, and this state is not the second state,
          // so it needs to be linked to the second state now.
          (*siLoop)->setBranchingInfo(*si2, (*siLoop)->getTokensToLive(), -1, -1);
        } else {
          // The state being deleted is not the root, or this state is not the second
          // state (so it can copy the root's data coz it'll be the root anyway).
          (*siLoop)->getNoticeStore()->copyTrunkSharedNotices();
          (*siLoop)->setBranchingInfo((*si)->getTrunkState(), (*si)->getTokensToLive(), -1, -1);
          (*siLoop)->getNoticeStore()->setTrunkSharedCount((*si)->getNoticeStore()->getTrunkSharedCount());
        }
      }
    }
  }
  // We need to find the state favored to this one so we can pass it along with the event.
  ParserState *favoredState = 0;
  if ((*si)->getTrunkState() == 0) {
    // If this state hasn't branched from another state, then take the state immediately after this, if any.
    si2 = si;
    si2++;
    if (si2!= this->states.end()) favoredState = *si2;
  } else {
    // Otherwise take the trunk state as the favored one.
    favoredState = (*si)->getTrunkState();
  }
  // Loop on all levels above the pre-root and remove them.
  if ((*si)->getProdLevelCount() > 0) {
    ParsingHandler *parsingHandler = this->getTopParsingHandler(*si);
    while ((*si)->getProdLevelCount() > 0) {
      // Fire parsing handler event.
      parsingHandler->onStateCancelling(this, *si, favoredState);
      if ((*si)->isAtProdRoot()) {
        (*si)->popProdLevel();
        if ((*si)->getProdLevelCount() > 0) {
          parsingHandler = this->getTopParsingHandler(*si);
        }
      } else {
        (*si)->popTermLevel();
      }
    }
  }
  // Delete the state itself.
  delete *si;
  this->states.erase(si);
}


void Parser::pushStateTermLevel(ParserState *state, Data::Term *term, Word posId, Data::Token const *token)
{
  state->pushTermLevel(term);
  state->setTopTermPosId(posId);
  this->getTopParsingHandler(state)->onTermStart(this, state, token);
}


void Parser::pushStateProdLevel(
  ParserState *state, Data::Module *module, Data::SymbolDefinition *prod, Data::Token const *token
) {
  state->pushProdLevel(module, prod);
  this->getTopParsingHandler(state)->onProdStart(this, state, token);
}


/**
 * Pops the top level from the given state and fire the related parsing event.
 *
 * @param state The state that will drop the top level.
 * @param success Specifies whether parsing that level was successful (true) or
 *                whether we are exiting it due to an error.
 */
void Parser::popStateLevel(ParserState *state, Bool success)
{
  // We shouldn't allow deleting the main level (level 0).
  ASSERT(state->getTermLevelCount() >= 2);
  if (state->isAtProdRoot()) {
    if (success) {
      // Fire "production end" parsing handler event.
      // The top production should be already cached at this point, so we can use the cached pointer instead of
      // searching for it.
      this->getTopParsingHandler(state)->onProdEnd(this, state);
    } else {
      // Fire "cancel production" parsing handler event.
      // The top production should be already cached at this point, so we can use the cached pointer instead of
      // searching for it.
      this->getTopParsingHandler(state)->onProdCancelling(this, state);
    }
  } else {
    if (success) {
      // Fire "level end" parsing handler event.
      this->getTopParsingHandler(state)->onTermEnd(this, state);
    } else {
      // Fire "cancel level" parsing handler event.
      this->getTopParsingHandler(state)->onTermCancelling(this, state);
    }
  }
  // Grab the level's data before deleting it.
  SharedPtr<TiObject> data = state->getData();
  // Now we can remove that state level.
  state->popLevel();
  // Did we just pop the program root production?
  if (state->getTermLevelCount() == 1) {
    // We just popped the program root, so we'll assign the data to the pre-root level so it can later on be
    // returned to the parser's user.
    state->setData(data);
  } else {
    // Fire "level exit" parsing handler event.
    this->getTopParsingHandler(state)->onLevelExit(this, state, data);
  }
}


/**
 * Check to see if two states are at the same exact position within the grammar.
 * This check is done by checking the entire state levels stack and see if all
 * position identifiers are exactly the same.
 *
 * @return true if they states are equivalent (at the same spot).
 */
bool Parser::compareStates(ParserState *s1, ParserState *s2)
{
  if (s1->getTermLevelCount() != s2->getTermLevelCount()) return false;
  if (s1->getProdLevelCount() != s2->getProdLevelCount()) return false;
  for (Int i = 0; i < s1->getTermLevelCount(); ++i) {
    ParserTermLevel &l1 = s1->refTermLevel(i);
    ParserTermLevel &l2 = s2->refTermLevel(i);
    if (l1.getPosId() != l2.getPosId() || l1.getTerm() != l2.getTerm()) return false;

  }
  for (Int i = 0; i < s1->getProdLevelCount(); ++i) {
    ParserProdLevel &l1 = s1->refProdLevel(i);
    ParserProdLevel &l2 = s2->refProdLevel(i);
    if (l1.getModule() != l2.getModule() || l1.getProd() != l2.getProd()) return false;
  }
  return true;
}


/**
 * Loops through all the levels of all the states to check whether any of them
 * use the production with the given id.
 */
Bool Parser::isDefinitionInUse(Data::SymbolDefinition *definition) const
{
  if (definition == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("definition"), STR("Should not be null."));
  }
  if (this->grammarRepository == 0) return false;
  for (ConstStateIterator si = this->states.begin(); si != this->states.end(); si++) {
    for (Int i = 0; i < (*si)->getProdLevelCount(); i++) {
      if ((*si)->refProdLevel(i).getProd() == definition) return true;
    }
  }
  return false;
}


Bool Parser::matchToken(Word matchId, TiObject *matchText, Data::Token const *token)
{
  Bool matched = true;
  Data::String *matchStr = 0;
  if (matchId != 0 && matchId != token->getId()) {
    matched = false;
  }
  if (matched == true && matchText != 0) {
    if (matchText->isA<Data::String>()) {
      matchStr = static_cast<Data::String*>(matchText);
      if (matchStr->getStr() != token->getText()) matched = false;
    } else if (matchText->isA<Data::SharedMap>()) {
      if (static_cast<Data::SharedMap*>(matchText)->findIndex(token->getText().c_str()) == -1) matched = false;
    }
  }
  return matched;
}


Bool Parser::matchErrorSyncBlockPairs(ParserState *state, Data::Token const *token)
{
  if (state->getErrorSyncBlockPairs() == 0) return false;
  // TODO: To improve performance, validate the error sync pairs once per grammar, rather than
  //       on each token received.
  // The odd entries in the match pairs list are for block start, the even entries are for block end.
  for (Int i = 0; i < state->getErrorSyncBlockPairs()->getCount(); i += 2) {
    Data::TokenTerm *term = tio_cast<Data::TokenTerm>(state->getErrorSyncBlockPairs()->get(i));
    if (term == 0) {
      throw EXCEPTION(GenericException, STR("Invalid error-sync-block-pair data. "
                                            "Pair entries must be of type TokenTerm."));
    }
    Data::Integer *matchId = term->getTokenId().tio_cast_get<Data::Integer>();
    TiObject *matchText = term->getTokenText().get();
    if (this->matchToken(matchId, matchText, token)) {
      state->getErrorSyncBlockStack().push_back(i);
      return true;
    }
  }

  // No match for block opening was found. If we are not already inside a block, then we don't
  // need to check block end.
  if (state->getErrorSyncBlockStack().size() == 0) return false;

  // We are inside a block already. Let's check block end.
  Int closingIndex = state->getErrorSyncBlockStack().back()+1;
  if (closingIndex >= state->getErrorSyncBlockPairs()->getCount()) {
    throw EXCEPTION(GenericException, STR("Invalid error-sync-block-pair data. "
                                          "There must be an even number of entries in this list."));
  }
  Data::TokenTerm *term = tio_cast<Data::TokenTerm>(state->getErrorSyncBlockPairs()->get(closingIndex));
  if (term == 0) {
    throw EXCEPTION(GenericException, STR("Invalid error-sync-block-pair data. "
                                          "Pair entries must be of type TokenTerm."));
  }
  Data::Integer *matchId = term->getTokenId().tio_cast_get<Data::Integer>();
  TiObject *matchText = term->getTokenText().get();
  if (this->matchToken(matchId, matchText, token)) {
    state->getErrorSyncBlockStack().pop_back();
  }
  return true;
}


//==============================================================================
// Branch Management Functions

Bool Parser::canStateDominate(ParserState *state) const
{
  for (auto s : this->states) {
    if (s != state && s->getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
      return false;
    } else if (s == state) {
      if (s->getProcessingStatus() == ParserProcessingStatus::COMPLETE) return true;
      else return false;
    }
  }
  // Passed state is incorrect.
  throw EXCEPTION(InvalidArgumentException, STR("state"),
                  STR("The given state was not found among the current list of states."));
}


Bool Parser::canAbandonState(ParserState *state) const
{
  for (auto s : this->states) {
    if (s != state && s->getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
      return true;
    } else if (s == state) {
      return false;
    }
  }
  // Passed state is incorrect.
  throw EXCEPTION(InvalidArgumentException, STR("state"),
                  STR("The given state was not found among the current list of states."));
}


Bool Parser::dominateState(ParserState *state)
{
  if (!this->canStateDominate(state)) return false;
  for (auto s : this->states) {
    if (s != state) s->setProcessingStatus(ParserProcessingStatus::ERROR);
  }
  return true;
}


Bool Parser::abandonState(ParserState *state)
{
  if (!this->canAbandonState(state)) return false;
  state->setProcessingStatus(ParserProcessingStatus::ERROR);
  return true;
}

} } // namespace

/**
 * @file Core/Processing/Parser.cpp
 * Contains the implementation of class Core::Processing::Parser.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing
{

// TODO: Use the cached term parameters from term levels.

//==============================================================================
// Member Functions

void Parser::initialize(SharedPtr<Data::Ast::Scope> rootScope)
{
  // Before we can change the production list, we need to make sure we have no outstanding
  // states.
  if (this->state != 0) {
    throw EXCEPTION(GenericException, S("Grammar manager can't be changed while being in use."));
  }

  // TODO: If we currently have a grammar manager, we need to unset it first.
  //if (this->production_definitions != 0) {
  //    this->production_definitions->production_in_use_inquirer.unconnect(this, &Parser::is_production_in_use);
  //}

  // Set the repositories.
  this->rootScope = rootScope;
  if (this->rootScope == 0) {
    throw EXCEPTION(InvalidArgumentException, S("rootScope"), S("Cannot be null."));
  }

  this->grammarRoot = getSharedPtr(Data::Grammar::getGrammarRoot(rootScope.get()));
  if (this->grammarRoot == 0) {
    throw EXCEPTION(InvalidArgumentException, S("rootScope"), S("Root scope does not contain a grammar."));
  }

  // TODO: If we have a new manager, we need to set the production_in_use_inquirer signal.
  //if (this->production_definitions != 0) {
  //    this->production_definitions->production_in_use_inquirer.connect(this, &Parser::is_production_in_use);
  //}

  // Lookup parsing dimensions.
  this->parsingDimensions.clear();
  for (Int i = 0; i < this->grammarRoot->getCount(); ++i) {
    Data::Grammar::ParsingDimension *dim = ti_cast<Data::Grammar::ParsingDimension>(this->grammarRoot->getElement(i));
    if (dim != 0) this->parsingDimensions.push_back(dim);
  }

  // Initialize the tempState used for path testing.
  this->tempState.initialize(
    RESERVED_PARSER_TERM_LEVEL_COUNT, RESERVED_PARSER_PRODUCTION_LEVEL_COUNT, this->grammarRoot.get()
  );
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
  if (this->grammarRoot == 0) {
    throw EXCEPTION(GenericException, S("Grammar root is not set."));
  }
  if (this->grammarRoot->getStartRef() == 0) {
    throw EXCEPTION(GenericException, S("Grammar root production is not set."));
  }

  // Delete all states, if any.
  this->clear();

  // Create the new state.
  this->state = std::make_shared<ParserState>(
    RESERVED_PARSER_TERM_LEVEL_COUNT, RESERVED_PARSER_PRODUCTION_LEVEL_COUNT, this->grammarRoot.get()
  );
  this->state->setPrevProcessingStatus(ParserProcessingStatus::COMPLETE);
  this->state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
  // Initialize the main level.
  this->state->pushTermLevel(0);
  // Initialize the program root level.
  auto prod = this->state->getGrammarContext()->getReferencedSymbol(this->grammarRoot->getStartRef().get());
  auto module = prod->findOwner<Data::Grammar::Module>();
  if (!prod->isA<Data::Grammar::SymbolDefinition>()) {
    throw EXCEPTION(GenericException, S("Reference term is pointing to a target of a wrong type."));
  }
  if (prod->getTerm() == 0) {
    throw EXCEPTION(GenericException, S("Formula of root production isn't set yet."));
  }
  this->pushStateProdLevel(this->state.get(), module, static_cast<Data::Grammar::SymbolDefinition*>(prod), 0);

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
SharedPtr<TiObject> Parser::endParsing(Data::SourceLocationRecord &endSourceLocation)
{
    // Validation.
  if (this->state == 0) {
    throw EXCEPTION(GenericException, S("Parsing is not initialized yet."));
  }

// Fold out the levels stack of existing states.
  this->tryCompleteFoldout(this->state.get());
  this->reportMislocatedLeadingModifiers(this->state.get());
  ASSERT(this->state->getProcessingStatus() != ParserProcessingStatus::IN_PROGRESS);

  if (this->state->getProcessingStatus() != ParserProcessingStatus::COMPLETE || this->state->getTermLevelCount() != 1) {
    // Even if the state was waiting we'll consider it to fail since a successful state should
    // fold out successfully.
    this->state->setProcessingStatus(ParserProcessingStatus::ERROR);
    // We'll keep looping and raising errors until we fold out of the grammar tree completely.
    Bool unexpectedEofRaised = false;
    do {
      // Raise error notification event, but only if state has been waiting for more tokens.
      if (this->state->getTermLevelCount() > 1) {
        if (!this->getTopParsingHandler(this->state.get())->onErrorToken(this, this->state.get(), 0)) {
          // We don't want to create duplicates of this error message.
          if (!unexpectedEofRaised) {
            auto sourceLocation = std::make_shared<Data::SourceLocationRecord>();
            sourceLocation->filename = endSourceLocation.filename;
            sourceLocation->line = endSourceLocation.line;
            sourceLocation->column = endSourceLocation.column;
            this->state->addNotice(SharedPtr<Notices::Notice>(new Notices::UnexpectedEofNotice(sourceLocation)));
            unexpectedEofRaised = true;
          }
        }
      }
      // First, move the state to an error sync position.
      // Note that here we'll move to the position immediately after the error sync position
      // because we dont want to wait for the sync token.
      while (this->state->getTermLevelCount() > 1) {
        if (this->state->refTopTermLevel().getTerm()->isA<Data::Grammar::ConcatTerm>()) {
          Data::Grammar::ConcatTerm *concatTerm =
              static_cast<Data::Grammar::ConcatTerm*>(this->state->refTopTermLevel().getTerm());
          Int errorSyncPosId = concatTerm->getErrorSyncPosId();
          auto posId = static_cast<Int>(this->state->refTopTermLevel().getPosId());
          if (errorSyncPosId >= posId) {
            // Here there is no point in waiting for tokens, so we'll jump out of the concat list
            // regardless of the sync pos id.
            this->popStateLevel(this->state.get(), false);
            break;
          }
        } else {
          TiInt *flags = this->state->getTermFlags();
          if ((flags == 0 ? 0 : flags->get()) & Data::Grammar::TermFlags::ERROR_SYNC_TERM) {
            // Here there is no point in waiting for tokens, so we'll jump out of the concat list
            // regardless of the sync pos id.
            this->popStateLevel(this->state.get(), false);
            break;
          }
        }
        this->popStateLevel(this->state.get(), false);
      }
      // Now let's try again to fold out of the grammar tree.
      this->tryCompleteFoldout(this->state.get());
    } while (this->state->getTermLevelCount() > 1);
  }

  // Finally, we are left with only one chosen state which has folded out of the grammar tree.

  // Flush build messages.
  this->flushApprovedNotices();

  // Return remaining parsing data after clearing the remaining state.
  SharedPtr<TiObject> data = this->state->getData();
  this->clear();
  return data;
}


/**
 * Try to take the upper route and ignore any optional inner routes until we hit
 * an incomplete level or fold out completely.
 */
void Parser::tryCompleteFoldout(ParserState *state)
{
  if (state->getTermLevelCount() <= 1) return;
  Data::Token token;
  token.setId(this->EOF_TOKEN);
  this->tempState.reset();
  this->tempState.setBranchingInfo(state, -1);
  this->tempState.setParsingDimensionInfo(
    state->getParsingDimensionIndex(), state->getParsingDimensionStartProdIndex()
  );
  this->testState(&token, &this->tempState);
  if (this->tempState.getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
    while (state->getTermLevelCount() > this->tempState.getTermLevelCount()) {
      this->popStateLevel(state, true);
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
    throw EXCEPTION(InvalidArgumentException, S("token"), S("token is null."));
  }

  // The caller should call beginParsing before calling handleNewToken, and should not call handleNewToken
  // after parsing folds out of the production tree.
  if (this->state->getTermLevelCount() == 0)
    throw EXCEPTION(GenericException, S("Parser::beginParsing should be called before calling handleNewToken"));
  else if (this->state->getTermLevelCount() == 1) {
    // Raise an unexpected token error.
    if (!this->unexpectedTokenNoticeRaised) {
      this->state->addNotice(std::make_shared<Notices::UnexpectedTokenNotice>(
        std::make_shared<Data::SourceLocationRecord>(token->getSourceLocation())
      ));
      this->unexpectedTokenNoticeRaised = true;
    }
    return;
  }

  // Reset the processing status of all states.
  this->state->setPrevProcessingStatus(this->state->getProcessingStatus());
  this->state->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);

  // Process the states.
  // Process the token.
  this->processState(token, this->state.get());

  // Flush any approved build messages.
  this->flushApprovedNotices();

  // Did we reach the end of parsing?
  if (this->state->getProcessingStatus() == ParserProcessingStatus::COMPLETE && this->state->getTermLevelCount() == 1) {
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
  Int count = this->state->getNoticeStore()->getCount();
  if (count == 0) return;

  // Now emit the messages.
  for (Int i = 0; i < count; ++i) {
    this->noticeSignal.emit(this->state->getNoticeStore()->get(i));
  }
  this->state->getNoticeStore()->flush(count);
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
void Parser::processState(const Data::Token * token, ParserState *state)
{
  // Have this state folded out to completion already? This can happen if the resulted from a branch at the root
  // production.
  if (state->getTermLevelCount() == 1) {
    state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
    LOG(LogLevel::PARSER_MID, S("Start Processing State: State already folded out to completion."));
    return;
  }

  LOG(LogLevel::PARSER_MID, S("Start Processing State: At production ") <<
      ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId()) <<
      S(", level ") << state->getTopProdTermLevelCount());

  Bool error = false;

  // Process the given token.
  while (state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS) {
    ASSERT(state->getTermLevelCount() > 0);
    // Did we fold out to completion?
    if (state->getTermLevelCount() == 1) {
      state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      break;
    }
    // If the previous state was an error, it means we are waiting to sync. But we'll need to check
    // block pairs so that we only sync when we are at the same block level rather than inside a
    // deeper level.
    if (state->getPrevProcessingStatus() == ParserProcessingStatus::ERROR &&
        this->matchErrorSyncBlockPairs(state, token)) {
      // We are in a deeper block (or just left that deeper block) so we'll assume we are still in
      // error.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
    } else {
      // We are not waiting to sync, or we are waiting to sync but we are already on the same block
      // level, so we'll proceed with matching the token normally.

      Int parseDimIndex = state->getParsingDimensionIndex() == -1 ? this->matchParsingDimensionEntry(token) : -1;
      if (parseDimIndex != -1) {
        this->enterParsingDimension(token, parseDimIndex, state);
        state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      } else {
        Data::Grammar::Term *term = state->refTopTermLevel().getTerm();
        if (term->isA<Data::Grammar::TokenTerm>()) {
          this->processTokenTerm(token, state);
        } else if (term->isA<Data::Grammar::MultiplyTerm>()) {
          this->processMultiplyTerm(token, state);
        } else if (term->isA<Data::Grammar::AlternateTerm>()) {
          this->processAlternateTerm(token, state);
        } else if (term->isA<Data::Grammar::ConcatTerm>()) {
          this->processConcatTerm(token, state);
        } else if (term->isA<Data::Grammar::ReferenceTerm>()) {
          this->processReferenceTerm(token, state);
        } else {
          // Invalid state type.
          StrStream stream;
          stream << S("Invalid term type while processing ")
                 << ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId())
                 << S(". Found Term Type: ")
                 << term->getTypeInfo()->getTypeName();
          throw EXCEPTION(GenericException, stream.str().c_str());
        }
      }
    }
    // Did the state error out?
    if (
      state->getPrevProcessingStatus() != ParserProcessingStatus::ERROR &&
      state->getProcessingStatus() == ParserProcessingStatus::ERROR &&
      error == false
    ) {
      error = true;

      if (!this->getTopParsingHandler(this->state.get())->onErrorToken(this, state, token)) {
        state->addNotice(std::make_shared<Notices::SyntaxErrorNotice>(
          std::make_shared<Data::SourceLocationRecord>(token->getSourceLocation())
        ));
      }
      // Move the state to an error sync position.
      while (state->getTermLevelCount() > 1) {
        LOG(LogLevel::PARSER_MINOR, S("Error Synching: Checking prod (") <<
            ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId()) <<
            S(") at level ") << (state->getTopProdTermLevelCount() - 1));
        if (state->refTopTermLevel().getTerm()->isA<Data::Grammar::ConcatTerm>()) {
          Int errorSyncPosId =
              static_cast<Data::Grammar::ConcatTerm*>
              (state->refTopTermLevel().getTerm())->getErrorSyncPosId();
          auto posId = static_cast<Int>(state->refTopTermLevel().getPosId() & (~THIS_PROCESSING_PASS));
          auto thisProcessingPass = (posId == 0) || (state->refTopTermLevel().getPosId() & THIS_PROCESSING_PASS);
          if (!thisProcessingPass && errorSyncPosId >= posId) {
            Int cnt = state->getListTermChildCount();
            if (errorSyncPosId > cnt) errorSyncPosId = cnt;
            state->setTopTermPosId(errorSyncPosId);
            break;
          }
        } else {
          TiInt *flags = state->getTermFlags();
          if ((flags == 0 ? 0 : flags->get()) & Data::Grammar::TermFlags::ERROR_SYNC_TERM) {
            break;
          }
        }
        this->popStateLevel(state, false);
      }
      LOG(LogLevel::PARSER_MINOR, S("Error Synching: Synched at prod (") <<
          ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId()) <<
          S(") at level ") << (state->getTopProdTermLevelCount() - 1));
      // We need to match the error block pairs in case the error token itself is the opening of
      // a block (a bracket for example).
      this->matchErrorSyncBlockPairs(state, token);

      // Set the status back to IN_PROGRESS for now (we'll be setting it back to ERROR later).
      state->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
    }
  }

  // Reset any THIS_PROCESSING_PASS flag found in term level indexes.
  for (Int i = 0; i < state->getTermLevelCount(); i++) {
    ParserTermLevel &termLevel = state->refTermLevel(i);
    if (termLevel.getPosId() & THIS_PROCESSING_PASS) {
      state->setTermPosId(termLevel.getPosId()&(~THIS_PROCESSING_PASS), i);
    }
  }

  // Pre-close levels that are complete, if enabled.
  // NOTE:
  // In most cases we don't need to pre-close complete levels, but some grammars might require that, so we're keeping it
  // behind a flag that is off by default. Cases that might require pre-closing include situations around state
  // branching where pre-closing can help drop re-merging states earlier. Other cases include grammars that are finite,
  // i.e grammars that aren't endlessly repetitive. All those situations are very rare and we don't need them in the
  // default grammar.
  if (this->preCloseCompleteLevels) {
    if (state->getProcessingStatus() == ParserProcessingStatus::COMPLETE && state->getTermLevelCount() > 1) {
      while (state->getTermLevelCount() > 1) {
        // Determine if this level is complete.
        bool levelComplete = false;
        Data::Grammar::Term *term = state->refTopTermLevel().getTerm();
        if (term->isA<Data::Grammar::ConcatTerm>()) {
          if (static_cast<Int>(state->refTopTermLevel().getPosId()) >=
              state->getListTermChildCount()) {
            levelComplete = true;
          }
        } else if (term->isA<Data::Grammar::MultiplyTerm>()) {
          TiInt *maxOccurances = state->getMultiplyTermMax();
          if (maxOccurances != 0 &&
              static_cast<Int>(state->refTopTermLevel().getPosId()) >= maxOccurances->get()) {
            levelComplete = true;
          }
        } else {
          if (state->refTopTermLevel().getPosId() != 0) levelComplete = true;
        }
        // Close the complete level or quit loop.
        if (levelComplete)
          this->popStateLevel(state, true);
        else
          break;
      }
    }
  }

  this->markLeadingModifiersMaxIndex(state);
  this->reportMislocatedLeadingModifiers(state);
  this->cancelTrailingModifiers(state);
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
void Parser::processTokenTerm(const Data::Token * token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::TokenTerm>());
  // Are we starting with this token, or are we done with it?
  if (state->refTopTermLevel().getPosId() == 0) {
    // Make sure we have some token id.
    Word matchId = state->getTokenTermId()->get();
    TiObject *matchText = state->getTokenTermText();
    if (matchId == UNKNOWN_ID && matchText == 0) {
      throw EXCEPTION(GenericException, S("Token term's match id isn't assigned yet."));
    }
    // Match this token with the token term.
    Bool matched = this->matchToken(matchId, matchText, token);
    if (matched) {
      // Fire parsing handler event.
      this->getTopParsingHandler(state)->onNewToken(this, state, token);
      // Processing of this state is complete.
      state->setTopTermPosId(1|THIS_PROCESSING_PASS);
      state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      LOG(LogLevel::PARSER_MID, S("Process State: Token accepted (") <<
          ID_GENERATOR->getDesc(matchId) << S(":") <<
          token->getText() << S(")"));
    }
    if (!matched) {
      // Processing of this state has errored out.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      #ifdef USE_LOGS
        auto matchStr = ti_cast<TiStr>(matchText);
        auto matchMap = ti_cast<Core::Data::Grammar::Map>(matchText);
      #endif
      LOG(LogLevel::PARSER_MID, S("Process State: Token failed (") <<
          ID_GENERATOR->getDesc(matchId) << S(":") <<
          (matchStr==0?(matchMap==0?"":matchMap->getKey(0).c_str()):matchStr->get()) << S(") -- Received (") <<
          ID_GENERATOR->getDesc(token->getId()) << S(":") <<
          token->getText() << S(")"));
    }
  } else {
    // We are already done with this token, so move back to the upper level after
    // removing the current state level.
    this->popStateLevel(state, true);
  }
}


/**
 * Apply the received token on a duplicate term within the given state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * Call the determineMultiplyRoute to determine the possible routes
 * to take. If no route was found to match the given token, the processing
 * status will be set to error. If one is found, the route will be followed
 * immediately. If two routes are found, the state will be duplicated with each
 * one of them set to follow one of the routes (one to to deeper and one to go
 * up).
 *
 * @param token A pointer to the token to apply to the state.
 * @param si The iterator of the state being processed.
 */
void Parser::processMultiplyTerm(const Data::Token * token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::MultiplyTerm>());
  Data::Grammar::MultiplyTerm *multiplyTerm =
      static_cast<Data::Grammar::MultiplyTerm*>(state->refTopTermLevel().getTerm());
  // Make sure we have a child term.
  Data::Grammar::Term *childTerm = multiplyTerm->getTerm().get();
  if (childTerm == 0) {
    throw EXCEPTION(GenericException, S("Multiply term's child term isn't set yet."));
  }

  Int successRoute = this->determineMultiplyRoute(token, state);
  this->getTopParsingHandler(state)->onMultiplyRouteDecision(this, state, successRoute, token);
  // Take the selected route.
  if (successRoute == 1) {
    // Increment the level index (iteration count).
    state->setTopTermPosId((state->refTopTermLevel().getPosId()+1)|THIS_PROCESSING_PASS);
    // Take the inner route.
    this->pushStateTermLevel(state, childTerm, 0, token);
    LOG(LogLevel::PARSER_MID, S("Process State: Taking multiply route."));
  } else if (successRoute == 0) {
    // Take the up route.
    this->popStateLevel(state, true);
    LOG(LogLevel::PARSER_MID, S("Process State: Rejecting multiply route."));
  } else {
    // Error out.
    state->setProcessingStatus(ParserProcessingStatus::ERROR);
    LOG(LogLevel::PARSER_MID, S("Process State: Failed to find a successful multiply route."));
  }
}


/**
 * Apply the received token on an alternative term within the given state.<br>
 * <br>
 * HOW IT WORKS:<br>
 * When parsing reaches the alternative term for the first time, the
 * determineAlternateRoute function will be called to determine the
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
void Parser::processAlternateTerm(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::AlternateTerm>());
  // Make sure we have some child terms attached.
  if (state->getListTermChildCount() < 1) {
    throw EXCEPTION(GenericException, S("Alternate term doesn't have any branches yet (zero children)."));
  }
  if (state->refTopTermLevel().getPosId() == 0) {
    // We are entering the term for the first time.
    Int successRoute = this->determineAlternateRoute(token, state);

    if (successRoute == -1) {
      // Processing of this state has errored out.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MID, S("Process State: No possible routes at alternate term."));
    } else {
      // Fire parsing handler event.
      this->getTopParsingHandler(state)->onAlternateRouteDecision(this, state, successRoute, token);
      // Take the selected route.
      state->setTopTermPosId((successRoute+1)|THIS_PROCESSING_PASS);
      Data::Grammar::Term *childTerm = state->getListTermChild(successRoute);
      this->pushStateTermLevel(state, childTerm, 0, token);
      LOG(LogLevel::PARSER_MID, S("Process State: Taking only one alternate route (") << successRoute << S(")."));
    }
  } else {
    // We are already done with this term, so move back to the upper level after
    // removing the current state level.
    this->popStateLevel(state, true);
    LOG(LogLevel::PARSER_MID, S("Process State: Finished alternate term."));
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
void Parser::processConcatTerm(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::ConcatTerm>());
  // Make sure we have a child term.
  Word termCount = state->getListTermChildCount();
  /*if (termCount == 0) {
    throw EXCEPTION(GenericException, S("Concat term's child terms aren't set yet."));
  }*/
  Int posId = state->refTopTermLevel().getPosId() & (~THIS_PROCESSING_PASS);
  // Did we finish all the terms in this list?
  if (static_cast<Word>(posId) < termCount) {
    // Fire parsing handler event.
    this->getTopParsingHandler(state)->onConcatStep(this, state, posId, token);
    // Increment the position id of this level.
    state->setTopTermPosId((posId+1)|THIS_PROCESSING_PASS);
    // Move to the term pointed by position id.
    Data::Grammar::Term *childTerm = state->getListTermChild(posId);
    this->pushStateTermLevel(state, childTerm, 0, token);
    LOG(LogLevel::PARSER_MID, S("Process State: Processing concat term (") << (posId+1) << S(")."));
  } else {
    // We are done with this list, so move to the upper level.
    this->popStateLevel(state, true);
    LOG(LogLevel::PARSER_MID, S("Process State: Finished concat term."));
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
void Parser::processReferenceTerm(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::ReferenceTerm>());
  // Are we starting with this term, or are we done with it?
  if (state->refTopTermLevel().getPosId() == 0) {
    // We are starting with this token.
    // Get the referenced module and definition.
    Data::Grammar::Module *module;
    Data::Grammar::SymbolDefinition *definition;
    state->getReferencedSymbol(module, definition);
    if (!definition->isA<Data::Grammar::SymbolDefinition>()) {
      throw EXCEPTION(GenericException, S("Reference term is pointing to a target of a wrong type."));
    }
    if (definition->getTerm() != 0) {
      // Update the current level's position id.
      state->setTopTermPosId(1|THIS_PROCESSING_PASS);
      // Create the new state level.
      this->pushStateProdLevel(state, module, static_cast<Data::Grammar::SymbolDefinition*>(definition), token);
      LOG(LogLevel::PARSER_MID, S("Process State: Processing referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << S(")."));
    } else {
      // An empty production is encountered.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MID, S("Process State: Erroring on empty referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << S(")."));
    }
  } else {
    // We are already done with this token, so move back to the upper level after
    // removing the current state level.
    this->popStateLevel(state, true);
    LOG(LogLevel::PARSER_MID, S("Process State: Done with referenced production. Back to (") <<
        ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId())
        << S(")."));
  }
}


void Parser::enterParsingDimension(Data::Token const *token, Int parseDimIndex, ParserState *state)
{
  auto ref = this->parsingDimensions[parseDimIndex]->getStartRef().get();
  auto prodDef = state->getGrammarContext()->getReferencedSymbol(ref);
  auto module = prodDef->findOwner<Data::Grammar::Module>();
  if (prodDef->getTerm() != 0) {
    state->setParsingDimensionInfo(parseDimIndex, state->getProdLevelCount());
    this->pushStateProdLevel(state, module, static_cast<Data::Grammar::SymbolDefinition*>(prodDef), token);
    LOG(LogLevel::PARSER_MID, S("Process State: Entering parsing dimension (") <<
        ID_GENERATOR->getDesc(prodDef->getId()) << S(")."));
  } else {
    // An empty production is encountered.
    state->setProcessingStatus(ParserProcessingStatus::ERROR);
    LOG(LogLevel::PARSER_MID, S("Process State: Erroring on empty parsing dimension production (") <<
        ID_GENERATOR->getDesc(prodDef->getId()) << S(")."));
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
  this->state.reset();
  this->tempState.reset();
}


/**
 * Compute the list of possible routes to take at a duplicate term. Checking for
 * a possible route is done by creating a temporary state that branches from the
 * given state and trying all possible routes on that temp state. Trying a
 * single route is done by setting the state levels of the temp state to the
 * given route and calling testState to see whether the temp state ends up in
 * COMPLETE or ERROR.
 *
 * @param token A pointer to the token against which to test the routes.
 * @param state A pointer to the state object from which to start testing the
 *              routes.
 */
Int Parser::determineMultiplyRoute(Data::Token const *token, ParserState *state)
{
  Data::Grammar::MultiplyTerm *multiplyTerm =
    static_cast<Data::Grammar::MultiplyTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(multiplyTerm->isA<Data::Grammar::MultiplyTerm>());
  TiInt *minOccurances = state->getMultiplyTermMin();
  TiInt *maxOccurances = state->getMultiplyTermMax();
  TiInt *flags = state->getTermFlags();
  Bool errorSync = ((flags == 0 ? 0 : flags->get()) & Data::Grammar::TermFlags::ERROR_SYNC_TERM);

  if (minOccurances != 0 && static_cast<Int>(state->refTopTermLevel().getPosId()) < minOccurances->get()) {
    // We are done with the minimum iterations, so we have only one way which is to go in.
    if (maxOccurances != 0 || static_cast<Int>(state->refTopTermLevel().getPosId()) < maxOccurances->get()) {
      return 1;
    } else {
      throw EXCEPTION(GenericException, S("Invalid multiply term."));
    }
  } else if (maxOccurances != 0 && static_cast<Int>(state->refTopTermLevel().getPosId()) >= maxOccurances->get()) {
    // We have met the maximum iterations, so we have only one way which is to go out.
    return 0;
  } else if (state->getTermLevelCount() == 2) {
    // We can theoretically go either in or out, but we are at the root, so we'll just consider in.
    return 1;
  } else {
    // We can go either in or out, so we'll test.

    // Check if we have previously cached the decision.
    if (token->isKeyword()) {
      // For keywords we need to check against the text of the token rather than just the category to which the token
      // belongs.
      auto i = multiplyTerm->getInnerTextBasedDecisionCache()->find(token->getText());
      if (i != multiplyTerm->getInnerTextBasedDecisionCache()->end()) {
        return i->second;
      }
    } else {
      // For non-keyword tokens we can just check against the category since the text doesn't influence the parsing
      // decision.
      auto i = multiplyTerm->getInnerIdBasedDecisionCache()->find(token->getId());
      if (i != multiplyTerm->getInnerIdBasedDecisionCache()->end()) {
        return i->second;
      }
    }

    // Initialize the temp state.
    this->tempState.reset();
    this->tempState.setBranchingInfo(state, -1);
    this->tempState.setParsingDimensionInfo(state->getParsingDimensionIndex(), state->getParsingDimensionStartProdIndex());
    // Replace the current state level with a new one.
    this->tempState.ownTopLevel();
    this->tempState.setTopTermPosId(1|THIS_PROCESSING_PASS);
    // Create the deeper level.
    this->tempState.pushTermLevel(multiplyTerm->getTerm().get());
    // Test the temp state.
    this->testState(token, &this->tempState);
    // Store results.
    if (this->tempState.getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
      if (token->isKeyword()) {
        multiplyTerm->getInnerTextBasedDecisionCache()->operator[](token->getText()) = 1;
      } else {
        multiplyTerm->getInnerIdBasedDecisionCache()->operator[](token->getId()) = 1;
      }
      return 1;
    } else {
      if (errorSync) {
        // Test outer route.
        this->tempState.reset();
        this->tempState.setBranchingInfo(state, -1);
        this->tempState.setParsingDimensionInfo(
          state->getParsingDimensionIndex(), state->getParsingDimensionStartProdIndex()
        );
        this->tempState.popLevel();
        // Test the temp state.
        this->testState(token, &this->tempState);
        if (this->tempState.getProcessingStatus() == ParserProcessingStatus::COMPLETE) {
          if (token->isKeyword()) {
            multiplyTerm->getInnerTextBasedDecisionCache()->operator[](token->getText()) = 0;
          } else {
            multiplyTerm->getInnerIdBasedDecisionCache()->operator[](token->getId()) = 0;
          }
          return 0;
        } else {
          if (token->isKeyword()) {
            multiplyTerm->getInnerTextBasedDecisionCache()->operator[](token->getText()) =-1;
          } else {
            multiplyTerm->getInnerIdBasedDecisionCache()->operator[](token->getId()) = -1;
          }
          return -1;
        }
      } else {
        if (token->isKeyword()) {
          multiplyTerm->getInnerTextBasedDecisionCache()->operator[](token->getText()) = 0;
        } else {
          multiplyTerm->getInnerIdBasedDecisionCache()->operator[](token->getId()) = 0;
        }
        return 0;
      }
    }
  }
}


/**
 * Compute the list of possible routes to take at an alternative term. Checking
 * for a possible route is done by creating a temporary state that branches from
 * the given state and trying all possible routes on that temp state. Trying a
 * single route is done by setting the state levels of the temp state to the
 * given route and calling testState to see whether the temp state ends up in
 * COMPLETE or ERROR.
 *
 * @param token A pointer to the token against which to test the routes.
 * @param state A pointer to the state object from which to start testing the
 *              routes.
 */
Int Parser::determineAlternateRoute(Data::Token const *token, ParserState *state)
{
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::AlternateTerm>());
  auto alternateTerm = static_cast<Data::Grammar::AlternateTerm*>(state->refTopTermLevel().getTerm());

  // Check if we have previously cached the decision.
  if (token->isKeyword()) {
    // For keywords we need to check against the text of the token rather than just the category to which the token
    // beongs.
    auto i = alternateTerm->getInnerTextBasedDecisionCache()->find(token->getText());
    if (i != alternateTerm->getInnerTextBasedDecisionCache()->end()) {
      return i->second;
    }
  } else {
    // For non-keyword tokens we can just check against the category since the text doesn't influence the parsing
    // decision.
    auto i = alternateTerm->getInnerIdBasedDecisionCache()->find(token->getId());
    if (i != alternateTerm->getInnerIdBasedDecisionCache()->end()) {
      return i->second;
    }
  }

  Word termCount = state->getListTermChildCount();
  for (Int i = 0; static_cast<Word>(i) < termCount; i++) {
    // Initialize the temp state branching from the current one.
    this->tempState.reset();
    this->tempState.setBranchingInfo(state, -1);
    this->tempState.setParsingDimensionInfo(
      state->getParsingDimensionIndex(),
      state->getParsingDimensionStartProdIndex()
    );
    // Own the top level so we don't modify the trunk state's level.
    this->tempState.ownTopLevel();
    this->tempState.setTopTermPosId((i+1)|THIS_PROCESSING_PASS);
    // Create the deeper level.
    this->tempState.pushTermLevel(state->getListTermChild(i));
    // Test the temp state.
    this->testState(token, &this->tempState);
    // Store results.
    if (this->tempState.getProcessingStatus()==ParserProcessingStatus::COMPLETE) {
      if (token->isKeyword()) {
        alternateTerm->getInnerTextBasedDecisionCache()->operator[](token->getText()) = i;
      } else {
        alternateTerm->getInnerIdBasedDecisionCache()->operator[](token->getId()) = i;
      }
      return i;
    }
  }
  if (token->isKeyword()) {
    alternateTerm->getInnerTextBasedDecisionCache()->operator[](token->getText()) = -1;
  } else {
    alternateTerm->getInnerIdBasedDecisionCache()->operator[](token->getId()) = -1;
  }
  return -1;
}


Int Parser::matchParsingDimensionEntry(Data::Token const *token)
{
  for (Int i = 0; i < this->parsingDimensions.size(); ++i) {
    if (token->getId() == this->parsingDimensions[i]->getEntryTokenId()->get()) return i;
  }
  return -1;
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
void Parser::testState(Data::Token const *token, ParserState *state)
{
  ASSERT(state != 0);

  // Process state levels.
  while (state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS) {
    // Did we fold out to completion?
    if (state->getTermLevelCount() == 1) {
      if (token->getId() == this->EOF_TOKEN) state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      else state->setProcessingStatus(ParserProcessingStatus::ERROR);
      break;
    }
    // Test the current state level.
    this->testStateLevel(token, state);
    // In case of errors, check if we can take another route somewhere up.
    while (state->getProcessingStatus() == ParserProcessingStatus::ERROR && state->getTermLevelCount() > 2) {
      // Remove the current level.
      state->popLevel();
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Poping a failed level. Back to (") <<
        (
          state->getProdLevelCount()==0 ?
          S("start") :
          ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId())
        )
        << S(" - ")
        << (state->getTopProdTermLevelCount() - 1)
        << S(")."));
      // Is this level a multi-route level that has been set during the test?
      if (state->refTopTermLevel().getPosId() & THIS_TESTING_PASS) {
        // This is an intersection set during this testing pass, so we can try a different
        // route here.
        this->testStateLevel(token, state);
      } else {
        // We got out of the testing chain of state levels, so continuing at this stage won't
        // lead us anywhere.
        return;
      }
    }
  }
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

  Int parseDimIndex = state->getParsingDimensionIndex() == -1 ? this->matchParsingDimensionEntry(token) : -1;
  if (parseDimIndex != -1) {
    this->testParsingDimension(token, parseDimIndex, state);
    state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
    return;
  }

  // check the term type
  Data::Grammar::Term *term = state->refTopTermLevel().getTerm();
  if (term->isA<Data::Grammar::TokenTerm>()) {
    this->testTokenTerm(token, state);
  } else if (term->isA<Data::Grammar::MultiplyTerm>()) {
    this->testMultiplyTerm(token, state);
  } else if (term->isA<Data::Grammar::AlternateTerm>()) {
    this->testAlternateTerm(token, state);
  } else if (term->isA<Data::Grammar::ConcatTerm>()) {
    this->testConcatTerm(token, state);
  } else if (term->isA<Data::Grammar::ReferenceTerm>()) {
    this->testReferenceTerm(token, state);
  } else {
    // Invalid state type.
    StrStream stream;
    stream << S("Invalid term type while testing ")
           << ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId())
           << S(". Found Term Type: ")
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
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::TokenTerm>());
  // Are we starting with this token, or are we done with it?
  if (state->refTopTermLevel().getPosId() == 0) {
    // We are checking this token.
    Word matchId = state->getTokenTermId()->get();
    TiObject *matchText = state->getTokenTermText();
    if (matchId == UNKNOWN_ID && matchText == 0) {
      throw EXCEPTION(GenericException, S("Token term's match id isn't assigned yet."));
    }
    Bool matched = this->matchToken(matchId, matchText, token);
    if (matched) {
      // Processing of this state is complete.
      state->ownTopLevel();
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      state->setProcessingStatus(ParserProcessingStatus::COMPLETE);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Matched for token (") <<
          ID_GENERATOR->getDesc(matchId) << S(":") <<
          token->getText() << S(")"));
    }
    if (!matched) {
      // Processing of this state has errored out.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      #ifdef USE_LOGS
        auto matchStr = ti_cast<TiStr>(matchText);
        auto matchMap = ti_cast<Core::Data::Grammar::Map>(matchText);
      #endif
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Failed for token (") <<
          ID_GENERATOR->getDesc(matchId) << S(":") <<
          (matchStr==0?(matchMap==0?"":matchMap->getKey(0).c_str()):matchStr->get()) << S(") -- Received (") <<
          ID_GENERATOR->getDesc(token->getId()) << S(":") <<
          token->getText() << S(")"));
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
  Data::Grammar::MultiplyTerm *multiplyTerm =
      static_cast<Data::Grammar::MultiplyTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(multiplyTerm->isA<Data::Grammar::MultiplyTerm>());
  TiInt *minOccurances = state->getMultiplyTermMin();
  TiInt *maxOccurances = state->getMultiplyTermMax();

  Int count;
  if (state->refTopTermLevel().getPosId() == 0) {
    // We are reaching this for the first time.
    ASSERT(maxOccurances == 0 || maxOccurances->get() >= 0);
    // Try moving deeper first.
    if (maxOccurances != 0 && maxOccurances->get() == 0) {
      state->popLevel();
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Skipping disabled inner multiply route (max == 0)."));
    } else {
      Data::Grammar::Term *childTerm = multiplyTerm->getTerm().get();
      ASSERT(childTerm != 0);
      state->ownTopLevel();
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      state->pushTermLevel(childTerm);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Trying inner multiply route (0 iterations so far)."));
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
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Trying outer multiply route (") <<
          count << S(" iterations completed)."));
    } else {
      // We haven't finished the minimum required iterations, so
      // we'll fail this state.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Failed for multiply term (") <<
          count << S(" iterations completed)."));
    }
  } else if (state->refTopTermLevel().getPosId() & THIS_PROCESSING_PASS) {
    // We have reached this before during this parsing pass, which
    // means the other route has already been tried, so we'll
    // consider this to fail regardless of the status in order to
    // avoid infinite loops.
    state->setProcessingStatus(ParserProcessingStatus::ERROR);
    LOG(LogLevel::PARSER_MINOR, S("Testing State: Failed for multiply term (already visited)."));
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
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Max iterations reached for multiply term (") <<
          count << S(" iterations)."));
    } else {
      Int posId = state->refTopTermLevel().getPosId();
      state->ownTopLevel();
      state->setTopTermPosId((posId+1)|THIS_TESTING_PASS);
      // Now create the deeper level.
      Data::Grammar::Term *childTerm = multiplyTerm->getTerm().get();
      ASSERT(childTerm != 0);
      state->pushTermLevel(childTerm);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Trying inner multiply route (") <<
          count << S(" iterations so far)."));
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
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::AlternateTerm>());
  if (state->refTopTermLevel().getPosId() == 0) {
    // If we don't have any terms we should treat this as a failed route.
    if (state->getListTermChildCount() == 0) {
      // Processing of this state has errored out.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Failing an empty alternate term."));
    } else {
      // We are entering this term for the first time, so try the
      // first route.
      state->ownTopLevel();
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      Data::Grammar::Term *childTerm = state->getListTermChild(0);
      state->pushTermLevel(childTerm);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Trying alternate route (1)."));
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
    if (static_cast<Word>(index) < termCount) {
      // Try the next route.
      // If we have had an empty loop before, mark that somehow in the index.
      state->setTopTermPosId(((emptyLoop?index+termCount:index)+1)|THIS_TESTING_PASS);
      Data::Grammar::Term *childTerm = state->getListTermChild(index);
      state->pushTermLevel(childTerm);
      // Return the status to IN_PROGRESS to give a chance for the other routes.
      state->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Trying alternate route (") << (index+1) << S(")."));
    } else {
      // We are out of routes, so return to the upper level.
      // If we have had an emptyLoop, we should set the status to
      // IN_PROGRESS.
      if (emptyLoop) {
        state->setProcessingStatus(ParserProcessingStatus::IN_PROGRESS);
        state->popLevel();
        LOG(LogLevel::PARSER_MINOR, S("Testing State: Alternate route has an empty loop. Try upper route."));
      } else {
        LOG(LogLevel::PARSER_MINOR, S("Testing State: Failed to match an alternate route."));
      }
    }
  } else {
    // We haven't visited this term during this test, so we don't need to try the other
    // routes since they are already tested before. So just pass control to the upper
    // level.
    ASSERT(state->getProcessingStatus() == ParserProcessingStatus::IN_PROGRESS);
    state->popLevel();
    LOG(LogLevel::PARSER_MINOR, S("Testing State: Exiting alternate term (not tested in this pass)."));
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
  ASSERT(state->refTopTermLevel().getTerm()->isA<Data::Grammar::ConcatTerm>());
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
  }
  // Increment the index of this level.
  index++;
  // Did we finish all the terms in this list?
  if (static_cast<Word>(index) > termCount) {
    // We are done with this list, so move to the upper level.
    state->popLevel();
    LOG(LogLevel::PARSER_MINOR, S("Testing State: Finished all concat terms (") << index << S(")."));
  } else {
    // Move to the term pointed by level index.
    state->ownTopLevel();
    state->setTopTermPosId(index | THIS_TESTING_PASS);
    Data::Grammar::Term *childTerm = state->getListTermChild(index-1);
    state->pushTermLevel(childTerm);
    LOG(LogLevel::PARSER_MINOR, S("Testing State: Trying concat term (") << index << S(")."));
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
    Data::Grammar::Module *module;
    Data::Grammar::SymbolDefinition *definition;
    state->getReferencedSymbol(module, definition);
    if (!definition->isA<Data::Grammar::SymbolDefinition>()) {
      throw EXCEPTION(GenericException, S("Reference term is pointing to a target of a wrong type."));
    }
    if (definition->getTerm() != 0) {
      // Update the current level's position id.
      state->ownTopLevel();
      state->setTopTermPosId(1|THIS_TESTING_PASS);
      // Create the new state level.
      state->pushProdLevel(module, static_cast<Data::Grammar::SymbolDefinition*>(definition));
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Trying referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << S(")."));
    } else {
      // An empty production is encountered.
      state->setProcessingStatus(ParserProcessingStatus::ERROR);
      LOG(LogLevel::PARSER_MINOR, S("Testing State: Erroring on empty referenced production (") <<
          ID_GENERATOR->getDesc(definition->getId()) << S(")."));
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
    LOG(LogLevel::PARSER_MINOR, S("Testing State: Done with referenced production. Back to (") <<
      (
        state->getProdLevelCount()==0 ?
        S("start") :
        ID_GENERATOR->getDesc(state->refTopProdLevel().getProd()->getId())
      )
      << S(")."));
  }
}


void Parser::testParsingDimension(Data::Token const *token, Int parseDimIndex, ParserState *state)
{
  auto ref = this->parsingDimensions[parseDimIndex]->getStartRef().get();
  auto prodDef = state->getGrammarContext()->getReferencedSymbol(ref);
  auto module = prodDef->findOwner<Data::Grammar::Module>();
  if (prodDef->getTerm() != 0) {
    state->setParsingDimensionInfo(parseDimIndex, state->getProdLevelCount());
    state->pushProdLevel(module, static_cast<Data::Grammar::SymbolDefinition*>(prodDef));
    LOG(LogLevel::PARSER_MID, S("Testing State: Entering parsing dimension (") <<
        ID_GENERATOR->getDesc(prodDef->getId()) << S(")."));
  } else {
    // An empty production is encountered.
    state->setProcessingStatus(ParserProcessingStatus::ERROR);
    LOG(LogLevel::PARSER_MID, S("Testing State: Erroring on empty parsing dimension production (") <<
        ID_GENERATOR->getDesc(prodDef->getId()) << S(")."));
  }
}


void Parser::pushStateTermLevel(ParserState *state, Data::Grammar::Term *term, Word posId, Data::Token const *token)
{
  state->pushTermLevel(term);
  state->setTopTermPosId(posId);
  this->getTopParsingHandler(state)->onTermStart(this, state, token);
}


void Parser::pushStateProdLevel(
  ParserState *state, Data::Grammar::Module *module, Data::Grammar::SymbolDefinition *prod,
  Data::Token const *token
) {
  state->pushProdLevel(module, prod);
  this->getTopParsingHandler(state)->onProdStart(this, state, token);
  this->processLeadingModifierEntry(state);
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
      this->processLeadingModifiersExit(state);
      this->processTrailingModifiers(state);
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
Bool Parser::compareStates(ParserState *s1, ParserState *s2)
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
Bool Parser::isDefinitionInUse(Data::Grammar::SymbolDefinition *definition) const
{
  if (definition == 0) {
    throw EXCEPTION(InvalidArgumentException, S("definition"), S("Should not be null."));
  }
  if (this->grammarRoot == 0) return false;
  for (Int i = 0; i < this->state->getProdLevelCount(); i++) {
    if (this->state->refProdLevel(i).getProd() == definition) return true;
  }
  return false;
}


Bool Parser::matchToken(Word matchId, TiObject *matchText, Data::Token const *token)
{
  Bool matched = true;
  TiStr *matchStr = 0;
  if (matchId != 0 && matchId != token->getId()) {
    matched = false;
  }
  if (matched == true && matchText != 0) {
    if (matchText->isA<TiStr>()) {
      matchStr = static_cast<TiStr*>(matchText);
      if (matchStr->getStr() != token->getText()) matched = false;
    } else if (matchText->isA<Data::Grammar::Map>()) {
      if (static_cast<Data::Grammar::Map*>(matchText)->findIndex(token->getText().c_str()) == -1) matched = false;
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
    auto element = state->getErrorSyncBlockPairs()->getElement(i);
    if (!element->isA<Data::Grammar::TokenTerm>()) {
      throw EXCEPTION(GenericException, S("Invalid error-sync-block-pair data. "
                                          "Pair entries must be of type TokenTerm."));
    }
    Data::Grammar::TokenTerm *term = static_cast<Data::Grammar::TokenTerm*>(element);
    TiInt *matchId = term->getTokenId().ti_cast_get<TiInt>();
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
    throw EXCEPTION(GenericException, S("Invalid error-sync-block-pair data. "
                                        "There must be an even number of entries in this list."));
  }
  auto element = state->getErrorSyncBlockPairs()->getElement(closingIndex);
  if (!element->isA<Data::Grammar::TokenTerm>()) {
    throw EXCEPTION(GenericException, S("Invalid error-sync-block-pair data. "
                                        "Pair entries must be of type TokenTerm."));
  }
  Data::Grammar::TokenTerm *term = static_cast<Data::Grammar::TokenTerm*>(element);
  TiInt *matchId = term->getTokenId().ti_cast_get<TiInt>();
  TiObject *matchText = term->getTokenText().get();
  if (this->matchToken(matchId, matchText, token)) {
    state->getErrorSyncBlockStack().pop_back();
  }
  return true;
}


//==============================================================================
// Modifier Functions

void Parser::processLeadingModifierEntry(ParserState *state)
{
  if (state->getParsingDimensionIndex() != -1) return;
  if (state->getLeadingModifierLevelCount() == 0) return;

  auto parsingHandler = this->getTopParsingHandler(state);

  // Find the first new modifier;
  Int first = 0;
  while (
    first < state->getLeadingModifierLevelCount() &&
    state->refLeadingModifierLevel(first).getMaxProdIndex() != -1
  ) ++first;

  // Pass modifiers to the parsing handler.
  Bool modifierAccepted = false;
  while (first < state->getLeadingModifierLevelCount()) {
    ParserModifierLevel &level = state->refLeadingModifierLevel(first);
    if (parsingHandler->onIncomingModifier(this, state, level.getData(), false)) {
      modifierAccepted = true;
      state->removeLeadingModifierLevel(first);
    } else {
      break;
    }
  }

  // Set the min index if no min index has been set yet.
  for (Int i = first; i < state->getLeadingModifierLevelCount(); ++i) {
    ParserModifierLevel &level = state->refLeadingModifierLevel(i);
    if (modifierAccepted || level.getMinProdIndex() == -1) {
      level.setMinProdIndex(state->getProdLevelCount() - 1);
    }
  }
}


void Parser::markLeadingModifiersMaxIndex(ParserState *state)
{
  if (state->getParsingDimensionIndex() != -1) return;

  Int maxIndex = state->getProdLevelCount() - 1;
  for (Int i = 0; i < state->getLeadingModifierLevelCount(); ++i) {
    ParserModifierLevel &level = state->refLeadingModifierLevel(i);
    if (level.getMaxProdIndex() == -1) {
      level.setMaxProdIndex(maxIndex);
    }
  }
}


void Parser::processLeadingModifiersExit(ParserState *state)
{
  if (state->getParsingDimensionIndex() != -1) return;
  if (state->getLeadingModifierLevelCount() == 0) return;

  auto parsingHandler = this->getTopParsingHandler(state);

  // Find the first non-new modifier;
  Int first = state->getLeadingModifierLevelCount() - 1;
  while (
    first >= 0 &&
    state->refLeadingModifierLevel(first).getMaxProdIndex() == -1
  ) --first;

  // Pass the modifiers to the parsing handler.
  while (first >= 0) {
    ParserModifierLevel &level = state->refLeadingModifierLevel(first);
    if (level.getMaxProdIndex() >= state->getProdLevelCount() - 1) {
      ASSERT(level.getMaxProdIndex() >= level.getMinProdIndex());
      if (parsingHandler->onIncomingModifier(this, state, level.getData(), true)) {
        state->removeLeadingModifierLevel(first);
        --first;
      } else if (level.getMinProdIndex() >= state->getProdLevelCount() - 1) {
        state->addNotice(
          std::make_shared<Notices::UnexpectedModifierNotice>(Data::Ast::findSourceLocation(level.getData().get()))
        );
        state->removeLeadingModifierLevel(first);
        --first;
      } else {
        break;
      }
    } else {
      return;
    }
  }

  // Lower the max index for modifiers that are in range and aren't accepted.
  while (first >= 0) {
    ParserModifierLevel &level = state->refLeadingModifierLevel(first);
    if (level.getMaxProdIndex() >= state->getProdLevelCount()) {
      level.setMaxProdIndex(state->getProdLevelCount() - 1);
      --first;
    } else {
      break;
    }
  }
}


void Parser::reportMislocatedLeadingModifiers(ParserState *state)
{
  if (state->getParsingDimensionIndex() != -1) return;

  for (Int i = 0; i < state->getLeadingModifierLevelCount(); ++i) {
    ParserModifierLevel &level = state->refLeadingModifierLevel(i);
    if (level.getMinProdIndex() == -1) {
      state->addNotice(
        std::make_shared<Notices::UnexpectedModifierNotice>(Data::Ast::findSourceLocation(level.getData().get()))
      );
      state->removeLeadingModifierLevel(i);
      --i;
    }
  }
}


void Parser::processTrailingModifiers(ParserState *state)
{
  if (state->getParsingDimensionIndex() != -1) return;
  if (state->getTrailingModifierLevelCount() == 0) return;

  auto parsingHandler = this->getTopParsingHandler(state);

  while (state->getTrailingModifierLevelCount() > 0) {
    ParserModifierLevel &level = state->refTrailingModifierLevel(0);
    if (parsingHandler->onIncomingModifier(this, state, level.getData(), true)) {
      state->popFrontTrailingModifierLevel();
    } else {
      break;
    }
  }
}


void Parser::cancelTrailingModifiers(ParserState *state)
{
  if (state->getParsingDimensionIndex() != -1) return;

  while (state->getTrailingModifierLevelCount() > 0) {
    ParserModifierLevel &level = state->refTrailingModifierLevel(0);
    state->addNotice(
      std::make_shared<Notices::UnexpectedModifierNotice>(Data::Ast::findSourceLocation(level.getData().get()))
    );
    state->popFrontTrailingModifierLevel();
  }
}

} // namespace

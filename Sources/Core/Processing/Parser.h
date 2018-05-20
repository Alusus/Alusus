/**
 * @file Core/Processing/Parser.h
 * Contains the header of class Core::Processing::Parser.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_PARSER_H
#define CORE_PROCESSING_PARSER_H

namespace Core { namespace Processing
{

// TODO: DOC

/**
 * @brief Contains all the functionality of the parser's state machine.
 * @ingroup core_processing
 *
 * This class contains all the member variables and functions of the state
 * machine.
 */
class Parser : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Parser, TiObject, "Core.Parser", "Core", "alusus.net");


  //============================================================================
  // Types

  /// An iterator used to iterate through elements of a ParserState linked list.
  private: typedef std::list<ParserState*>::iterator StateIterator;

  /**
   * @brief A const version of StateIterator.
   * @sa StateIterator
   */
  private: typedef std::list<ParserState*>::const_iterator ConstStateIterator;


  //============================================================================
  // Member Variables

  /**
   * @brief An identifier for an EOF token.
   * This token is used to test whether a state can fold out successfully.
   */
  public: const Int EOF_TOKEN;

  private: SharedPtr<Data::Ast::Scope> rootScope;
  private: SharedPtr<Data::Grammar::GrammarModule> grammarRoot;

  private: std::vector<Data::Grammar::ParsingDimension*> parsingDimensions;

  /**
   * @brief The array of current states.
   *
   * This vector contains the array of the states that are currently active
   * in the system.
   */
  private: std::list<ParserState*> states;

  /**
   * @brief A temp state to use for route computation.
   *
   * This is defined for performance improving purposes to avoid creating a
   * new state every time a route computation is to be done.
   */
  private: ParserState tempState;

  private: DecisionNodePool decisionNodes;

  /**
   * @brief Specifies whether an UnexpectedTokenNotice has already been raised.
   *
   * During a single parsing process, an UnexpectedTokenNotice (raised when a
   * a token is received after parsing folds completely out of the grammar
   * tree) should only be raised once since the user will know that from that
   * token onwards all tokens are unexpected and there is no need to raise a
   * separate msg for each of them.
   */
  private: Bool unexpectedTokenNoticeRaised;

  private: Bool preCloseCompleteLevels;


  //============================================================================
  // Signals & Slots

  /// Emitted when a build msg (error or warning) is generated.
  public: Signal<void, SharedPtr<Notices::Notice> const&> noticeSignal;

  /**
     * @brief Emitted when parsing has folded out of the grammar tree.
     *
     * This can happen during a call to process() if the grammar is designed in
     * such a way to allow that. When this signal is received, the caller should
     * call endParsing and should stop sending more tokens.
     */
  public: Signal<void> parsingCompleted;

  public: Slot<void, Data::Token const*> handleNewTokenSlot = {this, &Parser::handleNewToken};


  //============================================================================
  // Constructor / Destructor

  public: Parser() : EOF_TOKEN(Data::IdGenerator::getSingleton()->getId("EOF_TOKEN")), preCloseCompleteLevels(false)
  {
  }

  public: virtual ~Parser()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Related Functions
  /// @{

  public: void initialize(SharedPtr<Data::Ast::Scope> rootScope);

  public: void release()
  {
    this->clear();
    this->rootScope.reset();
    this->grammarRoot.reset();
    this->parsingDimensions.clear();
  }

  public: SharedPtr<Data::Ast::Scope> const& getRootScope() const
  {
    return this->rootScope;
  }

  public: SharedPtr<Data::Grammar::GrammarModule> const& getGrammarRoot() const
  {
    return this->grammarRoot;
  }

  public: void setCompleteLevelsPreClosing(Bool v)
  {
    this->preCloseCompleteLevels = v;
  }

  public: Bool getCompleteLevelsPreClosing() const
  {
    return this->preCloseCompleteLevels;
  }

  /// @}

  /// @name Parsing Operations
  /// @{

  /// Prepare the state machine for parsing.
  public: void beginParsing();

  /// Finalize the parsing process.
  public: SharedPtr<TiObject> endParsing();

  /// Try to fold out of the grammar tree.
  public: void tryCompleteFoldout(StateIterator si);

  /// Process the given token by updating the states.
  public: void handleNewToken(Data::Token const *token);

  /// Raise build msgs that are approved and remove them from the buffer.
  public: void flushApprovedNotices();

  /// Apply the received token on a specific state.
  private: void processState(Data::Token const *token, StateIterator si);

  /// Apply the received token on a token term.
  private: void processTokenTerm(Data::Token const *token, StateIterator si);

  /// Apply the received token on a duplicate term.
  private: void processMultiplyTerm(Data::Token const *token, StateIterator si);

  /// Apply the received token on an alternative term.
  private: void processAlternateTerm(Data::Token const *token, StateIterator si);

  /// Apply the received token on a concat term.
  private: void processConcatTerm(Data::Token const *token, StateIterator si);

  /// Apply the received token on a reference term.
  private: void processReferenceTerm(Data::Token const *token, StateIterator si);

  private: void enterParsingDimension(Data::Token const *token, Int parseDimIndex, StateIterator si);

  /// Release all states and their data, but not the definitions.
  public: void clear();

  /// @}

  /// @name Parsing Routes Detection Operations
  /// @{

  /// Compute the list of possible routes to take at a duplicate term.
  private: void computePossibleMultiplyRoutes(Data::Token const *token, ParserState *state);
  private: Bool computeInnerMultiplyRoute(
    Data::Token const *token, ParserState *state, Data::Grammar::MultiplyTerm *multiplyTerm
  );
  private: Bool computeOuterMultiplyRoute(
    Data::Token const *token, ParserState *state, Data::Grammar::MultiplyTerm *multiplyTerm, TiInt *priority
  );


  /// Compute the list of possible routes to take at an alternative term.
  private: void computePossibleAlternativeRoutes(Data::Token const *token, ParserState *state);

  private: Int matchParsingDimensionEntry(Data::Token const *token);

  /// Test the route taken by the given state.
  private: Int testState(Data::Token const *token, ParserState *state);

  /// Test the given token against a single level within the test state.
  private: void testStateLevel(Data::Token const *token, ParserState *state);

  /// Test the given token against a token term within the test state.
  private: void testTokenTerm(Data::Token const *token, ParserState *state);

  /// Test against a duplicate term within the test state.
  private: void testMultiplyTerm(Data::Token const *token, ParserState *state);

  /// Test against an alternative term within the test state.
  private: void testAlternateTerm(Data::Token const *token, ParserState *state);

  /// Test against a concat term within the test state.
  private: void testConcatTerm(Data::Token const *token, ParserState *state);

  /// Test against a reference term within the test state.
  private: void testReferenceTerm(Data::Token const *token, ParserState *state);

  private: void testParsingDimension(Data::Token const *token, Int parseDimIndex, ParserState *state);

  /// @}

  /// @name Utility Functions
  /// @{

  /// Create a new state object.
  private: StateIterator createState();

  /// Duplicate the given state.
  private: StateIterator duplicateState(
    StateIterator si, Data::Token const *token, Int tokensToLive, Int levelCount=-1
  );

  /// Delete a state from the states stack.
  private: void deleteState(StateIterator si, ParserStateTerminationCause stc);

  private: void pushStateTermLevel(ParserState *state, Data::Grammar::Term *term, Word posId, Data::Token const *token);

  private: void pushStateProdLevel(ParserState *state, Data::Grammar::GrammarModule *module,
                                   Data::Grammar::SymbolDefinition *prod, Data::Token const *token);

  /// Pop the top level from a specific state.
  private: void popStateLevel(ParserState *state, Bool success);

  /// Compare two states to see if they are at the same spot in the grammar.
  private: Bool compareStates(ParserState *s1, ParserState *s2);

  /// Get the parsing handler for the top production level in a state.
  private: ParsingHandler* getTopParsingHandler(ParserState *state)
  {
    return state->refTopProdLevel().getProd()->getBuildHandler().s_cast_get<ParsingHandler>();
  }

  /// Check whether the production with the given id is currently in use.
  public: Bool isDefinitionInUse(Data::Grammar::SymbolDefinition *definition) const;

  private: Bool matchToken(TiInt *matchId, TiObject *matchText, Data::Token const *token)
  {
    return this->matchToken(matchId==0?0:matchId->get(), matchText, token);
  }

  private: Bool matchToken(Word matchId, TiObject *matchText, Data::Token const *token);

  private: Bool matchErrorSyncBlockPairs(ParserState *state, Data::Token const *token);

  /// @}

  /// @name Modifier Functions
  /// @{

  private: void processLeadingModifierEntry(ParserState *state);

  private: void markLeadingModifiersMaxIndex(ParserState *state);

  private: void processLeadingModifiersExit(ParserState *state);

  private: void reportMislocatedLeadingModifiers(ParserState *state);

  private: void processTrailingModifiers(ParserState *state);

  private: void cancelTrailingModifiers(ParserState *state);

  /// @}

  /// @name Branch Management Functions
  /// @{

  public: Int getStateCount() const
  {
    return this->states.size();
  }

  /// Check whether there is only one state.
  public: Bool hasSoloState() const
  {
    return !this->states.empty() && (this->states.front() == this->states.back());
  }

  /**
   * @brief Check whether a given state is in a position to dominate all states.
   * When a state dominates, it forces other states to be dropped. For a state
   * to dominate it needs to be a successful state and it should be the highest
   * priority among successful states.
   */
  public: Bool canStateDominate(ParserState *state) const;

  /**
   * @brief Check whether the given state can be abandoned.
   * A state can be abandoned if there is a higher priority state that is
   * successful.
   */
  public: Bool canAbandonState(ParserState *state) const;

  /**
   * @brief If possible, drop all states except the given one.
   * @return true if the operation is successful, false if the state is not
   *         qualified to dominate.
   * @sa canStateDominate()
   */
  public: Bool dominateState(ParserState *state);

  /**
   * @brief Abandon the given state if possible.
   * A state can be abandoned if there is a higher priority state that is
   * successful.
   * @return true if successful, false if there is no successful higher
   *         priority state.
   */
  public: Bool abandonState(ParserState *state);

  /// @}

}; // class

} } // namespace

#endif

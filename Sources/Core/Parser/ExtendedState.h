/**
 * @file Core/Parser/ExtendedState.h
 * Contains the header of class Parser::ExtendedState.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_EXTENDED_STATE_H
#define PARSER_EXTENDED_STATE_H

namespace Core { namespace Parser
{

// TODO: DOC

class ExtendedState : public State
{
  //============================================================================
  // Member Variables

  /**
   * @brief The current processing status of this state object.
   * This variable holds the processing status, which will indicate whether
   * the state is still in process (waiting for the token to be applied to
   * it), the state has finished processing of this token, or the state has
   * hit a syntax error.
   */
  private: ProcessingStatus processingStatus;

  /**
   * @brief The processing status from the previous processing cycle.
   * This is the status from processing the previous token. This is needed to
   * prevent multiple error messages from being raised while waiting for a
   * sync token.
   */
  private: ProcessingStatus prevProcessingStatus;

  /**
   * @brief The time, in number of tokens, for this state to live.
   * This variable is used when the trunkState is not null to identify how
   * long this state should live if trunkState didn't die in the mean time.
   * When a state branches into two states, the parser will wait for a number
   * of tokens in hope that one of the two states will die by itself. If not,
   * it'll force the one with the lower priority to die. The state with the
   * lower priority is always the one with its trunkState variable set to
   * the one with the higher priority, while the one with the higher priority
   * will have its trunkState variable set to null.
   */
  private: Int tokensToLive;

  private: Int testUppermostLevel;


  //============================================================================
  // Constructors / Destructor

  public: ExtendedState();

  public: ExtendedState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                        Word reservedVarCount, Word reservedVarLevelCount, Data::GrammarModule *root);

  public: ExtendedState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                        Word reservedVarCount, Word reservedVarLevelCount, const Data::ParsingGrammarContext *context);

  public: ~ExtendedState()
  {
  }


  //============================================================================
  // Member Variables

  /// @name Main Functions
  /// @{

  public: void initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                          Word reservedVarCount, Word reservedVarLevelCount,
                          Data::GrammarModule *root)
  {
    State::initialize(reservedTermLevelCount, reservedProdLevelCount, maxVarNameLength,
                      reservedVarCount, reservedVarLevelCount, root);
  }

  public: void initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                          Word reservedVarCount, Word reservedVarLevelCount,
                          const Data::ParsingGrammarContext *context)
  {
    State::initialize(reservedTermLevelCount, reservedProdLevelCount, maxVarNameLength,
                      reservedVarCount, reservedVarLevelCount, context);
  }

  /// Reset the object to an empty state.
  public: void reset();

  /**
   * @brief Set the current processing status of this state object.
   *
   * Set the processing status, which will indicate whether the state is
   * still in process (waiting for the token to be applied to it), the state
   * has finished processing of this token, or the state has hit a syntax
   * error.
   */
  public: void setProcessingStatus(ProcessingStatus ps)
  {
    this->processingStatus = ps;
  }

  /**
   * @brief Get the current processing status of this state object.
   *
   * Returns the processing status, which will indicate whether the state is
   * still in process (waiting for the token to be applied to it), the state
   * has finished processing of this token, or the state has hit a syntax
   * error.
   */
  public: ProcessingStatus getProcessingStatus() const
  {
    return this->processingStatus;
  }

  /**
   * @brief Set the processing status from the previous processing cycle.
   *
   * This is the status from processing the previous token. This is needed to
   * prevent multiple error messages from being raised while waiting for a
   * sync token.
   */
  public: void setPrevProcessingStatus(ProcessingStatus ps)
  {
    this->prevProcessingStatus = ps;
  }

  /**
   * @brief Get the processing status from the previous processing cycle.
   *
   * This is the status from processing the previous token. This is needed to
   * prevent multiple error messages from being raised while waiting for a
   * sync token.
   */
  public: ProcessingStatus getPrevProcessingStatus() const
  {
    return this->prevProcessingStatus;
  }

  public: void setTestUppermostLevel(Int l)
  {
    this->testUppermostLevel = l;
  }

  public: Int getTestUppermostLevel() const
  {
    return this->testUppermostLevel;
  }

  /// @}

  /// @name Term Stack Functions
  /// @{

  /// Push a new level into the top of the level stack.
  public: void pushTermLevel(Data::Term *term)
  {
    State::pushTermLevel(term);
  }

  /// Pop a level from the top of the level stack.
  public: void popTermLevel()
  {
    State::popTermLevel();
  }

  public: void setTopTermPosId(Word id)
  {
    State::setTopTermPosId(id);
  }

  public: void setTermPosId(Word id, Int levelIndex)
  {
    State::setTermPosId(id, levelIndex);
  }

  /// @}

  /// @name Production Stack Functions
  /// @{

  public: void pushProdLevel(Data::Module *module, Data::SymbolDefinition *prod)
  {
    State::pushProdLevel(module, prod);
  }

  public: void popProdLevel()
  {
    State::popProdLevel();
  }

  /// @}

  /// @name Variable Stack Functions
  /// @{

  public: Data::VariableStack* getVariableStack()
  {
    return State::getVariableStack();
  }

  /// @}

  /// @name Term Helper Functions
  /// @{

  public: Data::Term* useListTermChild(Int index, Int levelOffset = -1)
  {
    return State::useListTermChild(index, levelOffset);
  }

  /// @}

  /// @name Other Functions
  /// @{

  public: Data::ParsingGrammarContext* getGrammarContext()
  {
    return State::getGrammarContext();
  }

  public: Data::GrammarHelper* getGrammarHelper()
  {
    return State::getGrammarHelper();
  }

  public: void popLevel()
  {
    State::popLevel();
  }

  /// @}

  /// @name Branching Functions
  /// @{

  /// Set this state as a branch of another state.
  public: void setBranchingInfo(ExtendedState *ts, Int ttl, Int tsi, Int psi)
  {
    ASSERT(ts != 0 || (ttl == -1 && tsi == -1 && psi == -1));
    ASSERT(ttl >= -1 && tsi >= -1 && psi >= -1);
    State::setBranchingInfo(ts, tsi, psi);
    this->tokensToLive = ttl;
  }

  public: void setBranchingInfo(ExtendedState *ts, Int ttl)
  {
    this->setBranchingInfo(ts, ttl, ts->getTermLevelCount()-1, ts->getProdLevelCount()-1);
  }

  /**
   * @brief Get the pointer to the trunk state.
   *
   * @return The pointer to the trunk state from which this state branch, or 0
   *         if this state didn't branch from anything.
   */
  public: ExtendedState* getTrunkState() const
  {
    return static_cast<ExtendedState*>(State::getTrunkState());
  }

  /**
   * @brief Get the value of tokensToLive.
   *
   * This variable is used when the trunkState is not null to identify how
   * long this state should live if trunkState didn't die in the mean time.
   * When a state branches into two states, the parser will wait for a number
   * of tokens in hope that one of the two states will die by itself. If not,
   * it'll force the one with the lower priority to die. The state with the
   * lower priority is always the one with its trunkState variable set to
   * the one with the higher priority, while the one with the higher priority
   * will have its trunkState variable set to null.
   */
  public: Int getTokensToLive() const
  {
    return this->tokensToLive;
  }

  /// Decrement the tokens to live counter.
  public: bool decrementTokensToLive();

  public: void ownTopTermLevel()
  {
    State::ownTopTermLevel();
  }

  public: void ownTopProdLevel()
  {
    State::ownTopProdLevel();
  }

  public: void ownTopLevel()
  {
    State::ownTopLevel();
  }

  public: void copyProdLevel(ExtendedState *src, Int offset)
  {
    State::copyProdLevel(src, offset);
  }

  public: void copyTermLevel(ExtendedState *src, Int offset)
  {
    State::copyTermLevel(src, offset);
  }

  /// @}

  /// @name Build Messages Management
  /// @{

  /// Remove a number of build messages from the beginning of the array.
  public: void flushBuildMsgs(Int count)
  {
    State::flushBuildMsgs(count);
  }

  /// Set the count of trunk build messages shared by this state.
  public: void setTrunkSharedBuildMsgCount(Int count)
  {
    State::setTrunkSharedBuildMsgCount(count);
  }

  /// Copy the shared build messages from the trunk into this state's list.
  public: void copyTrunkSharedBuildMsgs()
  {
    State::copyTrunkSharedBuildMsgs();
  }

  /// @}

}; // class

} } // namespace

#endif

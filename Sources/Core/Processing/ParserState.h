/**
 * @file Core/Processing/ParserState.h
 * Contains the header of class Core::Processing::ParserState.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_PARSERSTATE_H
#define CORE_PROCESSING_PARSERSTATE_H

namespace Core { namespace Processing
{

// TODO: DOC

/**
 * @brief Contains information describing current parsing status.
 * @ingroup core_processing
 *
 * The information contained specify where within the formula's terms
 * hierarchy the state is at. The object contains a stack of index entries,
 * each of which specifies the state at a certain level within the hierarchy.
 * The stack also holds data buffers associated with each level within the
 * hierarchy.
 */
class ParserState
{
  friend class Parser;
  friend class DecisionNodePool;

  //============================================================================
  // Member Variables

  /**
   * @brief The stack of indexes that defines the current state.
   * Each entry in this stack defines the position within one level of the
   * terms hierarchy. The first entry is used to specify the token definition
   * used by this index (the index within the list of token definitions
   * defined in the lexer).
   */
  private: std::vector<ParserTermLevel> termStack;

  private: std::vector<ParserProdLevel> prodStack;

  private: std::vector<ParserModifierLevel> leadingModifierStack;
  private: std::vector<ParserModifierLevel> trailingModifierStack;

  private: Data::VariableStack variableStack;

  private: Data::DataStack dataStack;

  private: Data::Grammar::Context grammarContext;

  private: Notices::Store noticeStore;

  /**
   * @brief A cached pointer to the top state level.
   * This pointer is used for optimization purposes to speed up the call to
   * refTopTermLevel() since that call is at the heart of the state machine
   * operation.
   */
  private: ParserTermLevel *topTermLevelCache;

  private: ParserProdLevel *topProdLevelCache;

  /**
   * @brief The state from which this state branched.
   * If this state branchd from another state, this variable will hold the
   * pointer to the original state, otherwise, it'll hold null.
   */
  private: ParserState *trunkState;

  /**
   * @brief The index of the level within the trunk state.
   * If this state branched from another state, this variable can hold the
   * index of the level within the original state from which the branching
   * started. The trunk level referred to by this variable is included in
   * the list of levels shared by the two states.
   * @note This variable is a temporary variable used for the purpose of
   *       speeding up the process of creating branches when we are not yet
   *       sure whether the branch will be retained or not. If the branch is
   *       to be retained, we'll need to copy the trunk's data into this
   *       state. This is necessary because we're not sure if the original
   *       state will live longer than this state. This variable is used
   *       during the testing pass prior to branching.
   */
  private: Int tempTrunkTermStackIndex;

  private: Int tempTrunkProdStackIndex;

  private: Int parsingDimensionIndex;

  private: Int parsingDimensionStartProdIndex;

  /**
   * @brief The current processing status of this state object.
   * This variable holds the processing status, which will indicate whether
   * the state is still in process (waiting for the token to be applied to
   * it), the state has finished processing of this token, or the state has
   * hit a syntax error.
   */
  private: ParserProcessingStatus processingStatus;

  /**
   * @brief The processing status from the previous processing cycle.
   * This is the status from processing the previous token. This is needed to
   * prevent multiple error messages from being raised while waiting for a
   * sync token.
   */
  private: ParserProcessingStatus prevProcessingStatus;

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

  private: Int decisionNodeIndex;

  private: Data::Grammar::List *errorSyncBlockPairs;

  private: std::vector<Int> errorSyncBlockStack;


  //============================================================================
  // Constructors / Destructor

  protected: ParserState();

  protected: ParserState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                         Word reservedVarCount, Word reservedVarLevelCount, Data::Grammar::Module *rootModule);

  protected: ParserState(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                         Word reservedVarCount, Word reservedVarLevelCount, const Data::Grammar::Context *context);

  public: ~ParserState()
  {
  }


  //============================================================================
  // Member Variables

  // TODO: Prevent non-const methods from referencing trunk state.

  /// @name Main Functions
  /// @{

  protected: void initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                             Word reservedVarCount, Word reservedVarLevelCount,
                             Data::Grammar::Module *rootModule);

  protected: void initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                             Word reservedVarCount, Word reservedVarLevelCount,
                             const Data::Grammar::Context *context);

  /// Reset the object to an empty state.
  protected: void reset();

  /**
   * @brief Set the current processing status of this state object.
   *
   * Set the processing status, which will indicate whether the state is
   * still in process (waiting for the token to be applied to it), the state
   * has finished processing of this token, or the state has hit a syntax
   * error.
   */
  protected: void setProcessingStatus(ParserProcessingStatus ps)
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
  public: ParserProcessingStatus getProcessingStatus() const
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
  protected: void setPrevProcessingStatus(ParserProcessingStatus ps)
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
  public: ParserProcessingStatus getPrevProcessingStatus() const
  {
    return this->prevProcessingStatus;
  }

  protected: void setDecisionNodeIndex(Int i)
  {
    this->decisionNodeIndex = i;
  }

  protected: Int getDecisionNodeIndex() const
  {
    return this->decisionNodeIndex;
  }

  /// @}

  /// @name Term Stack Functions
  /// @{

  /**
   * @brief Get the size of the hierarchy level stack.
   *
   * Get the size of the hierarchy level stack after checking whether this
   * state is a branch from another state. If the state is a branch, the
   * function will put into consideration the size of the trunk state's level
   * stack.
   */
  public: Int getTermLevelCount() const
  {
    return this->tempTrunkTermStackIndex + 1 + this->termStack.size();
  }

  /// Reference an entry in the hierarchy level stack.
  public: ParserTermLevel& refTermLevel(Int i);

  /// A const wrappar to refTermLevel().
  public: const ParserTermLevel& refTermLevel(Int i) const
  {
    return const_cast<ParserState*>(this)->refTermLevel(i);
  }

  /**
   * @brief Reference the top entry in the hierarchy level stack.
   * This method is faster than calling refTermLevel(-1) because it caches
   * the pointer to the top level.
   *
   * @return Returns a reference to the top entry. The caller should avoid
   *         keeping a pointer to this entry because the entry can be moved in
   *         memory as a result of a push or pop operation.
   */
  public: ParserTermLevel& refTopTermLevel()
  {
    if (this->topTermLevelCache == 0) {
      throw EXCEPTION(GenericException, S("This state has an empty level stack."));
    }
    return *(this->topTermLevelCache);
  }

  /// A const wrappar to refTopTermLevel().
  public: const ParserTermLevel& refTopTermLevel() const
  {
    return const_cast<ParserState*>(this)->refTopTermLevel();
  }

  /// Get the number of levels in the top production of this state.
  public: Int getTopProdTermLevelCount() const;

  /// Get the state level of the top production in this state.
  public: ParserTermLevel& refTopProdRootTermLevel()
  {
    // The first level does not belong to any production, so we need at least 2 levels.
    if (this->getTermLevelCount() <= 1) {
      throw EXCEPTION(GenericException, S("This state has an empty level stack."));
    }
    return this->refTermLevel(-this->getTopProdTermLevelCount());
  }

  /// A const wrapper to refTopProdRootTermLevel().
  public: const ParserTermLevel& refTopProdRootTermLevel() const
  {
    return const_cast<ParserState*>(this)->refTopProdRootTermLevel();
  }

  /// Push a new level into the top of the level stack.
  protected: void pushTermLevel(Data::Grammar::Term *term);

  /// Pop a level from the top of the level stack.
  protected: void popTermLevel();

  protected: void setTopTermPosId(Word id)
  {
    this->refTopTermLevel().setPosId(id);
  }

  protected: void setTermPosId(Word id, Int levelIndex)
  {
    this->refTermLevel(levelIndex).setPosId(id);
  }

  public: Word getReservedTermLevelCount() const
  {
    return this->termStack.capacity();
  }

  public: Bool isAtProdRoot() const
  {
    return (this->topProdLevelCache != 0) &&
        (this->topProdLevelCache->getTermStackIndex() == this->getTermLevelCount() - 1);
  }

  public: Bool isAProdRoot(Int termIndex) const
  {
    if (termIndex < 0) termIndex += this->getTermLevelCount();
    Int prodIndex = this->findProdLevel(termIndex);
    return prodIndex==-1 ? false : this->refProdLevel(prodIndex).getTermStackIndex() == termIndex;
  }

  public: Int findProdLevel(Int termIndex) const;

  private: Int _findProdLevel(Int termIndex, Int start, Int end) const;

  /// @}

  /// @name Production Stack Functions
  /// @{

  public: Int getProdLevelCount() const
  {
    return this->tempTrunkProdStackIndex + 1 + this->prodStack.size();
  }

  public: ParserProdLevel& refProdLevel(Int i);

  public: const ParserProdLevel& refProdLevel(Int i) const
  {
    return const_cast<ParserState*>(this)->refProdLevel(i);
  }

  public: ParserProdLevel& refTopProdLevel()
  {
    if (this->topProdLevelCache == 0) {
      throw EXCEPTION(GenericException, S("This state has an empty stack."));
    }
    return *(this->topProdLevelCache);
  }

  public: const ParserProdLevel& refTopProdLevel() const
  {
    return const_cast<ParserState*>(this)->refTopProdLevel();
  }

  protected: void pushProdLevel(Data::Grammar::Module *module, Data::Grammar::SymbolDefinition *prod);

  protected: void popProdLevel();

  public: Word getReservedProdLevelCount() const
  {
    return this->prodStack.capacity();
  }

  public: TiInt* getProdFlags(Int levelOffset = -1) const;

  /// @}

  /// @name Variable Stack Functions
  /// @{

  protected: Data::VariableStack* getVariableStack()
  {
    return &this->variableStack;
  }

  public: const Data::VariableStack* getVariableStack() const
  {
    return &this->variableStack;
  }

  public: Word getMaxVarNameLength() const
  {
    return this->variableStack.getMaxStrSize();
  }

  public: Word getReservedVariableCount() const
  {
    return this->variableStack.getReservedRecordCount();
  }

  public: Word getReservedVariableLevelCount() const
  {
    return this->variableStack.getReservedLevelCount();
  }

  /// @}

  /// @name Data Stack Functions
  /// @{

  /**
   * @brief Get the stack of parsing data.
   * This is arbitrary data cretaed and used by parsing handlers.
   */
  public: Data::DataStack* getDataStack()
  {
    return &this->dataStack;
  }

  /**
   * @brief Set the parsing data associated with a term level.
   * This is an arbitrary data created and used by the parsing handler.
   */
  public: void setData(SharedPtr<TiObject> const &data, Int levelOffset = -1)
  {
    this->dataStack.set(data, levelOffset);
  }

  /**
   * @brief Get the parsing data associated with a term level.
   * This is an arbitrary data created and used by the parsing handler.
   */
  public: SharedPtr<TiObject> const& getData(Int levelOffset = -1) const
  {
    return this->dataStack.get(levelOffset);
  }

  /// Checks whether another SharedPtr is sharing the data at a given level.
  public: Bool isDataShared(Int levelOffset = -1) const
  {
    return this->dataStack.isShared(levelOffset);
  }

  /// @}

  /// @name Modifier Stack Functions
  /// @{

  protected: std::vector<ParserModifierLevel>* getLeadingModifierStack()
  {
    return &this->leadingModifierStack;
  }
  protected: std::vector<ParserModifierLevel>* getTrailingModifierStack()
  {
    return &this->trailingModifierStack;
  }

  public: void pushLeadingModifierLevel(TioSharedPtr const &data)
  {
    this->leadingModifierStack.push_back(ParserModifierLevel(data));
  }
  public: void pushTrailingModifierLevel(TioSharedPtr const &data)
  {
    this->trailingModifierStack.push_back(ParserModifierLevel(data));
  }

  public: void pushLeadingModifierLevel(ParserModifierLevel const &level)
  {
    this->leadingModifierStack.push_back(level);
  }
  public: void pushTrailingModifierLevel(ParserModifierLevel const &level)
  {
    this->trailingModifierStack.push_back(level);
  }

  protected: void popFrontLeadingModifierLevel();
  protected: void popFrontTrailingModifierLevel();

  protected: void popBackLeadingModifierLevel();
  protected: void popBackTrailingModifierLevel();

  protected: void removeLeadingModifierLevel(Int index);
  protected: void removeTrailingModifierLevel(Int index);

  public: Word getLeadingModifierLevelCount() const
  {
    return this->leadingModifierStack.size();
  }
  public: Word getTrailingModifierLevelCount() const
  {
    return this->trailingModifierStack.size();
  }

  public: ParserModifierLevel& refLeadingModifierLevel(Int index);
  public: ParserModifierLevel& refTrailingModifierLevel(Int index);

  /// @}

  /// @name Term Helper Functions
  /// @{

    public: Word getListTermChildCount(Int levelOffset = -1) const;

  protected: Data::Grammar::Term* useListTermChild(Int index, Int levelOffset = -1);

  public: TiInt* getTokenTermId(Int levelOffset = -1) const;

  public: TiObject* getTokenTermText(Int levelOffset = -1) const;

  public: void getReferencedSymbol(
    Data::Grammar::Module *&module, Data::Grammar::SymbolDefinition *&definition, Int levelOffset = -1
  );

  public: TiInt* getMultiplyTermMax(Int levelOffset = -1) const;

  public: TiInt* getMultiplyTermMin(Int levelOffset = -1) const;

  public: TiInt* getMultiplyTermPriority(Int levelOffset = -1) const;

  public: TiInt* getTermFlags(Int levelOffset = -1) const;

  /// @}

  /// @name Other Functions
  /// @{

  protected: Data::Grammar::Context* getGrammarContext()
  {
    return &this->grammarContext;
  }

  public: const Data::Grammar::Context* getGrammarContext() const
  {
    return &this->grammarContext;
  }

  protected: void popLevel()
  {
    if (this->isAtProdRoot()) this->popProdLevel();
    else this->popTermLevel();
  }

  public: Data::Grammar::List* getErrorSyncBlockPairs() const
  {
    return this->errorSyncBlockPairs;
  }

  public: std::vector<Int>& getErrorSyncBlockStack()
  {
    return this->errorSyncBlockStack;
  }

  public: Bool isInsideDynamicList() const
  {
    return this->topProdLevelCache->getTermStackDynamicListIndex() != -1;
  }

  /// @}

  /// @name Branching Functions
  /// @{

  /// Set this state as a branch of another state.
  protected: void setBranchingInfo(ParserState *ts, Int ttl, Int tsi, Int psi);

  protected: void setBranchingInfo(ParserState *ts, Int ttl)
  {
    this->setBranchingInfo(ts, ttl, ts->getTermLevelCount()-1, ts->getProdLevelCount()-1);
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

  /**
   * @brief Get the pointer to the trunk state.
   *
   * @return The pointer to the trunk state from which this state branch, or 0
   *         if this state didn't branch from anything.
   */
  public: ParserState* getTrunkState() const
  {
    return this->trunkState;
  }

  public: Int getTempTrunkTermStackIndex()
  {
    return this->tempTrunkTermStackIndex;
  }

  protected: void ownTopTermLevel();

  protected: void ownTopProdLevel();

  protected: void ownTopLevel();

  protected: void copyProdLevel(ParserState *src, Int offset);

  protected: void copyTermLevel(ParserState *src, Int offset);

  /// @}

  /// @name Parsing Dimension Functions
  /// @{

  protected: void setParsingDimensionInfo(Int dimensionIndex, Int startProdIndex)
  {
    this->parsingDimensionIndex = dimensionIndex;
    this->parsingDimensionStartProdIndex = startProdIndex;
  }

  public: Int getParsingDimensionIndex() const
  {
    return this->parsingDimensionIndex;
  }

  public: Int getParsingDimensionStartProdIndex() const
  {
    return this->parsingDimensionStartProdIndex;
  }

  /// @}

  /// @name Build Messages Management
  /// @{

  /// Add a build message to the end of the list.
  public: void addNotice(SharedPtr<Notices::Notice> const &msg)
  {
    this->noticeStore.add(msg);
  }

  public: Notices::Store* getNoticeStore()
  {
    return &this->noticeStore;
  }

  /// @}

}; // class

} } // namespace

#endif

/**
 * @file Core/Parser/State.h
 * Contains the header of class Parser::State.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PARSER_STATE_H
#define PARSER_STATE_H

namespace Core { namespace Parser
{

// TODO: DOC

/**
 * @brief Contains information describing current parsing status.
 * @ingroup general
 *
 * The information contained specify where within the formula's terms
 * hierarchy the state is at. The object contains a stack of index entries,
 * each of which specifies the state at a certain level within the hierarchy.
 * The stack also holds data buffers associated with each level within the
 * hierarchy.
 */
class State
{
  //============================================================================
  // Member Variables

  /**
   * @brief The stack of indexes that defines the current state.
   * Each entry in this stack defines the position within one level of the
   * terms hierarchy. The first entry is used to specify the token definition
   * used by this index (the index within the list of token definitions
   * defined in the lexer).
   */
  private: std::vector<TermLevel> termStack;

  private: std::vector<ProdLevel> prodStack;

  private: Data::VariableStack variableStack;

  private: Data::ParsingGrammarContext grammarContext;

  private: Data::GrammarHelper grammarHelper;

  /**
   * @brief A cached pointer to the top state level.
   * This pointer is used for optimization purposes to speed up the call to
   * refTopTermLevel() since that call is at the heart of the state machine
   * operation.
   */
  private: TermLevel *topTermLevelCache;

  private: ProdLevel *topProdLevelCache;

  /**
   * @brief The state from which this state branched.
   * If this state branchd from another state, this variable will hold the
   * pointer to the original state, otherwise, it'll hold null.
   */
  private: State *trunkState;

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

  /// The list of error/warning build messages created for this state.
  private: std::vector<SharedPtr<Common::BuildMsg> > buildMsgs;

  /// Count of build messages in the trunk shared by this state.
  private: Int trunkSharedBuildMsgCount;


  //============================================================================
  // Constructors / Destructor

  public: State();

  public: State(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                Word reservedVarCount, Word reservedVarLevelCount, Data::GrammarModule *rootModule);

  public: State(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                Word reservedVarCount, Word reservedVarLevelCount, const Data::ParsingGrammarContext *context);

  public: ~State()
  {
  }


  //============================================================================
  // Member Variables

  // TODO: Prevent non-const methods from referencing trunk state.

  /// @name Main Functions
  /// @{

  protected: void initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                             Word reservedVarCount, Word reservedVarLevelCount,
                             Data::GrammarModule *rootModule);

  protected: void initialize(Word reservedTermLevelCount, Word reservedProdLevelCount, Word maxVarNameLength,
                             Word reservedVarCount, Word reservedVarLevelCount,
                             const Data::ParsingGrammarContext *context);

  /// Reset the object to an empty state.
  protected: void reset();

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
  public: TermLevel& refTermLevel(Int i);

  /// A const wrappar to refTermLevel().
  public: const TermLevel& refTermLevel(Int i) const
  {
    return const_cast<State*>(this)->refTermLevel(i);
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
  public: TermLevel& refTopTermLevel()
  {
    if (this->topTermLevelCache == 0) {
      throw GeneralException(STR("This state has an empty level stack."),
                             STR("Core::Parser::State::refTopTermLevel"));
    }
    return *(this->topTermLevelCache);
  }

  /// A const wrappar to refTopTermLevel().
  public: const TermLevel& refTopTermLevel() const
  {
    return const_cast<State*>(this)->refTopTermLevel();
  }

  /// Get the number of levels in the top production of this state.
  public: Int getTopprodTermLevelCount() const;

  /// Get the state level of the top production in this state.
  public: TermLevel& refTopprodRootTermLevel()
  {
    // The first level does not belong to any production, so we need at least 2 levels.
    if (this->getTermLevelCount() <= 1) {
      throw GeneralException(STR("This state has an empty level stack."),
                             STR("Core::Parser::State::refTopprodRootTermLevel"));
    }
    return this->refTermLevel(-this->getTopprodTermLevelCount());
  }

  /// A const wrapper to refTopprodRootTermLevel().
  public: const TermLevel& refTopprodRootTermLevel() const
  {
    return const_cast<State*>(this)->refTopprodRootTermLevel();
  }

  /// Push a new level into the top of the level stack.
  protected: void pushTermLevel(Data::Term *term);

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

  public: ProdLevel& refProdLevel(Int i);

  public: const ProdLevel& refProdLevel(Int i) const
  {
    return const_cast<State*>(this)->refProdLevel(i);
  }

  public: ProdLevel& refTopProdLevel()
  {
    if (this->topProdLevelCache == 0) {
      throw GeneralException(STR("This state has an empty stack."),
                             STR("Core::Parser::State::refTopProdLevel"));
    }
    return *(this->topProdLevelCache);
  }

  public: const ProdLevel& refTopProdLevel() const
  {
    return const_cast<State*>(this)->refTopProdLevel();
  }

  protected: void pushProdLevel(Data::Module *module, Data::SymbolDefinition *prod);

  protected: void popProdLevel();

  public: Word getReservedProdLevelCount() const
  {
    return this->prodStack.capacity();
  }

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

  /// @name Term Helper Functions
  /// @{

  public: Word getListTermChildCount(Int levelOffset = -1) const;

  protected: Data::Term* useListTermChild(Int index, Int levelOffset = -1);

  public: Data::Integer* getTokenTermId(Int levelOffset = -1) const;

  public: IdentifiableObject* getTokenTermText(Int levelOffset = -1) const;

  public: void getReferencedDefinition(Data::Module *&module, Data::SymbolDefinition *&definition,
                                       Int levelOffset = -1) const;

  public: Data::Integer* getMultiplyTermMax(Int levelOffset = -1) const;

  public: Data::Integer* getMultiplyTermMin(Int levelOffset = -1) const;

  public: Data::Integer* getMultiplyTermPriority(Int levelOffset = -1) const;

  /// @}

  /// @name Other Functions
  /// @{

  protected: Data::ParsingGrammarContext* getGrammarContext()
  {
    return &this->grammarContext;
  }

  public: const Data::ParsingGrammarContext* getGrammarContext() const
  {
    return &this->grammarContext;
  }

  protected: Data::GrammarHelper* getGrammarHelper()
  {
    return &this->grammarHelper;
  }

  public: const Data::GrammarHelper* getGrammarHelper() const
  {
    return &this->grammarHelper;
  }

  protected: void popLevel()
  {
    if (this->isAtProdRoot()) this->popProdLevel();
    else this->popTermLevel();
  }

  /// @}

  /// @name Branching Functions
  /// @{

  /// Set this state as a branch of another state.
  protected: void setBranchingInfo(State *ts, Int tsi, Int psi);

  protected: void setBranchingInfo(State *ts)
  {
    this->setBranchingInfo(ts, ts->getTermLevelCount()-1, ts->getProdLevelCount()-1);
  }

  /**
   * @brief Get the pointer to the trunk state.
   *
   * @return The pointer to the trunk state from which this state branch, or 0
   *         if this state didn't branch from anything.
   */
  public: State* getTrunkState() const
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

  protected: void copyProdLevel(State *src, Int offset);

  protected: void copyTermLevel(State *src, Int offset);

  /// @}

  /// @name Build Messages Management
  /// @{

  /// Add a build message to the end of the list.
  public: void addBuildMsg(const SharedPtr<Common::BuildMsg> &msg)
  {
    this->buildMsgs.push_back(SharedPtr<Common::BuildMsg>(msg));
  }

  /**
   * @brief Get the count of build messages in this state.
   *
   * This does not include shared messages in the trunk state.
   */
  public: Int getBuildMsgCount() const
  {
    return this->buildMsgs.size();
  }

  /**
   * @brief Get the build message with the given index.
   *
   * @param i The index of the requested message within this state. Messages
   *          contained in the trunk state (if any) that are also shared by
   *          this state are not counted by this index.
   */
  public: const SharedPtr<Common::BuildMsg>& getBuildMsg(Int i) const
  {
    if (static_cast<Word>(i) >= this->buildMsgs.size()) {
      throw InvalidArgumentException(STR("i"), STR("Core::Parser::State::getBuildMsg"),
                                     STR("Index out of range."));
    }
    return this->buildMsgs[i];
  }

  /// Remove a number of build messages from the beginning of the array.
  protected: void flushBuildMsgs(Int count)
  {
    if (count <= 0 || static_cast<Word>(count) > this->buildMsgs.size()) {
      throw InvalidArgumentException(STR("count"), STR("Core::Parser::State::flushBuildMsgs"),
                                     STR("Count is less than 0, or exceeds the total number of notificatoins."));
    }
    this->buildMsgs.erase(this->buildMsgs.begin(), this->buildMsgs.begin()+count);
  }

  /// Set the count of trunk build messages shared by this state.
  protected: void setTrunkSharedBuildMsgCount(Int count);

  /**
     * @brief Get the count of trunk build messages shared by this state.
     * @sa setTrunkSharedBuildMsgCount
     */
  public: Int getTrunkSharedBuildMsgCount() const
  {
    return this->trunkSharedBuildMsgCount;
  }

  /// Copy the shared build messages from the trunk into this state's list.
  protected: void copyTrunkSharedBuildMsgs();

  /// @}

}; // class

} } // namespace

#endif

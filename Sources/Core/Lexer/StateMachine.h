/**
 * @file Core/Lexer/StateMachine.h
 * Contains the header of class Lexer::StateMachine.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef LEXER_STATE_MACHINE_H
#define LEXER_STATE_MACHINE_H

namespace Core { namespace Lexer
{

/**
 * @brief Contains all the functionality of the state machine.
 * @ingroup lexer
 *
 * This class contains all the member variables and functions of the state
 * machine.
 */
class StateMachine : public SignalReceiver
{
  //============================================================================
  // Type Info

  TYPE_INFO(StateMachine, SignalReceiver, "Core.Lexer", "Core", "alusus.net");


  //============================================================================
  // Data Types

  /**
   * @brief Return type of processTempState.
   *
   * Specifies the action to be performed by the caller of processTempState.
   * Depending on the return value of processTempState, the caller (which
   * usually is also processTempState) decides what to do next.<br>
   * The following values are possible:<br>
   *
   * CONTINUE_NEW_CHAR : Tells the caller to proceed after getting a new
   *                       character. It means that the current character was
   *                       accepted by the current branch of the formula and
   *                       that the current branch has reached the end.<br>
   * CONTINUE_SAME_CHAR : Tells the caller to proceed with the same character.
   *                        It means the current character was not used by the
   *                        current branch of the formula, or that it was
   *                        accepted but accepting it was optional.<br>
   * STOP : Tells the caller to stop processing the current branch. It either
   *          means the current branch couldn't proceed, or it means the current
   *          branch didn't finish yet.
   */
  private: enum NextAction
  {
    UNKNOWN_ACTION = 0,
    CONTINUE_NEW_CHAR,
    CONTINUE_SAME_CHAR,
    STOP
  };


  //============================================================================
  // Member Variables

  /// A pointer to the list of token definitions to be used by this object.
  private: SharedPtr<TokenDefinitionList> tokenDefinitions;

  /// A pointer to the list of char group definitions.
  private: SharedPtr<CharGroupDefinitionList> charGroupDefinitions;

  /**
   * @brief The array of current states.
   * This vector contains the array of the states that are currently active
   * in the system.
   */
  private: std::vector<State> states;

  /// A temporary state object used during processing.
  private: State tempState;

  /**
   * @brief The index of the currently disabled state.
   *
   * Rather than deleting it, the state currently being processed is marked
   * by this index as disabled. The state will later be re-enabled at a
   * lower cost than creating a new one. The purpose behind this mechanism
   * is optimization. If this value is -1, then no state is currently
   * disabled.
   */
  private: Int disabledStateIndex;

  /**
   * @brief The buffer of input characters.
   *
   * This buffer contains the list of input characters that will be part of
   * the next token and it also tracks the token position in the source
   * code.
   */
  private: InputBuffer inputBuffer;

  /**
   * @brief Specifies whether the current token has been clamped.
   *
   * When the input buffer is full and a token gets clamped, this value is set
   * to true for a warning to be raised later. After the warning is raised,
   * the value is reset to false.
   */
  private: Bool currentTokenClamped;

  /**
   * @brief The index of the character to be processed next.
   *
   * This value represents the index of the character within inputBuffer to
   * be processed on the next call to "process".
   */
  private: Int currentProcessingIndex;

  /**
   * @brief The information of the matched token.
   *
   * This object will contain the information about the last found token. Each
   * time a new token is found, the values in this object will be overwritten
   * with the new token. The "process" function informs the caller whenever
   * a new value is written to this object and the caller must immediately
   * read this value before calling "process" again.
   */
  private: Common::Token lastToken;

  /**
   * @brief The buffer of error characters.
   *
   * This buffer contains a list of characters that couldn't be matched to
   * any token. This buffer is used to group consecutive error characters
   * instead of issuing an error message on each error character found. This
   * buffer also holds the position of the first error character, which will
   * later be used as the error position when the error is raised.
   */
  private: UnrecognizedCharMsg errorBuffer;


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: SIGNAL(buildMsgNotifier, (const SharedPtr<Common::BuildMsg> &msg), (msg));

  /**
   * @brief A signal to inform targets of a newly generated token.
   *
   * This signal is fired as soon as a new token is generated. Signal
   * receivers should not retain this pointer because its data won't be
   * retained beyond the firing of this signal.
   */
  public: SIGNAL(tokenGenerated, (const Common::Token *token), (token));


  //============================================================================
  // Constructor / Destructor

  public: StateMachine() :
    errorBuffer(STR(""),0,0),
    disabledStateIndex(-1),
    currentProcessingIndex(0),
    currentTokenClamped(false)
  {
  }

  /**
   * @brief Unconnect the currently selected token and char group definitions.
   *
   * If the token and char group definition lists are currently set, it means
   * the char group's charGroupInUseInquirer signal is currently connected
   * to the token definitions list, so now it can be disconnected.
   */
  public: ~StateMachine()
  {
    if (this->charGroupDefinitions != 0 && this->tokenDefinitions != 0) {
      this->charGroupDefinitions->charGroupInUseInquirer.unconnect(this->tokenDefinitions.get(),
                                                                   &TokenDefinitionList::isCharGroupInUse);
    }
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Related Functions
  /// @{

  /// Set the list of token definitions used by this state machine.
  public: void setTokenDefinitions(const SharedPtr<TokenDefinitionList> &list);

  /// Get a pointer to the currently set token definitions list.
  public: const SharedPtr<TokenDefinitionList>& getTokenDefinitions() const
  {
    return this->tokenDefinitions;
  }

  /// Set the list of char group definitions used by this state machine.
  public: void setCharGroupDefinitions(const SharedPtr<CharGroupDefinitionList> &list);

  /// Get a pointer to the currently set char group definitions list.
  public: const SharedPtr<CharGroupDefinitionList>& getCharGroupDefinitions() const
  {
    return this->charGroupDefinitions;
  }

  /// Release all data including parsing data and definitions data.
  public: void release()
  {
    this->clear();
    this->setCharGroupDefinitions(SharedPtr<CharGroupDefinitionList>(0));
    this->setTokenDefinitions(SharedPtr<TokenDefinitionList>(0));
  }

  /// @}

  /// @name Parsing Operations
  /// @{

  /// Add a single input character to the input buffer and process it.
  public: void handleNewChar(Char inputChar, Int line, Int column);

  /// Add a string of input characters to the input buffer and process them.
  public: void handleNewString(const Char *inputStr, Int &line, Int &column);

  /// Process all the characters currently waiting in the input buffer.
  private: void processBuffer();

  /// Push a character into the input buffer.
  private: Bool pushChar(Char ch, Int line, Int column);

  /// Process the given input character by updating the states.
  private: Int process();

  /// Process the first character in the token.
  private: void processStartChar(Char inputChar);

  /// Process the next character in the token.
  private: void processNextChar(Char inputChar);

  /// Recursively apply the given character on the temp state.
  private: NextAction processTempState(Char inputChar, Term *currentTerm,
                                       Int currentLevel);

  /// Select the best token among the detected tokens.
  private: Int selectBestToken();

  /// Match a given character to a character group hierarchy.
  private: static Bool matchCharGroup(Char ch, CharGroupUnit *unit);

  /// Release all states and related data, but not definitions.
  public: void clear();

  /// @}

  /// @name Utility Functions
  /// @{

  /// Create a new state object.
  private: Int createState();

  /// Extract a state from the stack and put it in the temp state.
  private: void extractState(Int index);

  /// Defrag the states list in case there is a disabled state.
  private: void defragStatesList();

  /// Check whether the production with the given id is currently in use.
  public: Bool isTokenInUse(Word production_id);

  /// @}

  /// @name Data Retrieval Functions
  /// @{

  /**
   * @brief Get a pointer to the last token object.
   *
   * This object will contain the information about the last found token. Each
   * time a new token is found, the values in this object will be overwritten
   * with the new token. The "process" function informs the caller whenever
   * a new value is written to this object and the caller must immediately
   * read this value before calling "process" again.
   */
  public: const Common::Token* getLastToken() const
  {
    return &this->lastToken;
  }

  /// @}

}; // class

} } // namespace

#endif

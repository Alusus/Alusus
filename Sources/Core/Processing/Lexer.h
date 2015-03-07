/**
 * @file Core/Processing/Lexer.h
 * Contains the header of class Core::Processing::Lexer.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef PROCESSING_LEXER_H
#define PROCESSING_LEXER_H

namespace Core { namespace Processing
{

// TODO: DOC

/**
 * @brief Contains all the functionality of the state machine.
 * @ingroup processing_lexer
 *
 * This class contains all the member variables and functions of the state
 * machine.
 */
class Lexer : public SignalReceiver
{
  //============================================================================
  // Type Info

  TYPE_INFO(Lexer, SignalReceiver, "Core.Processing", "Core", "alusus.net");


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
   *                     character. It means that the current character was
   *                     accepted by the current branch of the formula and
   *                     that the current branch has reached the end.<br>
   * CONTINUE_SAME_CHAR : Tells the caller to proceed with the same character.
   *                      It means the current character was not used by the
   *                      current branch of the formula, or that it was
   *                      accepted but accepting it was optional.<br>
   * STOP : Tells the caller to stop processing the current branch. It either
   *        means the current branch couldn't proceed, or it means the current
   *        branch didn't finish yet.
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

  /// A pointer to the repository that contains token and chargroup definitions.
  private: Data::GrammarRepository *grammarRepository;

  /// The context used to tracer through the grammar.
  private: Data::GrammarContext grammarContext;

  /**
   * @brief The array of current states.
   * This vector contains the array of the states that are currently active
   * in the system.
   */
  private: std::vector<LexerState> states;

  /// A temporary state object used during processing.
  private: LexerState tempState;

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
   * @brief A temporary buffer used to buffer byte characters for conversion.
   * This buffer is used to buffer the received byte characters when multi
   * byte sequences are received.
   */
  private: Char tempByteCharBuffer[4];

  /**
   * @brief Current count of bytes in tempByteCharBuffer.
   * @sa tempByteCharBuffer
   */
  private: Word tempByteCharCount;

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
  private: Data::Token lastToken;

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
  public: SIGNAL(buildMsgNotifier, (const SharedPtr<Processing::BuildMsg> &msg), (msg));

  /**
   * @brief A signal to inform targets of a newly generated token.
   *
   * This signal is fired as soon as a new token is generated. Signal
   * receivers should not retain this pointer because its data won't be
   * retained beyond the firing of this signal.
   */
  public: SIGNAL(tokenGenerated, (const Data::Token *token), (token));


  //============================================================================
  // Constructor / Destructor

  public: Lexer() :
    grammarRepository(0),
    errorBuffer(STR(""), Data::SourceLocation()),
    disabledStateIndex(-1),
    tempByteCharCount(0),
    currentProcessingIndex(0),
    currentTokenClamped(false)
  {
  }

  public: virtual ~Lexer()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Related Functions
  /// @{

  /// Set the repository containing token and chargroup definitions.
  public: void setGrammarRepository(Data::GrammarRepository *grammarRepo);

  public: Data::GrammarRepository* getGrammarRepository() const
  {
    return this->grammarRepository;
  }

  /// Release all data including parsing data and definitions data.
  public: void release()
  {
    this->clear();
    this->setGrammarRepository(0);
  }

  /// @}

  /// @name Parsing Operations
  /// @{

  /// Add a single input character to the input buffer and process it.
  public: void handleNewChar(Char inputChar, Data::SourceLocation &sourceLocation);

  /// Add a string of input characters to the input buffer and process them.
  public: void handleNewString(Char const *inputStr, Data::SourceLocation &sourceLocation);

  /// Process all the characters currently waiting in the input buffer.
  private: void processBuffer();

  /// Push a character into the input buffer.
  private: Bool pushChar(WChar ch, Data::SourceLocation const &sl);

  /// Process the given input character by updating the states.
  private: Int process();

  /// Process the first character in the token.
  private: void processStartChar(WChar inputChar);

  /// Process the next character in the token.
  private: void processNextChar(WChar inputChar);

  /// Recursively apply the given character on the temp state.
  private: NextAction processTempState(WChar inputChar, Data::Term *currentTerm,
                                       Int currentLevel);

  /// Select the best token among the detected tokens.
  private: Int selectBestToken();

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

  /// Get the symbol definition at the specified index.
  public: Data::SymbolDefinition* getSymbolDefinition(Int index)
  {
    Data::SymbolDefinition *def = static_cast<Data::SymbolDefinition*>(this->grammarContext.getModule()->get(index));
    ASSERT(def->isA<Data::SymbolDefinition>());
    return def;
  }

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
  public: const Data::Token* getLastToken() const
  {
    return &this->lastToken;
  }

  /// @}

}; // class

} } // namespace

#endif

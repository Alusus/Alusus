/**
 * @file Core/Lexer/StateMachine.cpp
 * Contains the implementation of Lexer::StateMachine.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Lexer
{

//==============================================================================
// Member Functions

/**
 * This function will fail if we have any outstanding states. If list is not 0
 * the function will connect its tokenInUseInquirer signal to this
 * class' isTokenInUse function. This signal will guarantee that no
 * token can be removed while any state in this state machine is still
 * using it. If a previous list was set, its signal will first be unconnected.
 * This function also makes sure the new token definitions is connected to the
 * currently selected char group definition list's is_char_group_in_use_inquirer
 * signal to make sure a char group is not removed while being used by anyone.
 *
 * @param list A smart pointer to the list to use. Passing 0 in this parameter
 *             will result only in unsetting the previous list, if any.
 */
void StateMachine::setTokenDefinitions(const SharedPtr<TokenDefinitionList> &list)
{
  // Before we can change the token list, we need to make sure we have no outstanding
  // states.
  if (!this->states.empty()) {
    throw GeneralException(STR("Token definitions list can't be changed while being in use."),
                           STR("Core::Lexer::StateMachine::setTokenDefinitions"));
  }
  // If we currently have a token definitions list, we need to unset it first.
  if (this->tokenDefinitions != 0) {
    this->tokenDefinitions->tokenInUseInquirer.unconnect(this, &StateMachine::isTokenInUse);
    // If we currently have a char group definitions list as well, we need to unset the connection first.
    if (this->charGroupDefinitions != 0) {
      this->charGroupDefinitions->charGroupInUseInquirer.unconnect(this->tokenDefinitions.get(),
                                                                   &TokenDefinitionList::isCharGroupInUse);
    }
  }
  // Set the new list.
  this->tokenDefinitions = list;
  // If we have a new token list, we need to set the tokenInUseInquirer signal.
  if (this->tokenDefinitions != 0) {
    this->tokenDefinitions->tokenInUseInquirer.connect(this, &StateMachine::isTokenInUse);
    // If we have a new char group list as well, we need to set the charGroupInUseInquirer signal.
    if (this->charGroupDefinitions != 0) {
      this->charGroupDefinitions->charGroupInUseInquirer.connect(this->tokenDefinitions.get(),
                                                                 &TokenDefinitionList::isCharGroupInUse);
    }
  }
}


/**
 * This function will fail if we have any outstanding states. If list is not 0
 * the function will connect its charGroupInUseInquirer signal to this
 * class' tokenDefinitions list. This signal will guarantee that no
 * char group can be removed while any token definition used by this state
 * machine is still using it. If a previous list was set, its signal will first
 * be unconnected.
 *
 * @param list A smart pointer to the list to use. Passing 0 in this parameter
 *             will result only in unsetting the previous list, if any.
 */
void StateMachine::setCharGroupDefinitions(const SharedPtr<CharGroupDefinitionList> &list)
{
  // Before we can change the token list, we need to make sure we have no outstanding
  // states.
  if (!this->states.empty()) {
    throw GeneralException(STR("Char group definitions list can't be changed while state machine is active (has states)."),
                           STR("Core::Lexer::StateMachine::setCharGroupDefinitions"));
  }
  // If we currently have a token definitions list, we need to unset the connection first.
  if (this->charGroupDefinitions != 0 && this->tokenDefinitions != 0) {
    this->charGroupDefinitions->charGroupInUseInquirer.unconnect(this->tokenDefinitions.get(),
                                                                 &TokenDefinitionList::isCharGroupInUse);
  }
  // Set the new list.
  this->charGroupDefinitions = list;
  // If we have a new char group list, we need to set the charGroupInUseInquirer signal.
  if (this->charGroupDefinitions != 0 && this->tokenDefinitions != 0) {
    this->charGroupDefinitions->charGroupInUseInquirer.connect(this->tokenDefinitions.get(),
                                                               &TokenDefinitionList::isCharGroupInUse);
  }
}


/**
 * Add a single character to the input buffer and keep processing until no more
 * characters are in the input buffer.
 *
 * @param inputChar The character to add to the input buffer.
 * @param line The source code line at which the character appeared.
 * @param column The source code column at which the character appeared.
 */
void StateMachine::handleNewChar(Char inputChar, Int line, Int column)
{
  this->pushChar(inputChar, line, column);
  this->processBuffer();
}


/**
 * Add a string to the input buffer and keep processing until no more characters
 * are in the input buffer.
 *
 * @param inputStr The string to add to the input buffer.
 * @param line The source code line at which the first character in the string
 *             appeared. This will be updated with the new location.
 * @param column The source code column at which the first character in the
 *               string appeared. This will be updated with the new location.
 */
void StateMachine::handleNewString(const Char *inputStr, Int &line, Int &column)
{
  for (Int i = 0; i < static_cast<Int>(strlen(inputStr)); i++) {
    this->handleNewChar(inputStr[i], line, column);
    if (inputStr[i] == CHR('\n')) {
      line++;
      column = 1;
    } else if (inputStr[i] == CHR('\r')) {
      column = 1;
    } else {
      column++;
    }
  }
}


/**
 * Keep processing the input buffer until it has no more input characters.
 */
void StateMachine::processBuffer()
{
  while (true) {
    Int r = this->process();

    if (r & 1) {
      this->tokenGenerated.emit(this->getLastToken());
    }
    if (r & 2) break;
  }
}


/**
 * Push a character into the input buffer. The characters will be kept in
 * the buffer until they are processed by the process function. Calling
 * pushChar multiple times before calling process is also possible.
 * In case of very long tokens that exceed the maximum capacity of the input
 * buffer the function will start replacing the last character in the buffer
 * in order to allow the state machine to perform the parsing correctly even
 * though the returned token is clipped. However, the function won't replace
 * character unless it's sure it won't need the replaced characters anymore
 * and that can only be guaranteed if all the characters currently in the
 * buffer are already processed and no closed states are available in the
 * stack.
 *
 * @param ch The character to add to the input buffer.
 * @param line The line at which the character appeared in the source code.
 * @param column The column at which the character appeared in the source code.
 * @return Returns true if the character was inserted, false otherwise.
 */
Bool StateMachine::pushChar(Char ch, Int line, Int column)
{
  // Is the input buffer full?
  if (this->inputBuffer.isFull()) {
    // Check if all the characters in the buffer are already processed.
    if (this->currentProcessingIndex >= this->inputBuffer.getCharCount()) {
      // Check if there are any closed state.
      Int closedStateCount = 0;
      for (Word i = 0; i < this->states.size(); i++) {
        if (this->states[i].getTokenLength() != 0) closedStateCount++;
      }
      if (closedStateCount == 0) {
        // There are no closed states, so replace the last character.
        this->inputBuffer.push(ch, line, column, true);
        this->currentProcessingIndex--;
        this->currentTokenClamped = true;
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    // The buffer is not full, so push the new character.
    this->inputBuffer.push(ch, line, column, false);
    return true;
  }
}


/**
 * Processes a single character from the input buffer by updating the states and
 * checking if a single token has been recognized. Whenever the function finds a
 * new token, it writes it on a single token member variable overwriting any
 * previously found tokens. The caller then must immediately read that token
 * before any next call is made on this function. Similarly, when an error is
 * found, the function writes the error text on a single member variable that
 * must be read immediately by the caller.<br>
 * When a new character doesn't match any token, the function doesn't report the
 * error immediately; instead, it keeps it in an error buffer until the next
 * time a token is found. This is done to avoid raising an error on each single
 * character; instead a single error message is raised for each group of
 * consecutive error characters.<br>
 * The function doesn't accepts a token until it makes sure it can't proceed
 * processing more characters without accepting one of the already found tokens
 * and starting a new token, at which point the function might revert back to
 * a token shorter than what's already been processed, which causes this
 * function to put some already processed characters back into the input buffer
 * to be reused at later calls to this function. The function will inform the
 * caller through the return value whether it has characters to process or
 * it needs more characters to be supplied by the caller before any next
 * call to process.<br>
 * For example, if a "+" character is matched, the function waits to make sure
 * this is not a "++" or "+=" tokens, so it process the next character which
 * then turns out to be "n". At this point, the "n" is already processed but the
 * accepted token is only "+", not "+n". So the function chooses "+" as a token
 * and returns "n" back to the input buffer and informs the caller that a token
 * has been found and that the function still has input characters to process.
 *
 * @return The bits of the return value has the following meanings:<br>
 *           Bit 0: Specifies whether a token was found. If the value is 1, then a
 *                  token is found and the caller must call getLastToken() to
 *                  read the value of that token.<br>
 *           Bit 1: Specifies whether the input buffer is empty or not. A value of
 *                  1 means the buffer is empty and the caller needs to push more
 *                  characters into it by calling pushChar.
 */
Int StateMachine::process()
{
  // Make sure the token definition list is defined.
  if (this->tokenDefinitions == 0) {
    throw GeneralException(STR("Token definitions list is not set."), STR("Core::Lexer::StateMachine::process()"));
  }

  // Make sure there are characters in the input buffer.
  // If there isn't, inform the caller to add new characters.
  if (this->currentProcessingIndex >= this->inputBuffer.getCharCount()) return 2;

  // Get the processing character.
  Char inputChar = this->inputBuffer.getChars()[this->currentProcessingIndex];

  // Check if this is the first character.
  if (this->currentProcessingIndex == 0) {
    this->processStartChar(inputChar);
  } else {
    this->processNextChar(inputChar);
  }

  Int r = 0;

  // Check the current status of the states:
  //   If there are open states, wait.
  //   If there are no states, move whatever characters in the buffer to the error buffer.
  //   If there are only closed states, choose the token then reuse the remaining characters.
  //   If there are any closed states or if the input buffer contains only one character
  //   which is FILE_TERMINATOR, report whatever characters in the error buffer.

  Int openStateCount = 0;
  Int closedStateCount = 0;
  for (Word i = 0; i < this->states.size(); i++) {
    if (this->states[i].getTokenLength() == 0) openStateCount++;
    else closedStateCount++;
  }
  if (openStateCount > 0) {
    // If the buffer is full and we have closed states, then we should choose one of them,
    // otherwise, wait until the open states are closed or deleted.
    if (closedStateCount > 0 && this->inputBuffer.isFull() == true &&
        this->currentProcessingIndex >= this->inputBuffer.getCharCount()-1) {
      // Raise a warning.
      this->buildMsgNotifier.emit(
            SharedPtr<Common::BuildMsg>(new BufferFullMsg(this->inputBuffer.getStartLine(),
                                                          this->inputBuffer.getStartColumn())));
      // Choose one of the closed states.
      Int i = this->selectBestToken();
      Word id = this->tokenDefinitions->get(this->states[i].getIndexStack()->at(0))->getId();
      // Check if the chosen token is not an ignored token.
      if (id < IGNORED_TOKEN_BASE) {
        // Has the token been clamped?
        if (this->currentTokenClamped) {
          // Raise a warning.
          this->buildMsgNotifier.emit(
                SharedPtr<Common::BuildMsg>(new TokenClampedMsg(this->inputBuffer.getStartLine(),
                                                                this->inputBuffer.getStartColumn())));
          this->currentTokenClamped = false;
        }
        // Set token properties.
        this->lastToken.setId(id);
        this->lastToken.setLine(this->inputBuffer.getStartLine());
        this->lastToken.setColumn(this->inputBuffer.getStartColumn());
        this->lastToken.setText(this->inputBuffer.getChars(), this->states[i].getTokenLength());
        // Inform the caller that there is a new token.
        r |= 1;
      }
      // Reuse the remaining characters in the input buffer.
      this->inputBuffer.remove(this->states[i].getTokenLength());
      // Set the processing index to -1 since the character we are currently processing is shifted
      // out of the buffer.
      this->currentProcessingIndex = -1;
      // Delete all the states.
      this->states.clear();
    } else if (closedStateCount > 1) {
      // Keep only the best closed state to conserve memory.
      // Select a token among the closed states.
      Int bestToken = this->selectBestToken();
      // Remove all closed tokens except for the selected one.
      for (Int i = 0; i < static_cast<Int>(this->states.size()); i++) {
        if (this->states[i].getTokenLength() != 0 && i != bestToken) {
          // Remove this unselected closed token.
          this->disabledStateIndex = i;
          this->defragStatesList();
          // Check if the moved state was actually the selected one.
          if (bestToken >= static_cast<Int>(this->states.size())) bestToken = i;
        }
      }
    }
  } else if (closedStateCount > 0) {
    // Choose one of the closed states.
    Int i = this->selectBestToken();
    Word id = this->tokenDefinitions->get(this->states[i].getIndexStack()->at(0))->getId();
    // Check if the chosen token is not an ignored token.
    if (id < IGNORED_TOKEN_BASE) {
      // Has the token been clamped?
      if (this->currentTokenClamped) {
        // Raise a warning.
        this->buildMsgNotifier.emit(
              SharedPtr<Common::BuildMsg>(new TokenClampedMsg(this->inputBuffer.getStartLine(),
                                                              this->inputBuffer.getStartColumn())));
        this->currentTokenClamped = false;
      }
      // Set token properties.
      this->lastToken.setId(id);
      this->lastToken.setLine(this->inputBuffer.getStartLine());
      this->lastToken.setColumn(this->inputBuffer.getStartColumn());
      this->lastToken.setText(this->inputBuffer.getChars(), this->states[i].getTokenLength());
      // Inform the caller that there is a new token.
      r |= 1;
    }
    // Reuse the remaining characters in the input buffer.
    this->inputBuffer.remove(this->states[i].getTokenLength());
    // Set the processing index to -1 since the character we are currently processing is shifted
    // out of the buffer.
    this->currentProcessingIndex = -1;
    // Delete all the states.
    this->states.clear();
  } else {
    // No states are still alive, so move the first character in the input buffer to the error
    // buffer and try again.
    Str err;
    Int line = -1;
    Int column = -1;
    if (this->inputBuffer.getChars()[0] != FILE_TERMINATOR) {
      err.assign(this->inputBuffer.getChars(), 1);
      line = this->inputBuffer.getStartLine();
      column = this->inputBuffer.getStartColumn();
      this->inputBuffer.remove(1);
    }
    // limit the error text to ERROR_BUFFER_MAX_CHARACTERS
    if (this->errorBuffer.getTextLength() < ERROR_BUFFER_MAX_CHARACTERS) {
      this->errorBuffer.appendText(err.c_str(), line, column);
    }
    // Set the processing index to -1 since the character we are currently processing is shifted
    // out of the buffer.
    this->currentProcessingIndex = -1;
  }

  // Is it time to report any characters in the error buffer (found a token or finished the file)?
  if (closedStateCount > 0 ||
      (this->inputBuffer.getCharCount()==1 &&
       this->inputBuffer.getChars()[0]==FILE_TERMINATOR)) {
    // Report any characters in the error buffer.
    if (this->errorBuffer.getTextLength() > 0) {
      this->buildMsgNotifier.emit(
            SharedPtr<Common::BuildMsg>(new UnrecognizedCharMsg(this->errorBuffer.getText().c_str(),
                                                                this->errorBuffer.getLine(),
                                                                this->errorBuffer.getColumn())));
      this->errorBuffer.clear();
    }
  }

  // Update the processing index.
  this->currentProcessingIndex++;

  // Check if the only remaining character is a single FILE_TERMINATOR.
  if (this->inputBuffer.getCharCount() == 1 &&
      this->inputBuffer.getChars()[0] == FILE_TERMINATOR) {
    // There should be no more open states at this point.
    ASSERT(openStateCount == 0);
    this->inputBuffer.clear();
  }

  // Check if we need more characters to be added to the input buffer.
  if (this->currentProcessingIndex >= this->inputBuffer.getCharCount()) r |= 2;

  return r;
}


/**
 * Processes the first character in the token by searching the list of token
 * types and creating a state object for each token that can start with the
 * given character.
 *
 * @param inputChar The input character to search against.
 */
void StateMachine::processStartChar(Char inputChar)
{
  // There must not be any state currently in the stack.
  ASSERT(this->states.size() == 0);

  LOG(LogLevel::LEXER_MID, STR("Starting a new token. New char: '") << inputChar << STR("'"));

  for (Word i = 0; i < this->tokenDefinitions->getCount(); i++) {
    // Skip inner tokens.
    if (this->tokenDefinitions->get(i)->isInner()) continue;
    // Validation.
    if (this->tokenDefinitions->get(i)->getFormulaHead() == 0) {
      Str excMsg = STR("Invalid token definition (");
      excMsg += this->tokenDefinitions->get(i)->getId();
      excMsg += STR("). The definition formula is not set yet.");
      throw GeneralException(excMsg.c_str(), STR("Core::Lexer::StateMachine::processStartChar"));
    }
    // Set the first entry in the state index stack to the token definition index.
    this->tempState.getIndexStack()->resize(1);
    this->tempState.getIndexStack()->at(0) = i;
    // Reset the token length.
    this->tempState.setTokenLength(0);
    // Process the token.
    switch (this->processTempState(inputChar, this->tokenDefinitions->get(i)->getFormulaHead().get(),
                                   1)) {
      case CONTINUE_NEW_CHAR:
        // The character was accepted and a full token is formed.
        this->tempState.setTokenLength(1);
        this->createState();
        break;
      case CONTINUE_SAME_CHAR:
        // This should never be reached.
        ASSERT(false);
        break;
    }
  }

#ifdef USE_LOGS
  for (Int i = 0; i < static_cast<Int>(this->states.size()); ++i) {
    Int index = this->states[i].getIndexStack()->at(0);
    Int id = this->tokenDefinitions->get(index)->getId() & (~IGNORED_TOKEN_BASE);
    LOG(LogLevel::LEXER_MID, STR("Starting Token: ") << ID_GENERATOR->getDesc(id));
  }
#endif
}


/**
 * Processes the next character in the token by updating the given state with
 * the given input character. Updating the state can also result in a new state
 * being created, or the given state being deleted.
 *
 * @param inputChar The next input character received from the input stream.
 *                     Receiving the value of '\0' indicates that there are no
 *                     more characters in the input stream.
 */
void StateMachine::processNextChar(Char inputChar)
{
  // There must be some states currently in the stack.
  ASSERT(this->states.size() != 0);

  LOG(LogLevel::LEXER_MID, STR("Processing new character: '") << inputChar << STR("'"));

  // Make sure not to process newly created states by limiting the loop to the
  // current states count.
  Int statesCount = this->states.size();
  for (Int i = 0; i < statesCount; i++) {
    if (this->states[i].getTokenLength() > 0) continue;
    // Extract the state into the temp state.
    this->extractState(i);
    // Get the formula head of the state.
    Term *head =
        this->tokenDefinitions->get(this->tempState.getIndexStack()->at(0))->getFormulaHead().get();
    // Process the token.
    switch (this->processTempState(inputChar, head, 1)) {
      case CONTINUE_NEW_CHAR:
        // The character was accepted and a full token is formed.
        this->tempState.setTokenLength(this->currentProcessingIndex+1);
        this->createState();
        break;
      case CONTINUE_SAME_CHAR:
        // The character was not accepted, but a full token is formed.
        this->tempState.setTokenLength(this->currentProcessingIndex);
        this->createState();
        break;
    }
    // Check if we need to defrag the stack.
    if (this->disabledStateIndex != -1) {
      // If the current state count is equal to the original count then
      // one of the current states will be moved to the position of the
      // disabled state, so we need to make sure this state is not
      // ignored.
      if (static_cast<Int>(this->states.size()) == statesCount) {
        statesCount--;
        i--;
      }
      // Defrag the states list.
      this->defragStatesList();
    }
  }

#ifdef USE_LOGS
  for (Int i = 0; i < static_cast<Int>(this->states.size()); ++i) {
    Int index = this->states[i].getIndexStack()->at(0);
    Int id = this->tokenDefinitions->get(index)->getId() & (~IGNORED_TOKEN_BASE);
    LOG(LogLevel::LEXER_MID, STR("Keeping Token: ") << ID_GENERATOR->getDesc(id));
  }
#endif
}


/**
 * Apply the given input character on the temp state object creating new state
 * objects if necessary. This recursive function is the core of the state
 * machine. It recursively handles all the possibilities within the hierarchy
 * of terms.
 *
 * @param inputChar The next input character received from the input stream.
 *                     Receiving the value of '\0' indicates that there are no
 *                     more characters in the input stream.
 * @param currentTerm A pointer to the term object currently being processed.
 * @param currentLevel The index of the entry within the state's internal
 *                        stack that is associated with the current term object.
 * @return Returns a NextAction value telling the caller what to do next.
 */
StateMachine::NextAction StateMachine::processTempState(Char inputChar, Term *currentTerm,
                                                        Int currentLevel)
{
  // make sure the current level isn't out of range
  ASSERT(currentLevel <= static_cast<Int>(this->tempState.getIndexStack()->size()));

  // check the term type
  switch (currentTerm->getType().val) {
    case TermType::CONST: {
      Int charIndex;
      ConstTerm *constTerm = static_cast<ConstTerm*>(currentTerm);
      if (constTerm->getMatchString().size() == 0) {
        throw GeneralException(STR("Const term match string is not set yet."),
                               STR("Core::Lexer::StateMachine::processTempState"));
      }
      // this type of terms is always the deepest level.
      ASSERT(currentLevel >= static_cast<Int>(this->tempState.getIndexStack()->size())-1);
      // get the index within the term.
      if (currentLevel == static_cast<Int>(this->tempState.getIndexStack()->size())) {
        charIndex = 0;
      } else {
        charIndex = this->tempState.getIndexStack()->at(currentLevel);
      }
      // assert that charIndex is not out of the range of the constant.
      ASSERT(charIndex < static_cast<Int>(constTerm->getMatchString().size()));
      // check if the next character is accepted.
      if (constTerm->getMatchString()[charIndex] == inputChar) {
        charIndex++;
        // Check if we reached the end of the string.
        if (charIndex == static_cast<Int>(constTerm->getMatchString().size())) {
          // Remove charIndex from the stack, if any.
          if (currentLevel < static_cast<Int>(this->tempState.getIndexStack()->size())) {
            this->tempState.getIndexStack()->pop_back();
          }
          return CONTINUE_NEW_CHAR; // No more chars is expected on this term object.
        } else {
          // Add or update charIndex on the stack.
          if (currentLevel == static_cast<Int>(this->tempState.getIndexStack()->size())) {
            this->tempState.getIndexStack()->push_back(charIndex);
          } else {
            this->tempState.getIndexStack()->at(currentLevel) = charIndex;
          }
          // Push the temp state onto the states stack.
          this->createState();

          return STOP; // More chars is expected on this term object.
        }
      } else {
        return STOP; // This term object failed to match.
      }
      break;
    }

    case TermType::CHAR_GROUP: {
      if (this->charGroupDefinitions == 0) {
        throw GeneralException(STR("Char group definitions list is not set."),
                               STR("Core::Lexer::StateMachine::processTempState()"));
      }
      CharGroupTerm *charGroupTerm = static_cast<CharGroupTerm*>(currentTerm);
      if (charGroupTerm->getCharGroupUnit() == 0) {
        CharGroupDefinition * cgd =
            this->charGroupDefinitions->find(charGroupTerm->getCharGroupId()).get();
        // Check for null cgd.
        if (cgd == 0) {
          Str excMsg = STR("Invalid character group id (");
          excMsg += charGroupTerm->getCharGroupId();
          excMsg += STR(")");
          throw GeneralException(excMsg.c_str(), STR("Core::Lexer::StateMachine::processTempState"));
        }
        if (cgd->getCharGroupUnit() == 0) {
          Str excMsg = STR("Invalid character group (");
          excMsg += cgd->getId();
          excMsg += STR("). The definition formula is not set yet.");
          throw GeneralException(excMsg.c_str(), STR("Core::Lexer::StateMachine::processTempState"));
        }
        // Set the char group.
        charGroupTerm->setCharGroupUnit(cgd->getCharGroupUnit());
      }
      if (StateMachine::matchCharGroup(inputChar, charGroupTerm->getCharGroupUnit().get())) {
        return CONTINUE_NEW_CHAR;
      } else {
        return STOP;
      }
      break;
    }

    case TermType::OPTIONAL: {
      // HOW IT WORKS:
      // When parsing reaches the optional term for the first time a state
      // will be created for the parsing path that goes through the
      // optional term, and a CONTINUE_SAME_CHAR return value will be
      // returned regardless of whether the optional term accepted the
      // input character or not. When parsing reaches the optional term
      // later (through the created state) the parsing will simply be
      // passed on to the optional term.
      // When the optional term is reached for the first time, an index
      // value of 0 will be inserted if the optional term is waiting for
      // more characters. If the optional term accepted the input char
      // and reached its end then an index value of -1 will be inserted
      // to inform the function later to return CONTINUE_SAME_CHAR on
      // the next input character.

      OptionalTerm *optionalTerm = static_cast<OptionalTerm*>(currentTerm);
      if (optionalTerm->getChildTerm() == 0) {
        throw GeneralException(STR("Optional term's child term is not set yet."),
                               STR("Core::Lexer::StateMachine::processTempState"));
      }
      // Get the index within the term.
      if (currentLevel == static_cast<Int>(this->tempState.getIndexStack()->size())) {
        // Just entering the term for the first time.
        Term *term = optionalTerm->getChildTerm().get();
        ASSERT(term != 0);
        // add the current index
        this->tempState.getIndexStack()->push_back(0);
        NextAction ret = this->processTempState(inputChar, term, currentLevel+1);
        if (ret == CONTINUE_NEW_CHAR) {
          this->tempState.getIndexStack()->at(currentLevel) = -1;
          this->createState();
        }
        // remove any added index
        this->tempState.getIndexStack()->resize(currentLevel);
        return CONTINUE_SAME_CHAR;
      } else {
        // Returning to the term a second time.
        // Check if this is a delayed state
        if (this->tempState.getIndexStack()->at(currentLevel) == -1) {
          // This is a delayed state, so just return to let the parent branch continue
          ASSERT(static_cast<Int>(this->tempState.getIndexStack()->size()) == currentLevel+1);
          this->tempState.getIndexStack()->pop_back();
          return CONTINUE_SAME_CHAR;
        } else {
          // Get the term object.
          Term *term = optionalTerm->getChildTerm().get();
          ASSERT(term != 0);
          // Call the inner branch.
          NextAction ret = this->processTempState(inputChar, term, currentLevel+1);
          if (ret == CONTINUE_NEW_CHAR || ret == CONTINUE_SAME_CHAR) {
            this->tempState.getIndexStack()->pop_back();
          }
          return ret;
        }
      }
      break;
    }

    case TermType::DUPLICATE: {
      // HOW IT WORKS:
      // The term index of the duplicate term will contain the iteration
      // index (starting from 0). Each time parsing finishes going through
      // the contained term, the iteration index will be incremented and
      // a new state will be created. Each time parsing starts a new
      // iteration (determined by whether an index for the deeper level is
      // created or not), it will determine the return value depending on
      // whether the iteration index is less than the required minimum
      // occurances. If the minimum occurances is met (the duplicate term
      // has appeared for at least the required minimum) the return value
      // will be CONTINUE_SAME_CHAR to tell the upper level to continue
      // parsing causing two different paths to go on at the same time. If
      // the minimum occurances is not met the return value will be STOP
      // to prevent the upper level from creating an invalid path that
      // doesn't meet the minimum occurances requirement.

      Int iterationIndex = 0;
      DuplicateTerm *duplicateTerm = static_cast<DuplicateTerm*>(currentTerm);
      if (duplicateTerm->getChildTerm() == 0) {
        throw GeneralException(STR("Duplicate term's child term is not set yet."),
                               STR("Core::Lexer::StateMachine::processTempState"));
      }
      // Get the index within the term.
      if (currentLevel < static_cast<Int>(this->tempState.getIndexStack()->size())) {
        iterationIndex = this->tempState.getIndexStack()->at(currentLevel);
      }
      // Are we in the middle of the duplicate term?
      if (static_cast<Int>(this->tempState.getIndexStack()->size()) <= currentLevel+1) {
        // We are at the beginning of the duplicate term
        Term *term = duplicateTerm->getChildTerm().get();
        ASSERT(term != 0);
        // add the current index
        this->tempState.getIndexStack()->resize(currentLevel+1);
        this->tempState.getIndexStack()->at(currentLevel) = iterationIndex;
        NextAction ret = this->processTempState(inputChar, term, currentLevel+1);
        if (ret == CONTINUE_NEW_CHAR &&
            (duplicateTerm->getMaxOccurances() == 0 ||
             duplicateTerm->getMaxOccurances() > iterationIndex+1)) {
          this->tempState.getIndexStack()->at(currentLevel) = iterationIndex+1;
          this->createState();
        }
        // remove any added index
        this->tempState.getIndexStack()->resize(currentLevel);
        // Check if we have already passed the required minimum number of occurances.
        if (iterationIndex < duplicateTerm->getMinOccurances()) return STOP;
        else return CONTINUE_SAME_CHAR;
      } else {
        // We are in the middle of the duplicate term.
        // Get the term object.
        Term *term = duplicateTerm->getChildTerm().get();
        ASSERT(term != 0);
        // Call the inner branch.
        NextAction ret = this->processTempState(inputChar, term, currentLevel+1);
        if (ret == CONTINUE_SAME_CHAR) {
          // The same character can be used again, so restart on the same term.
          iterationIndex++;
          this->tempState.getIndexStack()->at(currentLevel) = iterationIndex;
          ret = this->processTempState(inputChar, term, currentLevel+1);
          // NOTE: We don't need to check again for the CONTINUE_SAME_CHAR return value
          //       since that's an illogical infinite loop.
        }
        if (ret == CONTINUE_NEW_CHAR &&
            (duplicateTerm->getMaxOccurances() == 0 ||
             duplicateTerm->getMaxOccurances() > iterationIndex+1)) {
          // Create a state to repeat the term later.
          iterationIndex++;
          this->tempState.getIndexStack()->at(currentLevel) = iterationIndex;
          this->createState();
          this->tempState.getIndexStack()->pop_back();
        }
        return ret;
      }
      break;
    }

    case TermType::ALTERNATIVE: {
      // HOW IT WORKS:
      // When the parsing reaches the alternative term for the first time
      // a state will be created for each branch and parsing for the
      // current state will end. When the parsing reaches the alternative
      // term later (through one of the states created in the first visit)
      // the parsing will simply be passed on to the selected branch
      // without paying any attention to other branches of the alternative
      // term.
      // However, when the alternative term is reached for the first time
      // and the called branches return with a CONTINUE return value
      // (rather than waiting for more characters), the function will
      // return CONTINUE as the return value from the alternative term.
      // Special Case:
      // In a first visit, when two branches return with values of
      // CONTINUE_SAME_CHAR and CONTINUE_NEW_CHAR, the function creates
      // a delayed state and returns CONTINUE_SAME_CHAR. The delayed
      // state has an index of -1 for the level of the alternative term.
      // When the alternative term is reached having an index of -1, it
      // will simply return CONTINUE_SAME_CHAR. The result of this
      // approach will be returning CONTNUE_SAME_CHAR and waiting until
      // the next char to return another CONTINUE_SAME_CHAR, which is
      // similar to returning CONTINUE_NEW_CHAR immediately.

      Int termIndex;
      AlternativeTerm *alternativeTerm = static_cast<AlternativeTerm*>(currentTerm);
      if (alternativeTerm->getChildTermCount() < 2) {
        throw GeneralException(STR("Alternative term doesn't have enough branches yet (less than two)."),
                               STR("Core::Lexer::StateMachine::processTempState"));
      }
      // Get the index within the term.
      if (currentLevel == static_cast<Int>(this->tempState.getIndexStack()->size())) {
        // Just entering the term for the first time.
        // Loop through all the alternatives of the term.
        NextAction ret = UNKNOWN_ACTION;
        Bool delayedStateCreated = false;
        termIndex = 0;
        Term *term = alternativeTerm->getChildTermsHead().get();
        while (term) {
          // add the current index
          this->tempState.getIndexStack()->push_back(termIndex);
          switch (this->processTempState(inputChar, term, currentLevel+1)) {
            case CONTINUE_NEW_CHAR:
              if (ret == CONTINUE_SAME_CHAR) {
                if (!delayedStateCreated) {
                  // create delayed state
                  this->tempState.getIndexStack()->at(currentLevel) = -1;
                  this->createState();
                  delayedStateCreated = true;
                }
              } else {
                ret = CONTINUE_NEW_CHAR;
              }
              break;
            case CONTINUE_SAME_CHAR:
              if (ret == CONTINUE_NEW_CHAR) {
                if (!delayedStateCreated) {
                  // create delayed state
                  this->tempState.getIndexStack()->at(currentLevel) = -1;
                  this->createState();
                  delayedStateCreated = true;
                }
              }
              ret = CONTINUE_SAME_CHAR;
              break;
            case STOP:
              if (ret == UNKNOWN_ACTION) ret = STOP;
              break;
          }
          // remove any added index
          this->tempState.getIndexStack()->resize(currentLevel);
          // move to next term
          term = term->getNextTerm().get();
          termIndex++;
        }
        return ret;
      } else {
        // Returning to a single branch of the term.
        // Check if this is a delayed state
        if (this->tempState.getIndexStack()->at(currentLevel) == -1) {
          // This is a delayed state, so just return to let the parent branch continue
          ASSERT(static_cast<Int>(this->tempState.getIndexStack()->size()) == currentLevel+1);
          this->tempState.getIndexStack()->pop_back();
          return CONTINUE_SAME_CHAR;
        } else {
          // Get up to the next term object.
          Term *term = alternativeTerm->getChildTermsHead().get();
          termIndex = this->tempState.getIndexStack()->at(currentLevel);
          for (Int i = 0; i < termIndex; i++) {
            ASSERT(term->getNextTerm() != 0);
            term = term->getNextTerm().get();
          }
          // Call the inner branch.
          NextAction ret = this->processTempState(inputChar, term, currentLevel+1);
          if (ret == CONTINUE_NEW_CHAR || ret == CONTINUE_SAME_CHAR) {
            this->tempState.getIndexStack()->pop_back();
          }
          return ret;
        }
      }
      break;
    }

    case TermType::CONCAT: {
      Int termIndex;
      ConcatTerm *concatTerm = static_cast<ConcatTerm*>(currentTerm);
      if (concatTerm->getChildTermCount() == 0) {
        throw GeneralException(STR("Concat term's child terms aren't set yet."),
                               STR("Core::Lexer::StateMachine::processTempState"));
      }
      // get the index within the term
      if (currentLevel == static_cast<Int>(this->tempState.getIndexStack()->size())) {
        termIndex = 0;
        this->tempState.getIndexStack()->push_back(termIndex);
      } else {
        termIndex = this->tempState.getIndexStack()->at(currentLevel);
      }
      // get up to the next term object
      Term *term = concatTerm->getChildTermsHead().get();
      for (Int i = 0; i < termIndex; i++) {
        ASSERT(term->getNextTerm() != 0);
        term = term->getNextTerm().get();
      }
      // loop on calling the next level
      while (true) {
        switch (this->processTempState(inputChar, term, currentLevel+1)) {
          case CONTINUE_NEW_CHAR:
            // move to the next term and stop
            if (term->getNextTerm() == 0) {
              // we finished all the terms on this level, so return
              // remove termIndex from the stack
              this->tempState.getIndexStack()->pop_back();
              return CONTINUE_NEW_CHAR;
            } else {
              // update the term index on the stack
              termIndex++;
              this->tempState.getIndexStack()->at(currentLevel) = termIndex;
              // save the state on the stack
              this->createState();
              return STOP;
            }
          case CONTINUE_SAME_CHAR:
            // move to the next term immediately
            term = term->getNextTerm().get();
            if (term == 0) {
              // we finished all the terms on this level, so return
              // remove termIndex from the stack
              this->tempState.getIndexStack()->pop_back();
              return CONTINUE_SAME_CHAR;
            } else {
              // update the term index on the stack
              termIndex++;
              this->tempState.getIndexStack()->at(currentLevel) = termIndex;
            }
            break;
          case STOP:
            // stop processing
            return STOP;
        }
      }
      break;
    }

    case TermType::REFERENCE: {
      // Pass the call to the referenced term.
      ReferenceTerm *referenceTerm = static_cast<ReferenceTerm*>(currentTerm);
      if (referenceTerm->getReferenceHead() == 0) {
        // Search for and set the reference head.
        TokenDefinition *td = this->tokenDefinitions->find(referenceTerm->getReferenceId()).get();
        // Check for null td.
        if (td == 0) {
          Str excMsg = STR("Invalid token definition id (");
          excMsg += Data::IdGenerator::getSingleton()->getDesc(referenceTerm->getReferenceId());
          excMsg += STR(")");
          throw GeneralException(excMsg.c_str(), STR("Core::Lexer::StateMachine::processTempState"));
        }
        if (td->getFormulaHead() == 0) {
          Str excMsg = STR("Invalid token definition (");
          excMsg += Data::IdGenerator::getSingleton()->getDesc(td->getId());
          excMsg += STR("). The definition's formula isn't set yet.");
          throw GeneralException(excMsg.c_str(), STR("Core::Lexer::StateMachine::processTempState"));
        }
        // Set the reference term.
        referenceTerm->setReferenceHead(td->getFormulaHead().get());
      }
      return this->processTempState(inputChar, referenceTerm->getReferenceHead(),
                                      currentLevel);
    }

    default:
      // This should not be reachable.
      ASSERT(false);
      return STOP;
  }
}


/**
 * Select the best token among the tokens that are currently represented in the
 * states stack. The selection is based on the following criteria:<br>
 * -Always select the longer token.<br>
 * -If two tokens have the same length and one of them is a constant string,
 *  select the constant string token.
 * -If two tokens have the same length and both or none of them is a constant
 *  string, select the one that comes first in the token definitions array.
 *
 * @return Returns the index of the state of the selected token.
 */
Int StateMachine::selectBestToken()
{
  Int index = -1;
  for (Int i = 0; i < static_cast<Int>(this->states.size()); ++i) {
    if (this->states[i].getTokenLength() == 0) continue;
    if (index == -1) {
      index = i;
    } else {
      // get token lengths
      Int lengthI = this->states[i].getTokenLength();
      Int lengthIndex = this->states[index].getTokenLength();
      // check for a longer token
      if (lengthI > lengthIndex) {
        // choose the longer token
        index = i;
      } else if (lengthI == lengthIndex) {
        // get token definition index
        Int tokenDefinitionI = this->states[i].getIndexStack()->at(0);
        Int tokenDefinitionIndex = this->states[index].getIndexStack()->at(0);
        // check which state is for a constant token
        Bool isConstantI, isConstantIndex;
        Term *head;
        head = this->tokenDefinitions->get(tokenDefinitionI)->getFormulaHead().get();
        if (head->getType() == TermType::CONST) isConstantI = true;
        else isConstantI = false;
        head = this->tokenDefinitions->get(tokenDefinitionIndex)->getFormulaHead().get();
        if (head->getType() == TermType::CONST) isConstantIndex = true;
        else isConstantIndex = false;
        // check if one token is constant and the other is not
        if (isConstantI && !isConstantIndex) {
          // choose the constant token
          index = i;
        } else if (isConstantI == isConstantIndex) {
          if (tokenDefinitionI < tokenDefinitionIndex) {
            // choose the higher token definition
            index = i;
          }
        }
      }
    }
  }

  return index;
}


/**
 * Recursively matches the given character to the given character group. This
 * recursive function will descend into the entire character group tree to
 * match the given character.
 *
 * @param ch The character to match.
 * @param unit A pointer to the character group unit object to match. This
 *             object can be a the head of a tree of CharGroupUnit objects.
 * @return Returns true if the character matches, false otherwise.
 */
Bool StateMachine::matchCharGroup(Char ch, CharGroupUnit *unit)
{
  ASSERT(unit);

  switch (unit->getType().val) {
    case CharGroupUnitType::SEQUENCE: {
      SequenceCharGroupUnit *u = static_cast<SequenceCharGroupUnit*>(unit);
      if (u->getStartCode() == 0 && u->getEndCode() == 0) {
        throw GeneralException(STR("Sequence char group unit is not configured yet."),
                               STR("Core::Lexer::StateMachine::matchCharGroup"));
      }
      if (ch >= u->getStartCode() && ch <= u->getEndCode()) return true;
      else return false;
    }

    case CharGroupUnitType::RANDOM: {
      RandomCharGroupUnit *u = static_cast<RandomCharGroupUnit*>(unit);
      if (u->getCharList() == 0) {
        throw GeneralException(STR("Random char group unit is not configured yet."),
                               STR("Core::Lexer::StateMachine::matchCharGroup"));
      }
      for (Int i = 0; i < u->getCharListSize(); i++) {
        if (u->getCharList()[i] == ch) return true;
      }
      return false;
    }

    case CharGroupUnitType::UNION: {
      UnionCharGroupUnit *u = static_cast<UnionCharGroupUnit*>(unit);
      if (u->getCharGroupUnits()->size() == 0) {
        throw GeneralException(STR("Union char group unit is not configured yet."),
                               STR("Core::Lexer::StateMachine::matchCharGroup"));
      }
      for (Int i = 0; i < static_cast<Int>(u->getCharGroupUnits()->size()); i++) {
        if (StateMachine::matchCharGroup(ch, u->getCharGroupUnits()->at(i).get()) == true) {
          return true;
        }
      }
      return false;
    }

    case CharGroupUnitType::INVERT: {
      InvertCharGroupUnit *u = static_cast<InvertCharGroupUnit*>(unit);
      if (u->getChildCharGroupUnit() == 0) {
        throw GeneralException(STR("Invert char group unit is not configured yet."),
                               STR("Core::Lexer::StateMachine::matchCharGroup"));
      }
      if (StateMachine::matchCharGroup(ch, u->getChildCharGroupUnit().get()) == true) {
        return false;
      } else {
        return true;
      }
    }

    default:
      // This should not be reachable.
      ASSERT(false);
  }
  return false; // just to prevent warnings
}


/**
 * Clear the states stack and all other buffers to the state of the machine
 * before parsing started. Token and char group definitions will not be
 * affected.
 */
void StateMachine::clear()
{
  this->states.clear();
  this->inputBuffer.clear();
  this->errorBuffer.clear();

  this->tempState.getIndexStack()->clear();
  this->tempState.setTokenLength(0);

  this->disabledStateIndex = -1;
  this->currentProcessingIndex = 0;
  this->currentTokenClamped = false;
  this->lastToken.setId(UNKNOWN_ID);
}


/**
 * Creates a state machine instance on the states stack. The function also puts
 * into consideration the disabled state, if any. If there is a disabled state,
 * the function will simply use it, otherwise it will add a new instance of
 * State on the stack. The new object will automatically contain a copy of the
 * values in the temp state object (tempState).
 *
 * @return Int Returns the index of the new state on the stack.
 */
Int StateMachine::createState()
{
  Int r;
  if (this->disabledStateIndex != -1) {
    r = this->disabledStateIndex;
    this->disabledStateIndex = -1;
    this->states[r] = this->tempState;
    return r;
  } else {
    r = this->states.size();
    // Limit the size of the array to a maximum value.
    if (r >= STATES_ARRAY_MAX_SIZE) {
      throw GeneralException(STR("States array maximum size exceeded."),
                             STR("Core::Lexer::StateMachine::createState"));
    }
    // Create the new state.
    this->states.push_back(this->tempState);
    return r;
  }
}


/**
 * Copy the state with the given index from the stack into the temp state then
 * disabled it.
 *
 * @param index The index of the state to extract.
 */
void StateMachine::extractState(Int index)
{
  // make sure no state is currently extracted
  ASSERT(this->disabledStateIndex == -1);

  this->tempState = this->states[index];
  this->disabledStateIndex = index;
}


/**
 * If a state is disabled, move the last state in the states list into the
 * location of the disabled state.
 */
void StateMachine::defragStatesList()
{
  if (this->disabledStateIndex != -1 && this->states.size() != 0) {
    if (this->disabledStateIndex == static_cast<Int>(this->states.size())-1) {
      // the last state in the stack is the disabled one, so just reduce the stack size
      this->states.pop_back();
    } else {
      // move the last state to the location of the disabled state
      this->states[this->disabledStateIndex] = this->states[this->states.size()-1];
      this->states.pop_back();
    }
    this->disabledStateIndex = -1;
  }
}


/**
 * Loops through all the states to check whether any of them use the token with
 * the given id.
 */
Bool StateMachine::isTokenInUse(Word tokenId)
{
  if (tokenId == UNKNOWN_ID) {
    throw InvalidArgumentException(STR("tokenId"), STR("Core::Lexer::StateMachine::is_production_in_use"),
                                   STR("tokenId is UNKNOWN_ID"));
  }
  if (this->tokenDefinitions == 0) return false;
  Int index = this->tokenDefinitions->findIndex(tokenId);
  for (Int i = 0; i < static_cast<Int>(this->states.size()); ++i) {
    if (this->states[i].getIndexStack()->at(0) == index) return true;
  }
  return false;
}

} } // namespace

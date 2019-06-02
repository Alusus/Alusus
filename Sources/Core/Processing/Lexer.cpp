/**
 * @file Core/Processing/Lexer.cpp
 * Contains the implementation of Processing::Lexer.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing
{

//==============================================================================
// Member Functions

void Lexer::initialize(SharedPtr<Data::Ast::Scope> rootScope)
{
  // Before we can change the token list, we need to make sure we have no outstanding
  // states.
  if (!this->states.empty()) {
    throw EXCEPTION(GenericException, S("Token definitions list can't be changed while being in use."));
  }

  // TODO: If we currently have a grammar, we need to unset it first.
  //if (this->production_definitions != 0) {
  //    this->production_definitions->production_in_use_inquirer.unconnect(this, &Parser::is_production_in_use);
  //}

  // Set the new repository.
  this->grammarRoot = getSharedPtr(Data::Grammar::getGrammarRoot(rootScope.get()));
  if (this->grammarRoot == 0) {
    throw EXCEPTION(InvalidArgumentException, S("rootScope"), S("Root scope does not contain a grammar."));
  }

  // Prepare the context.
  this->grammarContext.setRoot(this->grammarRoot.get());
  Data::Grammar::Module *lexerModule = this->grammarContext.getAssociatedLexerModule();
  if (lexerModule == 0) {
    throw EXCEPTION(GenericException, S("Couldn't find a lexer module in the given grammar repository."));
  }
  this->grammarContext.setModule(lexerModule);

  // TODO: If we have a new grammar, we need to set the production_in_use_inquirer signal.
  //if (this->production_definitions != 0) {
  //    this->production_definitions->production_in_use_inquirer.connect(this, &Parser::is_production_in_use);
  //}
}


/**
 * Add a single character to the input buffer and keep processing until no more
 * characters are in the input buffer.
 *
 * @param inputChar The character to add to the input buffer.
 * @param sourceLocaiton The source location at which the character appeared.
 *                       This will be updated with the location immediately
 *                       following this character.
 */
void Lexer::handleNewChar(Char inputChar, Data::SourceLocationRecord &sourceLocation)
{
  // Buffer the input sequence until it can be converted to wide characters.
  this->tempByteCharBuffer[this->tempByteCharCount] = inputChar;
  ++this->tempByteCharCount;
  WChar wideCharBuffer[1];
  // Try converting to wide characters.
  Int processedIn, processedOut;
  convertStr(this->tempByteCharBuffer, this->tempByteCharCount, wideCharBuffer, 1, processedIn, processedOut);
  if (processedOut != 0) {
    // Conversion was successful. Send converted character to the buffer.
    this->pushChar(wideCharBuffer[0], sourceLocation);
    this->processBuffer();
    computeNextCharPosition(wideCharBuffer[0], sourceLocation.line, sourceLocation.column);
    this->tempByteCharCount = 0;
  } else if (this->tempByteCharCount == 4) {
    throw EXCEPTION(GenericException,
                    S("Invalid input character sequence. Sequence could not be converted to wide characters."));
  }
}


/**
 * Add a string to the input buffer and keep processing until no more characters
 * are in the input buffer.
 *
 * @param inputStr The string to add to the input buffer.
 * @param sourceLocation The source location of the first character in the
 *                       string. This will be updated with the new location.
 */
void Lexer::handleNewString(Char const *inputStr, Data::SourceLocationRecord &sourceLocation)
{
  for (Int i = 0; i < static_cast<Int>(strlen(inputStr)); i++) {
    this->handleNewChar(inputStr[i], sourceLocation);
  }
}


/**
 * Keep processing the input buffer until it has no more input characters.
 */
void Lexer::processBuffer()
{
  while (true) {
    Int r = this->process();

    if (r & 1) {
      LOG(LogLevel::LEXER_MAJOR, S("Emitting token. ID: ") << ID_GENERATOR->getDesc(this->getLastToken()->getId())
          << S(", Text: ") << this->getLastToken()->getText());
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
 * @param sl The source location of the given character.
 * @return Returns true if the character was inserted, false otherwise.
 */
Bool Lexer::pushChar(WChar ch, Data::SourceLocationRecord const &sl)
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
        this->inputBuffer.push(ch, sl, true);
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
    this->inputBuffer.push(ch, sl, false);
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
Int Lexer::process()
{
  // Make sure the token definition list is defined.
  if (this->grammarContext.getModule() == 0) {
    throw EXCEPTION(GenericException, S("Lexer module is not set."));
  }

  // Make sure there are characters in the input buffer.
  // If there isn't, inform the caller to add new characters.
  if (this->currentProcessingIndex >= this->inputBuffer.getCharCount()) return 2;

  // Get the processing character.
  WChar inputChar = this->inputBuffer.getChars()[this->currentProcessingIndex];

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
      this->noticeSignal.emit(std::make_shared<Notices::BufferFullNotice>(
        std::make_shared<Data::SourceLocationRecord>(this->inputBuffer.getSourceLocation())
      ));
      // Choose one of the closed states.
      Int i = this->selectBestToken();
      Data::Grammar::SymbolDefinition *def = this->getSymbolDefinition(this->states[i].getTokenDefIndex());
      // Check if the chosen token is not an ignored token.
      TiInt *flags = this->grammarContext.getSymbolFlags(def);
      if (!((flags == 0 ? 0 : flags->get()) & Data::Grammar::SymbolFlags::IGNORED_TOKEN)) {
        // Has the token been clamped?
        if (this->currentTokenClamped) {
          // Raise a warning.
          this->noticeSignal.emit(std::make_shared<Notices::TokenClampedNotice>(
            std::make_shared<Data::SourceLocationRecord>(this->inputBuffer.getSourceLocation())
          ));
          this->currentTokenClamped = false;
        }
        // Set token properties.
        TokenizingHandler *handler = ti_cast<TokenizingHandler>(def->getBuildHandler().get());
        if (handler == 0) {
          this->lastToken.setId(def->getId());
          this->lastToken.setText(this->inputBuffer.getChars(), this->states[i].getTokenLength());
          this->lastToken.setSourceLocation(this->inputBuffer.getSourceLocation());
        } else {
          handler->prepareToken(&this->lastToken, def->getId(), this->inputBuffer.getChars(),
                                this->states[i].getTokenLength(), this->inputBuffer.getSourceLocation());
        }
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
    } else if (closedStateCount > 0) {
      Int bestToken = this->selectBestToken();
      if (closedStateCount > 1) {
        // Keep only the best closed state to conserve memory.
        // Select a token among the closed states.

        // Remove all closed tokens except for the selected one.
        for (Int i = 0; i < static_cast<Int>(this->states.size()); i++) {
          if (this->states[i].getTokenLength() != 0 && i != bestToken) {
            // Remove this unselected closed token.
            this->disabledStateIndex = i;
            this->defragStatesList();
            // Check if the moved state was actually the selected one.
            if (bestToken >= static_cast<Int>(this->states.size())) bestToken = i;
            --i;
          }
        }
      }
      // Remove any open states belonging to a PREFERE_SHORTER rule that happens to be the same rule as the
      // chosen closed state.
      auto bestTokenDefIndex = this->states[bestToken].getTokenDefIndex();
      auto def = this->getSymbolDefinition(bestTokenDefIndex);
      TiInt *flags = this->grammarContext.getSymbolFlags(def);
      if (flags != 0 && flags->get() & Data::Grammar::SymbolFlags::PREFER_SHORTER) {
        for (Int i = 0; i < static_cast<Int>(this->states.size()); i++) {
          if (i == bestToken) continue;
          if (this->states[i].getTokenDefIndex() == bestTokenDefIndex) {
            this->disabledStateIndex = i;
            this->defragStatesList();
            // Check if the moved state was actually the selected one.
            if (bestToken >= static_cast<Int>(this->states.size())) bestToken = i;
            else --i;
          }
        }
      }
    }
  } else if (closedStateCount > 0) {
    // Choose one of the closed states.
    Int i = this->selectBestToken();
    Data::Grammar::SymbolDefinition *def = this->getSymbolDefinition(this->states[i].getTokenDefIndex());
    // Check if the chosen token is not an ignored token.
    TiInt *flags = this->grammarContext.getSymbolFlags(def);
    if (!((flags == 0 ? 0 : flags->get()) & Data::Grammar::SymbolFlags::IGNORED_TOKEN)) {
      // Has the token been clamped?
      if (this->currentTokenClamped) {
        // Raise a warning.
        this->noticeSignal.emit(std::make_shared<Notices::TokenClampedNotice>(
          std::make_shared<Data::SourceLocationRecord>(this->inputBuffer.getSourceLocation())
        ));
        this->currentTokenClamped = false;
      }
      // Set token properties.
      TokenizingHandler *handler = ti_cast<TokenizingHandler>(def->getBuildHandler().get());
      if (handler == 0) {
        this->lastToken.setId(def->getId());
        this->lastToken.setText(this->inputBuffer.getChars(), this->states[i].getTokenLength());
        this->lastToken.setSourceLocation(this->inputBuffer.getSourceLocation());
      } else {
        handler->prepareToken(&this->lastToken, def->getId(), this->inputBuffer.getChars(),
                              this->states[i].getTokenLength(), this->inputBuffer.getSourceLocation());
      }
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
    Data::SourceLocationRecord sl;
    if (this->inputBuffer.getChars()[0] != FILE_TERMINATOR) {
      err.assign(this->inputBuffer.getChars(), 1);
      sl = this->inputBuffer.getSourceLocation();
      this->inputBuffer.remove(1);
    }
    // limit the error text to LEXER_ERROR_BUFFER_MAX_CHARACTERS
    if (this->errorBuffer.getTextLength() < LEXER_ERROR_BUFFER_MAX_CHARACTERS) {
      this->errorBuffer.appendText(err.c_str(), sl);
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
      this->noticeSignal.emit(std::make_shared<Notices::UnrecognizedCharNotice>(
        this->errorBuffer.getText().c_str(),
        this->errorBuffer.getSourceLocation()
      ));
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
void Lexer::processStartChar(WChar inputChar)
{
  // There must not be any state currently in the stack.
  ASSERT(this->states.size() == 0);

  LOG(LogLevel::LEXER_MID, S("Starting a new token. New char: '") << inputChar << S("'"));

  for (Word i = 0; i < this->grammarContext.getModule()->getCount(); i++) {
    // Skip non tokens and non-root tokens.
    TiObject *obj = this->grammarContext.getModule()->getElement(i);
    if (obj == 0 || !obj->isA<Data::Grammar::SymbolDefinition>()) continue;
    Data::Grammar::SymbolDefinition *def = static_cast<Data::Grammar::SymbolDefinition*>(obj);
    TiInt *flags = this->grammarContext.getSymbolFlags(def);
    if (!((flags == 0 ? 0 : flags->get()) & Data::Grammar::SymbolFlags::ROOT_TOKEN)) continue;
    // Validation.
    if (def->getTerm() == 0) {
      Str excMsg = S("Invalid token definition (");
      excMsg += ID_GENERATOR->getDesc(def->getId());
      excMsg += S("). The definition formula is not set yet.");
      throw EXCEPTION(GenericException, excMsg.c_str());
    }
    if (!(def->getTerm()->isDerivedFrom<Data::Grammar::Term>())) {
      Str excMsg = S("Data driven token definitions are not supported by the lexer yet. Token def: ");
      excMsg += ID_GENERATOR->getDesc(def->getId());
      throw EXCEPTION(GenericException, excMsg.c_str());
    }
    // Set the first entry in the state index stack to the token definition index.
    this->tempState.setTokenDefIndex(i);
    this->tempState.resizeLevels(1);
    this->tempState.refLevel(0).posId = 0;
    this->tempState.refLevel(0).term = def->getTerm().s_cast_get<Data::Grammar::Term>();
    // Reset the token length.
    this->tempState.setTokenLength(0);
    // Process the token.
    switch (this->processState(&this->tempState, inputChar, 0)) {
      case CONTINUE_NEW_CHAR:
        // The character was accepted and a full token is formed.
        this->tempState.setTokenLength(1);
        this->duplicateState(&this->tempState);
        break;
      case CONTINUE_SAME_CHAR:
        // This should never be reached.
        ASSERT(false);
        break;
    }
  }

  #ifdef USE_LOGS
    for (Int i = 0; i < static_cast<Int>(this->states.size()); ++i) {
      Int index = this->states[i].getTokenDefIndex();
      Int id = this->getSymbolDefinition(index)->getId();
      LOG(LogLevel::LEXER_MID, S("Starting Token: ") << ID_GENERATOR->getDesc(id));
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
void Lexer::processNextChar(WChar inputChar)
{
  // There must be some states currently in the stack.
  ASSERT(this->states.size() != 0);

  LOG(LogLevel::LEXER_MID, S("Processing new character: '") << inputChar << S("'"));

  // Make sure not to process newly created states by limiting the loop to the
  // current states count.
  Int statesCount = this->states.size();
  for (Int i = 0; i < statesCount; i++) {
    if (this->states[i].getTokenLength() > 0) continue;
    // Extract the state into the temp state.
    this->extractState(i);
    // Process the token.
    switch (this->processState(&this->tempState, inputChar, 0)) {
      case CONTINUE_NEW_CHAR:
        // The character was accepted and a full token is formed.
        this->tempState.setTokenLength(this->currentProcessingIndex+1);
        this->duplicateState(&this->tempState);
        break;
      case CONTINUE_SAME_CHAR:
        // The character was not accepted, but a full token is formed.
        this->tempState.setTokenLength(this->currentProcessingIndex);
        this->duplicateState(&this->tempState);
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
      Int index = this->states[i].getTokenDefIndex();
      Int id = this->getSymbolDefinition(index)->getId();
      LOG(LogLevel::LEXER_MID, S("Keeping Token: ") << ID_GENERATOR->getDesc(id));
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
 * @param currentLevel The index of the entry within the state's internal
 *                        stack that is associated with the current term object.
 * @return Returns a NextAction value telling the caller what to do next.
 */
Lexer::NextAction Lexer::processState(LexerState *state, WChar inputChar, Int currentLevel)
{
  ASSERT(state != 0);
  // make sure the current level isn't out of range
  ASSERT(currentLevel <= static_cast<Int>(state->getLevelCount()));

  // check the term type
  auto currentTerm = state->refLevel(currentLevel).term;
  if (currentTerm->isA<Data::Grammar::ConstTerm>()) {
    return this->processConstTerm(state, inputChar, currentLevel);
  } else if (currentTerm->isA<Data::Grammar::CharGroupTerm>()) {
    return this->processCharGroupTerm(state, inputChar, currentLevel);
  } else if (currentTerm->isA<Data::Grammar::MultiplyTerm>()) {
    return this->processMultiplyTerm(state, inputChar, currentLevel);
  } else if (currentTerm->isA<Data::Grammar::AlternateTerm>()) {
    return this->processAlternateTerm(state, inputChar, currentLevel);
  } else if (currentTerm->isA<Data::Grammar::ConcatTerm>()) {
    return this->processConcatTerm(state, inputChar, currentLevel);
  } else if (currentTerm->isA<Data::Grammar::ReferenceTerm>()) {
    return this->processReferenceTerm(state, inputChar, currentLevel);
  } else {
    Str excMsg = S("Invalid token type found. Token definition: ");
    excMsg += ID_GENERATOR->getDesc(
      this->getSymbolDefinition(state->getTokenDefIndex())->getId()
    );
    excMsg += S(". Object type: ");
    excMsg += currentTerm->getMyTypeInfo()->getTypeName();
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
}


Lexer::NextAction Lexer::processConstTerm(LexerState *state, WChar inputChar, Int currentLevel)
{
  auto currentTerm = state->refLevel(currentLevel).term;
  ASSERT(currentTerm->isA<Data::Grammar::ConstTerm>());
  Int charIndex;
  Data::Grammar::ConstTerm *constTerm = static_cast<Data::Grammar::ConstTerm*>(currentTerm);
  if (constTerm->getMatchString().size() == 0) {
    throw EXCEPTION(GenericException, S("Const term match string is not set yet."));
  }
  // this type of terms is always the deepest level.
  ASSERT(currentLevel == static_cast<Int>(state->getLevelCount())-1);
  // get the index within the term.
  charIndex = state->refLevel(currentLevel).posId;
  // assert that charIndex is not out of the range of the constant.
  ASSERT(charIndex < static_cast<Int>(constTerm->getMatchString().size()));
  // check if the next character is accepted.
  if (constTerm->getMatchString()[charIndex] == inputChar) {
    charIndex++;
    // Check if we reached the end of the string.
    if (charIndex == static_cast<Int>(constTerm->getMatchString().size())) {
      // Remove level from the stack
      state->popTermLevel();
      return CONTINUE_NEW_CHAR; // No more chars is expected on this term object.
    } else {
      // Update charIndex on the stack.
      state->refLevel(currentLevel).posId = charIndex;
      // Push the temp state onto the states stack.
      this->duplicateState(state);
      return STOP; // More chars is expected on this term object.
    }
  } else {
    return STOP; // This term object failed to match.
  }
}


Lexer::NextAction Lexer::processCharGroupTerm(LexerState *state, WChar inputChar, Int currentLevel)
{
  auto currentTerm = state->refLevel(currentLevel).term;
  ASSERT(currentTerm->isA<Data::Grammar::CharGroupTerm>());
  Data::Grammar::CharGroupTerm *charGroupTerm = static_cast<Data::Grammar::CharGroupTerm*>(currentTerm);
  Data::Grammar::CharGroupDefinition *def;
  Data::Grammar::Reference *ref = charGroupTerm->getCharGroupReference().get();
  if (ref == 0) {
    Str excMsg = S("Reference is null for CharGroupTerm at token definition: ");
    excMsg += ID_GENERATOR->getDesc(
      this->getSymbolDefinition(state->getTokenDefIndex())->getId()
    );
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
  this->grammarContext.getReferencedCharGroup(ref, def);
  if (def->getCharGroupUnit() == 0) {
    Str excMsg = S("Invalid character group (");
    excMsg += ID_GENERATOR->getDesc(def->getId());
    excMsg += S("). The definition formula is not set yet.");
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
  if (Data::Grammar::matchCharGroup(inputChar, def->getCharGroupUnit().get())) {
    state->popTermLevel();
    return CONTINUE_NEW_CHAR;
  } else {
    return STOP;
  }
}


Lexer::NextAction Lexer::processMultiplyTerm(LexerState *state, WChar inputChar, Int currentLevel)
{
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

  auto currentTerm = state->refLevel(currentLevel).term;
  ASSERT(currentTerm->isA<Data::Grammar::MultiplyTerm>());
  Data::Grammar::MultiplyTerm *multiplyTerm = static_cast<Data::Grammar::MultiplyTerm*>(currentTerm);
  if (multiplyTerm->getTerm().ti_cast_get<Data::Grammar::Term>() == 0) {
    Str excMsg = S("Multiply term with null or invalid child is found at definition: ");
    excMsg += ID_GENERATOR->getDesc(
      this->getSymbolDefinition(state->getTokenDefIndex())->getId()
    );
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
  // Get the index within the term.
  Int iterationIndex = state->refLevel(currentLevel).posId;
  // Are we standing at the multiply term itself?
  if (static_cast<Int>(state->getLevelCount()) == currentLevel+1) {
    // We are standing at the multiply term itself.
    Data::Grammar::Term *term = multiplyTerm->getTerm().s_cast_get<Data::Grammar::Term>();
    ASSERT(term != 0);
    if (
      multiplyTerm->getMax() == 0 ||
      this->grammarContext.getMultiplyTermMax(multiplyTerm)->get() > iterationIndex
    ) {
      // Try the inner route.
      state->pushTermLevel(0, term);
      NextAction ret = this->processState(state, inputChar, currentLevel+1);
      if (ret == CONTINUE_NEW_CHAR) {
        state->refLevel(currentLevel).posId = iterationIndex+1;
        this->duplicateState(state);
      }
    }
    // Check if we have already passed the required minimum number of occurances.
    if (
      multiplyTerm->getMin() != 0 && iterationIndex < this->grammarContext.getMultiplyTermMin(multiplyTerm)->get()
    ) return STOP;
    else {
      state->resizeLevels(currentLevel);
      return CONTINUE_SAME_CHAR;
    }
  } else {
    // We are in the middle of the multiply term.
    // Call the inner branch.
    NextAction ret = this->processState(state, inputChar, currentLevel+1);
    if (ret == CONTINUE_SAME_CHAR) {
      Int minOccurances = multiplyTerm->getMin() == 0 ?
                            0 : this->grammarContext.getMultiplyTermMin(multiplyTerm)->get();
      Int maxOccurances = multiplyTerm->getMax() == 0 ?
                            0 : this->grammarContext.getMultiplyTermMax(multiplyTerm)->get();
      // We can continue using the same character. We'll try the multiply branch again using the
      // same character before moving upward and trying the outer branch with the same character.
      if (maxOccurances == 0 || maxOccurances > iterationIndex+1) {
        // The max occurances hasn't been reached, so we can try the multiply branch again.
        state->refLevel(currentLevel).posId = iterationIndex+1;
        auto *term = multiplyTerm->getTerm().s_cast_get<Data::Grammar::Term>();
        state->pushTermLevel(0, term);
        ret = this->processState(state, inputChar, currentLevel+1);
        if (ret == CONTINUE_NEW_CHAR) {
          // The character was consumed in the multiply branch which is now complete, so now
          // we'll setup to try the next character on both the multiply branch again and the
          // outer branch.
          state->refLevel(currentLevel).posId = iterationIndex+2;
          this->duplicateState(state);
        }
        // NOTE: We don't need to check again for the CONTINUE_SAME_CHAR return value
        //       since that's an illogical infinite loop, so we'll treat that as fail.
        // Trying the multiply branch with the same character again either was not successful
        // or is successful but is still pending inside the branch. In both cases we don't
        // need to do anything at this point.
      }
      if (iterationIndex >= minOccurances) {
        // Now we need to try the outer branch with the same character.
        state->resizeLevels(currentLevel);
        return CONTINUE_SAME_CHAR;
      } else {
        // The minimum occurances isn't met so we won't try the outer branch.
        return STOP;
      }
    } else if (ret == CONTINUE_NEW_CHAR) {
      // Branch was completed successfully and the character was consumed, so we'll setup to
      // try the next character with both the inner and outer branches.
      state->refLevel(currentLevel).posId = iterationIndex+1;
      this->duplicateState(state);
      return STOP;
    } else {
      // The multiply term either isn't complete yet or wasn't successful.
      return STOP;
    }
  }
}


Lexer::NextAction Lexer::processAlternateTerm(LexerState *state, WChar inputChar, Int currentLevel)
{
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

  auto currentTerm = state->refLevel(currentLevel).term;
  ASSERT(currentTerm->isA<Data::Grammar::AlternateTerm>());
  Data::Grammar::AlternateTerm *alternateTerm = static_cast<Data::Grammar::AlternateTerm*>(currentTerm);
  if (alternateTerm->isDynamic()) {
    Str excMsg = S("Data driven token definitions are not supported by the lexer yet. Token def: ");
    excMsg += ID_GENERATOR->getDesc(
      this->getSymbolDefinition(state->getTokenDefIndex())->getId()
    );
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
  auto alternateList = alternateTerm->getTerms().s_cast_get<Data::Grammar::List>();
  if (alternateList->getCount() < 2) {
    Str excMsg = S("Alternative term doesn't have enough branches yet (less than two). Token def: ");
    excMsg += ID_GENERATOR->getDesc(
      this->getSymbolDefinition(state->getTokenDefIndex())->getId()
    );
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
  // Get the index within the term.
  if (currentLevel == static_cast<Int>(state->getLevelCount()) - 1) {
    // We are standing at the alternate term, which could mean we are entering the alternate term now
    // or we are already done with it.
    if (state->refLevel(currentLevel).posId != -1) {
      // Just entering the term for the first time.
      // Loop through all the alternatives of the term.
      NextAction ret = UNKNOWN_ACTION;
      Bool delayedStateCreated = false;
      for (Int i = 0; i < alternateList->getCount(); ++i) {
        state->refLevel(currentLevel).posId = i;
        Data::Grammar::Term *term = ti_cast<Data::Grammar::Term>(alternateList->getElement(i));
        if (term == 0) {
          Str excMsg = S("Null term found in an alternate branch. Token def: ");
          excMsg += ID_GENERATOR->getDesc(
            this->getSymbolDefinition(state->getTokenDefIndex())->getId()
          );
          throw EXCEPTION(GenericException, excMsg.c_str());
        }
        // add the current index
        state->resizeLevels(currentLevel+1);
        state->pushTermLevel(0, term);
        switch (this->processState(state, inputChar, currentLevel+1)) {
          case CONTINUE_NEW_CHAR:
            if (ret == CONTINUE_SAME_CHAR) {
              if (!delayedStateCreated) {
                // create delayed state
                state->refLevel(currentLevel).posId = -1;
                this->duplicateState(state);
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
                state->refLevel(currentLevel).posId = -1;
                this->duplicateState(state);
                delayedStateCreated = true;
              }
            }
            ret = CONTINUE_SAME_CHAR;
            break;
          case STOP:
            if (ret == UNKNOWN_ACTION) ret = STOP;
            break;
        }
      }
      // remove the added index
      if (ret != STOP) state->resizeLevels(currentLevel);
      return ret;
    } else {
      // This is a delayed state, so just return to let the parent branch continue
      state->popTermLevel();
      return CONTINUE_SAME_CHAR;
    }
  } else {
    // Returning to a single branch of the term.
    // Get up to the next term object.
    NextAction ret = this->processState(state, inputChar, currentLevel+1);
    if (ret == CONTINUE_NEW_CHAR || ret == CONTINUE_SAME_CHAR) {
      state->popTermLevel();
    }
    return ret;
  }
}


Lexer::NextAction Lexer::processConcatTerm(LexerState *state, WChar inputChar, Int currentLevel)
{
  auto currentTerm = state->refLevel(currentLevel).term;
  ASSERT(currentTerm->isA<Data::Grammar::ConcatTerm>());
  Data::Grammar::ConcatTerm *concatTerm = static_cast<Data::Grammar::ConcatTerm*>(currentTerm);
  if (concatTerm->isDynamic()) {
    Str excMsg = S("Data driven token definitions are not supported by the lexer yet. Token def: ");
    excMsg += ID_GENERATOR->getDesc(
      this->getSymbolDefinition(state->getTokenDefIndex())->getId()
    );
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
  auto concatList = concatTerm->getTerms().s_cast_get<Data::Grammar::List>();
  if (concatList->getCount() == 0) {
    Str excMsg = S("Concat term's child terms aren't set yet. Token def: ");
    excMsg += ID_GENERATOR->getDesc(
      this->getSymbolDefinition(state->getTokenDefIndex())->getId()
    );
    throw EXCEPTION(GenericException, excMsg.c_str());
  }
  // get the index within the term
  Int termIndex = state->refLevel(currentLevel).posId;
  // If we are just entering the concat state then push the inner level.
  if (currentLevel == state->getLevelCount() - 1) {
    Data::Grammar::Term *term = ti_cast<Data::Grammar::Term>(concatList->getElement(termIndex));
    if (term == 0) {
      Str excMsg = S("Concat term's child term is null. Token def: ");
      excMsg += ID_GENERATOR->getDesc(
        this->getSymbolDefinition(state->getTokenDefIndex())->getId()
      );
      throw EXCEPTION(GenericException, excMsg.c_str());
    }
    state->pushTermLevel(0, term);
  }
  // loop on calling the next level
  while (true) {
    switch (this->processState(state, inputChar, currentLevel+1)) {
      case CONTINUE_NEW_CHAR:
        // move to the next term and stop
        if (termIndex >= concatList->getCount()-1) {
          // we finished all the terms on this level, so return
          // remove termIndex from the stack
          state->popTermLevel();
          return CONTINUE_NEW_CHAR;
        } else {
          // update the term index on the stack
          termIndex++;
          state->refLevel(currentLevel).posId = termIndex;
          // save the state on the stack
          this->duplicateState(state);
          return STOP;
        }
      case CONTINUE_SAME_CHAR:
        // move to the next term immediately
        if (termIndex >= concatList->getCount()-1) {
          // we finished all the terms on this level, so return
          // remove termIndex from the stack
          state->popTermLevel();
          return CONTINUE_SAME_CHAR;
        } else {
          // update the term index on the stack
          termIndex++;
          state->refLevel(currentLevel).posId = termIndex;
          auto term = ti_cast<Data::Grammar::Term>(concatList->getElement(termIndex));
          if (term == 0) {
            Str excMsg = S("Concat term's child term is null. Token def: ");
            excMsg += ID_GENERATOR->getDesc(
              this->getSymbolDefinition(state->getTokenDefIndex())->getId()
            );
            throw EXCEPTION(GenericException, excMsg.c_str());
          }
          state->pushTermLevel(0, term);
        }
        break;
      case STOP:
        // stop processing
        return STOP;
    }
  }
}


Lexer::NextAction Lexer::processReferenceTerm(LexerState *state, WChar inputChar, Int currentLevel)
{
  auto currentTerm = state->refLevel(currentLevel).term;
  ASSERT(currentTerm->isA<Data::Grammar::ReferenceTerm>());
  Data::Grammar::ReferenceTerm *referenceTerm = static_cast<Data::Grammar::ReferenceTerm*>(currentTerm);
  Data::Grammar::SymbolDefinition *def;
  Data::Grammar::Module *retModule;
  if (currentLevel == state->getLevelCount() - 1) {
    // We are entering the reference term now.
    Data::Grammar::Reference *ref = referenceTerm->getReference().get();
    if (ref == 0) {
      Str excMsg = S("Reference is null for ReferenceTerm at token definition: ");
      excMsg += ID_GENERATOR->getDesc(
        this->getSymbolDefinition(state->getTokenDefIndex())->getId()
      );
      throw EXCEPTION(GenericException, excMsg.c_str());
    }
    this->grammarContext.getReferencedSymbol(ref, retModule, def);
    if (retModule != this->grammarContext.getModule()) {
      Str excMsg = S("Lexer doesn't yet support referencing terms in a different module. Token definition: ");
      excMsg += ID_GENERATOR->getDesc(
        this->getSymbolDefinition(state->getTokenDefIndex())->getId()
      );
      throw EXCEPTION(GenericException, excMsg.c_str());
    }
    if (def->getTerm() == 0) {
      Str excMsg = S("Invalid referenced token definition. (");
      excMsg += ID_GENERATOR->getDesc(def->getId());
      excMsg += S("). The definition formula is not set yet.");
      throw EXCEPTION(GenericException, excMsg.c_str());
    }
    if (!(def->getTerm()->isDerivedFrom<Data::Grammar::Term>())) {
      Str excMsg = S("Data driven token definitions are not supported by the lexer yet. Token def: ");
      excMsg += ID_GENERATOR->getDesc(def->getId());
      throw EXCEPTION(GenericException, excMsg.c_str());
    }
    state->pushTermLevel(0, def->getTerm().s_cast_get<Data::Grammar::Term>());
  }
  auto ret = this->processState(state, inputChar, currentLevel + 1);
  if (ret != STOP) {
    state->popTermLevel();
  }
  return ret;
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
Int Lexer::selectBestToken()
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
        Int tokenDefinitionI = this->states[i].getTokenDefIndex();
        Int tokenDefinitionIndex = this->states[index].getTokenDefIndex();
        // check which state is for a constant token
        Bool isConstantI, isConstantIndex;

        // Check if tokenDefinitionI refers to a const term.
        Data::Grammar::SymbolDefinition *def = this->getSymbolDefinition(tokenDefinitionI);
        ASSERT(def->isA<Data::Grammar::SymbolDefinition>());
        Data::Grammar::Term *head = def->getTerm().s_cast_get<Data::Grammar::Term>();
        ASSERT(head->isDerivedFrom<Data::Grammar::Term>());
        if (head->isA<Data::Grammar::ConstTerm>()) isConstantI = true;
        else isConstantI = false;

        // Check if tokenDeinitionIndex refers to a const term.
        def = this->getSymbolDefinition(tokenDefinitionIndex);
        ASSERT(def->isA<Data::Grammar::SymbolDefinition>());
        head = def->getTerm().s_cast_get<Data::Grammar::Term>();
        ASSERT(head->isDerivedFrom<Data::Grammar::Term>());
        if (head->isA<Data::Grammar::ConstTerm>()) isConstantIndex = true;
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
 * Clear the states stack and all other buffers to the state of the machine
 * before parsing started. Token and char group definitions will not be
 * affected.
 */
void Lexer::clear()
{
  this->states.clear();
  this->inputBuffer.clear();
  this->errorBuffer.clear();

  this->tempState.resizeLevels(0);
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
 * LexerState on the stack. The new object will automatically contain a copy of the
 * values in the temp state object (tempState).
 *
 * @return Int Returns the index of the new state on the stack.
 */
Int Lexer::duplicateState(LexerState *state)
{
  Int r;
  if (this->disabledStateIndex != -1) {
    r = this->disabledStateIndex;
    this->disabledStateIndex = -1;
    this->states[r].copyFrom(state);
    return r;
  } else {
    r = this->states.size();
    // Limit the size of the array to a maximum value.
    if (r >= LEXER_STATES_ARRAY_MAX_SIZE) {
      throw EXCEPTION(GenericException, S("States array maximum size exceeded."));
    }
    // Create the new state.
    this->states.push_back(*state);
    return r;
  }
}


/**
 * Copy the state with the given index from the stack into the temp state then
 * disabled it.
 *
 * @param index The index of the state to extract.
 */
void Lexer::extractState(Int index)
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
void Lexer::defragStatesList()
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

} // namespace

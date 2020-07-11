/**
 * @file Core/Processing/Engine.cpp
 * Contains the implementation of class Core::Processing::Engine.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

#ifdef _WIN32
#include "Win32Helpers.h"
#endif

namespace Core { namespace Processing
{

//==============================================================================
// Member Functions

void Engine::initialize(SharedPtr<Data::Ast::Scope> const &rootScope)
{
  // Prepare the lexer.
  this->lexer.initialize(rootScope);
  this->noticeSignal.relay(this->lexer.noticeSignal);

  // Prepare the parser.
  this->parser.initialize(rootScope);
  this->noticeSignal.relay(this->parser.noticeSignal);
  this->lexer.tokenGenerated.connect(this->parser.handleNewTokenSlot);
}


SharedPtr<TiObject> Engine::processString(Char const *str, Char const *name)
{
  if (str == 0) {
    throw EXCEPTION(InvalidArgumentException, S("str"), S("Cannot be null."), str);
  }

  this->parser.beginParsing();

  // Start passing characters to the lexer.

  Data::SourceLocationRecord sourceLocation;
  sourceLocation.filename = std::make_shared<Str>(name);
  sourceLocation.line = 1;
  sourceLocation.column = 1;
  lexer.handleNewString(str, sourceLocation);

  auto endLine = sourceLocation.line;
  auto endColumn = sourceLocation.column;

  lexer.handleNewChar((Char) FILE_TERMINATOR, sourceLocation);

  sourceLocation.line = endLine;
  sourceLocation.column = endColumn;

  return this->parser.endParsing(sourceLocation);
}

SharedPtr<TiObject> Engine::processString(WChar const *str, Char const *name)
{
  if (str == 0) {
    throw EXCEPTION(InvalidArgumentException, S("str"), S("Cannot be null."), str);
  }

  this->parser.beginParsing();

  // Start passing characters to the lexer.

  Data::SourceLocationRecord sourceLocation;
  sourceLocation.filename = std::make_shared<Str>(name);
  sourceLocation.line = 1;
  sourceLocation.column = 1;
  lexer.handleNewString(str, sourceLocation);

  auto endLine = sourceLocation.line;
  auto endColumn = sourceLocation.column;

  lexer.handleNewChar((Char) FILE_TERMINATOR, sourceLocation);

  sourceLocation.line = endLine;
  sourceLocation.column = endColumn;

  return this->parser.endParsing(sourceLocation);
}

SharedPtr<TiObject> Engine::processFile(Char const *filename)
{
  // Open the file.
#ifdef _WIN32
  WStr wFilename = utf8Decode(Str(filename));
  FILE* fd = _wfopen(wFilename.c_str(), L"r, ccs=UTF-8");
  if (!fd) {
    throw EXCEPTION(InvalidArgumentException, S("filename"), S("Could not open file."), filename);
  }
  StdCharInStream finStream(fd);
  auto returnVal = this->processStream(&finStream, filename);
  fclose(fd);
  return returnVal;
#else
  std::ifstream fin(filename);
  if (fin.fail()) {
    throw EXCEPTION(InvalidArgumentException, S("filename"), S("Could not open file."), filename);
  }
  StdCharInStream finStream(&fin);
  return this->processStream(&finStream, filename);
#endif
}


SharedPtr<TiObject> Engine::processStream(CharInStreaming *is, Char const *streamName)
{
  // Open the file.
  if (is == 0) {
    throw EXCEPTION(InvalidArgumentException, S("is"), S("Cannot be null."));
  }

  parser.beginParsing();

  // Start passing characters to the lexer.
  Data::SourceLocationRecord sourceLocation;
  sourceLocation.filename = std::make_shared<Str>(streamName);
  sourceLocation.line = 1;
  sourceLocation.column = 1;
#ifdef _WIN32
  WChar c = is->get();
  Bool isPrevLF = c == L'\n'; // To check for a second line feed on interactive mode in Windows.
#else
  Char c = is->get();
#endif
  while (!is->isEof()) {
    lexer.handleNewChar(c, sourceLocation);
    c = is->get();
#ifdef _WIN32
    // Skipping the second line feed in interactive mode in Windows.
    if (isPrevLF && c == L'\n' && is->getType() == CharInStreaming::CharInStreamingType::INTERACTIVE_CHAR_IN_STREAM) {
      c = is->get();
      isPrevLF = c == L'\n';
    }
#endif
  }

  auto endLine = sourceLocation.line;
  auto endColumn = sourceLocation.column;

  lexer.handleNewChar((Char) FILE_TERMINATOR, sourceLocation);

  sourceLocation.line = endLine;
  sourceLocation.column = endColumn;

  return parser.endParsing(sourceLocation);
}

} } // namespace

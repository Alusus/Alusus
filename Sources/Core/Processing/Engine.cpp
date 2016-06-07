/**
 * @file Core/Processing/Engine.cpp
 * Contains the implementation of class Core::Processing::Engine.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

//==============================================================================
// Member Functions

void Engine::initialize(Data::GrammarRepository *grammarRepo, Data::SharedRepository *definitionsRepo)
{
  // Prepare the lexer.
  this->lexer.setGrammarRepository(grammarRepo);
  this->lexer.buildMsgNotifier.connect(this, &Engine::buildMsgNotifierRelay);

  // Prepare the parser.
  this->parser.initialize(grammarRepo, definitionsRepo);
  this->lexer.tokenGenerated.connect(&this->parser, &Processing::Parser::handleNewToken);
  this->parser.buildMsgNotifier.connect(this, &Engine::buildMsgNotifierRelay);
}


SharedPtr<TiObject> Engine::processString(Char const *str, Char const *name)
{
  if (str == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("str"), STR("Cannot be null."), str);
  }

  parser.beginParsing();

  // Start passing characters to the lexer.

  Data::SourceLocation sourceLocation;
  sourceLocation.filename = std::make_shared<Str>(name);
  sourceLocation.line = 1;
  sourceLocation.column = 1;
  lexer.handleNewString(str, sourceLocation);
  lexer.handleNewChar(FILE_TERMINATOR, sourceLocation);

  return parser.endParsing();
}


SharedPtr<TiObject> Engine::processFile(Char const *filename)
{
  // Open the file.
  std::ifstream fin(filename);
  if (fin.fail()) {
    throw EXCEPTION(InvalidArgumentException, STR("filename"), STR("Could not open file."), filename);
  }

  parser.beginParsing();

  // Start passing characters to the lexer.
  Data::SourceLocation sourceLocation;
  sourceLocation.filename = std::make_shared<Str>(filename);
  sourceLocation.line = 1;
  sourceLocation.column = 1;
  Char c;
  while (!fin.eof()) {
    fin.get(c);
    lexer.handleNewChar(c, sourceLocation);
  }
  lexer.handleNewChar(FILE_TERMINATOR, sourceLocation);

  return parser.endParsing();
}

} } // namespace

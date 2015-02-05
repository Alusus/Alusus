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


SharedPtr<IdentifiableObject> Engine::processString(Char const *str)
{
  if (str == 0) {
    throw InvalidArgumentException(STR("str"), STR("Core::Processing::Engine::processString"),
                                   STR("Cannot be null."));
  }

  parser.beginParsing();

  // Start passing characters to the lexer.
  Int line = 1;
  Int column = 1;
  lexer.handleNewString(str, line, column);
  lexer.handleNewChar(FILE_TERMINATOR, line, column);

  return parser.endParsing();
}


SharedPtr<IdentifiableObject> Engine::processFile(Char const *filename)
{
  // Open the file.
  std::ifstream fin(filename);
  if (fin.fail()) {
    throw InvalidArgumentException(STR("filename"), STR("Core::Processing::Engine::processFile"),
                                   STR("Could not open file."), filename);
  }

  parser.beginParsing();

  // Start passing characters to the lexer.
  Int line = 1;
  Int column = 1;
  Char c;
  while (!fin.eof()) {
    fin.get(c);
    lexer.handleNewChar(c, line, column);
  }
  lexer.handleNewChar(FILE_TERMINATOR, line, column);

  return parser.endParsing();
}

} } // namespace

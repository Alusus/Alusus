/**
 * @file Core/Main/Processor.cpp
 * Contains the implementation of class Core::Main::Processor.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Main
{

//==============================================================================
// Member Functions

void Processor::initialize(const SharedPtr<Lexer::CharGroupDefinitionList> &charGroupDefs,
                           const SharedPtr<Lexer::TokenDefinitionList> &tokenDefs,
                           Data::GrammarRepository *grammarRepo)
{
  // Prepare the lexer.
  this->lexer.setCharGroupDefinitions(charGroupDefs);
  this->lexer.setTokenDefinitions(tokenDefs);
  this->lexer.buildMsgNotifier.connect(this, &Processor::buildMsgNotifierRelay);

  // Prepare the parser.
  this->parser.initialize(grammarRepo);
  this->lexer.tokenGenerated.connect(&this->parser, &Processing::Parser::handleNewToken);
  this->parser.buildMsgNotifier.connect(this, &Processor::buildMsgNotifierRelay);
}


SharedPtr<IdentifiableObject> Processor::processString(Char const *str)
{
  if (str == 0) {
    throw InvalidArgumentException(STR("str"), STR("Core::Main::Processor::processString"),
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


SharedPtr<IdentifiableObject> Processor::processFile(Char const *filename)
{
  // Open the file.
  std::ifstream fin(filename);
  if (fin.fail()) {
    throw InvalidArgumentException(STR("filename"), STR("Core::Main::Processor::processFile"),
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
    if (c == CHR('\n')) {
      line++;
      column = 1;
    } else if (c == CHR('\r')) {
      column = 1;
    } else {
      column++;
    }
  }
  lexer.handleNewChar(FILE_TERMINATOR, line, column);

  return parser.endParsing();
}

} } // namespace

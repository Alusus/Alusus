/**
 * @file Core/Processing/Handlers/GenericCommandParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::GenericCommandParsingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

void GenericCommandParsingHandler::onProdStart(Parser *parser, ParserState *state, Data::Token const *token)
{
  auto command = Data::Ast::GenericCommand::create({ {S("type"), &this->type} });
  command->setSourceLocation(newSrdObj<Data::SourceLocationRecord>(token->getSourceLocation()));
  state->setData(command);
}


Bool GenericCommandParsingHandler::onIncomingModifier(
  Parser *parser, ParserState *state, TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto command = state->getData(levelOffset).ti_cast_get<Data::Ast::GenericCommand>();
  ASSERT(command != 0);
  command->addModifier(modifierData);
  return true;
}


void GenericCommandParsingHandler::addData(
  SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex
) {
  if (state->isAProdRoot(levelIndex)) {
    auto command = state->getData(levelIndex).ti_cast_get<Data::Ast::GenericCommand>();
    ASSERT(command != 0);
    ASSERT(command->getProdId() == UNKNOWN_ID);
    command->addArg(data);
  } else {
    GenericParsingHandler::addData(data, parser, state, levelIndex);
  }
}

} // namespace

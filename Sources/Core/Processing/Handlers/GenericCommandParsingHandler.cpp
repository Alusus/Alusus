/**
 * @file Core/Processing/Handlers/GenericCommandParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::GenericCommandParsingHandler.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

void GenericCommandParsingHandler::onProdStart(Parser *parser, ParserState *state, Ast::Token const *token)
{
  auto command = Ast::GenericCommand::create({ {S("type"), &this->type} });
  command->setSourceLocation(Ast::cloneSourceLocation(token->getSourceLocation().get()));
  state->setData(command);
}


void GenericCommandParsingHandler::addData(
  SharedPtr<Ast::Node> const &data, Parser *parser, ParserState *state, Int levelIndex
) {
  if (state->isAProdRoot(levelIndex)) {
    auto command = state->getData(levelIndex).ti_cast_get<Ast::GenericCommand>();
    ASSERT(command != 0);
    ASSERT(command->getProdId() == UNKNOWN_ID);
    command->addArg(data);
  } else {
    GenericParsingHandler::addData(data, parser, state, levelIndex);
  }
}

} // namespace

/**
 * @file Core/Processing/Handlers/GenericCommandParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::GenericCommandParsingHandler.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing { namespace Handlers
{

void GenericCommandParsingHandler::onProdStart(Parser *parser, ParserState *state, Data::Token const *token)
{
  auto command = Data::Ast::GenericCommand::create({ {STR("type"), &this->type} });
  command->setSourceLocation(std::make_shared<Data::SourceLocationRecord>(token->getSourceLocation()));
  state->setData(command);
}


Bool GenericCommandParsingHandler::onIncomingModifier(
  Parser *parser, ParserState *state, TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto command = state->getData(levelOffset).ti_cast_get<Data::Ast::GenericCommand>();
  ASSERT(command != 0);
  this->prepareToModifyData(state, levelOffset);
  command = state->getData(levelOffset).ti_cast_get<Data::Ast::GenericCommand>();
  ASSERT(command != 0);
  command->addModifier(modifierData);
  return true;
}


void GenericCommandParsingHandler::addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
{
  if (state->isAProdRoot(levelIndex)) {
    auto command = state->getData(levelIndex).ti_cast_get<Data::Ast::GenericCommand>();
    ASSERT(command != 0);
    ASSERT(command->getProdId() == UNKNOWN_ID);
    this->prepareToModifyData(state, levelIndex);
    command = state->getData(levelIndex).ti_cast_get<Data::Ast::GenericCommand>();
    ASSERT(command != 0);
    command->addArg(data);
  } else {
    GenericParsingHandler::addData(data, state, levelIndex);
  }
}


void GenericCommandParsingHandler::prepareToModifyData(Processing::ParserState *state, Int levelIndex)
{
  if (state->isAProdRoot(levelIndex)) {
    if (state->isDataShared(levelIndex)) {
      // Duplicate the data.
      auto command = state->getData(levelIndex).ti_cast_get<Data::Ast::GenericCommand>();
      if (command != 0) {
        auto newCommand = command->clone().s_cast<Data::Ast::GenericCommand>();
        newCommand->setArgs(command->getArgs()->clone().ti_cast<Data::Ast::List>());
        newCommand->setModifiers(command->getModifiers()->clone().ti_cast<Data::Ast::List>());
        state->setData(newCommand, levelIndex);
      }
    }
  } else {
    GenericParsingHandler::prepareToModifyData(state, levelIndex);
  }
}

} } } // namespace

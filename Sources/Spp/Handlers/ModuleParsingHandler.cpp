/**
 * @file Spp/Handlers/ModuleParsingHandler.cpp
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Handlers
{

using namespace Core;
using namespace Core::Data;

void ModuleParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto currentList = state->getData().ti_cast_get<Core::Data::Ast::List>();

  auto bodyIndex = currentList->getElementCount() - 1;
  auto body = currentList->getElement(bodyIndex);

  // We'll use the source location of the "module" keyword, rather than of the first statement.
  auto metaHaving = ti_cast<Core::Data::Ast::MetaHaving>(body);
  metaHaving->setSourceLocation(Core::Data::Ast::findSourceLocation(currentList->getElement(0)));

  if (bodyIndex == 1) {
    state->setData(getSharedPtr(body));
  } else {
    auto defName = ti_cast<Core::Data::Ast::Identifier>(currentList->getElement(1));
    if (defName == 0) {
      throw EXCEPTION(GenericException, S("Invalid element type for module name."));
    }
    auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(currentList);
    auto def = Core::Data::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), metadata->getProdId() },
      { S("sourceLocation"), metadata->findSourceLocation() }
    }, {
      { S("target"), getSharedPtr(body) }
    });
    state->setData(def);
  }
}


Bool ModuleParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Data::Ast::Definition>(data);

  if (definition == 0) return false;

  auto symbolDef = state->refTopProdLevel().getProd();

  // Look for merge modifier.
  auto identifier = modifierData.ti_cast_get<Core::Data::Ast::Identifier>();
  if (identifier != 0) {
    if (symbolDef->getTranslatedModifierKeyword(identifier->getValue().get()) == S("merge")) {
      // Set toMerge in the definition.
      definition->setToMerge(true);
      return true;
    }
  }

  // Add an unknown modifier.
  Core::Data::Ast::translateModifier(symbolDef, modifierData.get());
  definition->addModifier(modifierData);
  return true;
}

} // namespace

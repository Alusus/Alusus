/**
 * @file Spp/Handlers/ModuleParsingHandler.cpp
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

void ModuleParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto currentList = state->getData().ti_cast_get<Core::Ast::List>();

  auto bodyIndex = currentList->getElementCount() - 1;
  auto body = currentList->getElement(bodyIndex);

  // We'll use the source location of the "module" keyword, rather than of the first statement.
  body->setSourceLocation(Core::Ast::findSourceLocation(currentList->getElement(0)));

  if (bodyIndex == 1) {
    state->setData(getSharedPtr(body));
  } else {
    auto defName = ti_cast<Core::Ast::Identifier>(currentList->getElement(1));
    if (defName == 0) {
      throw EXCEPTION(GenericException, S("Invalid element type for module name."));
    }
    auto def = Core::Ast::Definition::create({
      { S("name"), defName->getValue() },
      { S("prodId"), currentList->getProdId() },
      { S("sourceLocation"), currentList->findSourceLocation() }
    }, {
      { S("target"), getSharedPtr(body) }
    });
    state->setData(def);
  }
}


Bool ModuleParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  SharedPtr<Core::Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (GenericParsingHandler::onIncomingModifier(parser, state, modifierData, prodProcessingComplete)) {
    return true;
  }

  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Ast::Definition>(data);

  if (definition == 0) return false;

  // Look for merge modifier. Its keyword may have already been translated by the call to the parent
  // implementation above.
  auto identifier = modifierData.ti_cast_get<Core::Ast::Identifier>();
  if (identifier != 0 && identifier->getValue() == S("merge")) {
    // Set toMerge in the definition.
    definition->setToMerge(true);
    return true;
  }

  // The modifier is unknown to us, leave it for an outer parsing handler to deal with.
  return false;
}

} // namespace

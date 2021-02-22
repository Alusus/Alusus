/**
 * @file Core/Processing/Handlers/DefParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::DefParsingHandler.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

using namespace Core;
using namespace Core::Data;


//==============================================================================
// Overloaded Abstract Functions

void DefParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();
  ASSERT(expr != 0);
  auto exprMetadata = ti_cast<Core::Data::Ast::MetaHaving>(expr);
  ASSERT(exprMetadata != 0);

  Core::Data::Ast::LinkOperator *linkOp = 0;
  if (expr->getCount() == 2) {
    linkOp = ti_cast<Core::Data::Ast::LinkOperator>(expr->getElement(1));
  }
  if (linkOp == 0) {
    state->addNotice(newSrdObj<Notices::MissingDefLinkNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Get the name of the definition.
  auto nameToken = linkOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (nameToken == 0) {
    state->addNotice(newSrdObj<Notices::MissingDefNameNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  auto name = nameToken->getValue();

  // Get the definition target (after the colon).
  auto val = linkOp->getSecond();
  if (val == 0) {
    // TODO: We need to choose terms for the parts of a define command, e.g.
    // definition name, definition, etc.
    state->addNotice(newSrdObj<Notices::InvalidDefCommandNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  if(val->isDerivedFrom<Core::Data::Ast::AssignmentOperator>()) {
    auto def = Core::Data::Ast::Definition::create({
      { "prodId", exprMetadata->getProdId() },
      { "sourceLocation", exprMetadata->findSourceLocation() },
      { "name", name }
    }, {
      { "target", static_cast<Core::Data::Ast::AssignmentOperator*>(val.get())->getFirst() }
    });

    auto assignment = Core::Data::Ast::AssignmentOperator::create({
      { "prodId", exprMetadata->getProdId() },
      { "sourceLocation", exprMetadata->findSourceLocation() },
      { "type", TiStr("=") }
    }, {
      { "first", nameToken },
      { "second", static_cast<Core::Data::Ast::AssignmentOperator*>(val.get())->getSecond() }
    });

    // Create the definition.
    state->setData(Core::Data::Ast::MergeList::create({}, {
      def,
      assignment
    }));
  } else {
    auto def = Core::Data::Ast::Definition::create({
      { "prodId", exprMetadata->getProdId() },
      { "sourceLocation", exprMetadata->findSourceLocation() },
      { "name", name }
    }, {
      { "target", val }
    });

    // Create the definition.
    state->setData(def);
  }
}


Bool DefParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Data::Ast::Definition>(data);

  // If we couldn't find a definition, it's probably because of a syntax error.
  // We'll just ignore the modifier in this case.
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

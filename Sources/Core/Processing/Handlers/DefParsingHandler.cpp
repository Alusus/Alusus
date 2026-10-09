/**
 * @file Core/Processing/Handlers/DefParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::DefParsingHandler.
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

//==============================================================================
// Overloaded Abstract Functions

void DefParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Ast::List>();
  ASSERT(expr != 0);

  Core::Ast::LinkOperator *linkOp = 0;
  if (expr->getCount() == 2) {
    linkOp = ti_cast<Core::Ast::LinkOperator>(expr->getElement(1));
  }
  if (linkOp == 0) {
    state->addNotice(newSrdObj<Notices::MissingDefLinkNotice>(expr->findSourceLocation()));
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }

  // Get the name of the definition.
  auto nameToken = linkOp->getFirst().ti_cast_get<Core::Ast::Identifier>();
  if (nameToken == 0) {
    state->addNotice(newSrdObj<Notices::MissingDefNameNotice>(expr->findSourceLocation()));
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }
  auto name = nameToken->getValue();

  // Get the definition target (after the colon).
  auto val = linkOp->getSecond();
  if (val == 0) {
    // TODO: We need to choose terms for the parts of a define command, e.g.
    // definition name, definition, etc.
    state->addNotice(newSrdObj<Notices::InvalidDefCommandNotice>(expr->findSourceLocation()));
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }

  if(val->isDerivedFrom<Core::Ast::AssignmentOperator>()) {
    auto def = Core::Ast::Definition::create({
      { "prodId", expr->getProdId() },
      { "sourceLocation", expr->findSourceLocation() },
      { "name", name }
    }, {
      { "target", static_cast<Core::Ast::AssignmentOperator*>(val.get())->getFirst() }
    });

    auto assignment = Core::Ast::AssignmentOperator::create({
      { "prodId", expr->getProdId() },
      { "sourceLocation", expr->findSourceLocation() },
      { "type", TiStr("=") }
    }, {
      { "first", nameToken },
      { "second", static_cast<Core::Ast::AssignmentOperator*>(val.get())->getSecond() }
    });

    // Create the definition.
    state->setData(Core::Ast::MergeList::create({}, {
      def,
      assignment
    }));
  } else {
    auto def = Core::Ast::Definition::create({
      { "prodId", expr->getProdId() },
      { "sourceLocation", expr->findSourceLocation() },
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
  SharedPtr<Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (GenericParsingHandler::onIncomingModifier(parser, state, modifierData, prodProcessingComplete)) {
    return true;
  }

  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Ast::Definition>(data);

  // If we couldn't find a definition, it's probably because of a syntax error.
  // We'll just ignore the modifier in this case.
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

/**
 * @file Core/Processing/Handlers/DefParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::DefParsingHandler.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing { namespace Handlers
{

using namespace Core;
using namespace Core::Data;


//==============================================================================
// Overloaded Abstract Functions

void DefParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Data::Ast::List>();
  ASSERT(expr != 0);
  auto exprMetadata = ti_cast<Core::Data::Ast::Metadata>(expr);
  ASSERT(exprMetadata != 0);

  Core::Data::Ast::LinkOperator *linkOp = 0;
  if (expr->getCount() == 2) {
    linkOp = ti_cast<Core::Data::Ast::LinkOperator>(expr->get(1));
  }
  if (linkOp == 0) {
    state->addBuildMsg(std::make_shared<MissingDefLink>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Get the name of the definition.
  auto nameToken = linkOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (nameToken == 0) {
    state->addBuildMsg(std::make_shared<MissingDefName>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  auto name = nameToken->getValue();

  // Get the definee (after the colon).
  auto val = linkOp->getSecond();
  if (val == 0) {
    // TODO: We need to choose terms for the parts of a define command, e.g.
    // definition name, definition, etc.
    state->addBuildMsg(std::make_shared<InvalidDefCommand>(exprMetadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Create the definition.
  auto def = Core::Data::Ast::Definition::create({
    { "prodId", exprMetadata->getProdId() },
    { "sourceLocation", exprMetadata->getSourceLocation() },
    { "name", name }
  }, {
    { "target", val }
  });
  state->setData(def);
}

} } } // namespace

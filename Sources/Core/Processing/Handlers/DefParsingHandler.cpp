/**
 * @file Core/Processing/Handlers/DefParsingHandler.cpp
 * Contains the implementation of Core::Processing::Handlers::DefParsingHandler.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
    state->addNotice(std::make_shared<Notices::MissingDefLinkNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Get the name of the definition.
  auto nameToken = linkOp->getFirst().ti_cast_get<Core::Data::Ast::Identifier>();
  if (nameToken == 0) {
    state->addNotice(std::make_shared<Notices::MissingDefNameNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }
  auto name = nameToken->getValue();

  // Get the definee (after the colon).
  auto val = linkOp->getSecond();
  if (val == 0) {
    // TODO: We need to choose terms for the parts of a define command, e.g.
    // definition name, definition, etc.
    state->addNotice(std::make_shared<Notices::InvalidDefCommandNotice>(exprMetadata->findSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  // Create the definition.
  auto def = Core::Data::Ast::Definition::create({
    { "prodId", exprMetadata->getProdId() },
    { "sourceLocation", exprMetadata->findSourceLocation() },
    { "name", name }
  }, {
    { "target", val }
  });
  state->setData(def);
}


Bool DefParsingHandler::onIncomingModifier(
  Core::Processing::Parser *parser, Core::Processing::ParserState *state,
  TioSharedPtr const &modifierData, Bool prodProcessingComplete
) {
  if (!prodProcessingComplete) return false;

  // Prepare to modify.
  Int levelOffset = -state->getTopProdTermLevelCount();
  this->prepareToModifyData(state, levelOffset);
  auto data = state->getData(levelOffset).get();
  auto definition = ti_cast<Core::Data::Ast::Definition>(data);

  // Look for merge modifier.
  auto identifier = modifierData.ti_cast_get<Core::Data::Ast::Identifier>();
  if (identifier == 0) return false;
  auto symbolDef = state->refTopProdLevel().getProd();
  if (symbolDef->getTranslatedModifierKeyword(identifier->getValue().get()) == S("merge")) {
    // Set toMerge in the definition.
    definition->setToMerge(true);
  } else {
    // Add an unknown modifier.
    this->translateModifier(symbolDef, modifierData.get());
    definition->addModifier(modifierData);
  }

  return true;
}


void DefParsingHandler::translateModifier(Data::Grammar::SymbolDefinition *symbolDef, TiObject *modifier)
{
  if (modifier->isDerivedFrom<Data::Ast::Identifier>()) {
    auto identifier = static_cast<Data::Ast::Identifier*>(modifier);
    identifier->setValue(symbolDef->getTranslatedModifierKeyword(identifier->getValue().get()).c_str());
  } else if (modifier->isDerivedFrom<Data::Ast::LinkOperator>()) {
    auto link = static_cast<Data::Ast::LinkOperator*>(modifier);
    this->translateModifier(symbolDef, link->getFirst().get());
  } else if (modifier->isDerivedFrom<Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Data::Ast::ParamPass*>(modifier);
    this->translateModifier(symbolDef, paramPass->getOperand().get());
  }
}

} // namespace

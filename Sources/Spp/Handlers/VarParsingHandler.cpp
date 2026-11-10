/**
 * @file Spp/Handlers/VarParsingHandler.cpp
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

void VarParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  GenericParsingHandler::onProdEnd(parser, state);

  auto expr = state->getData().ti_cast_get<Core::Ast::List>();
  ASSERT(expr != 0);
  auto astExpr = expr->getCount() == 2 ? expr->getElement(1) : 0;

  // Multiple variables, as in `var a = 1, b: Int`, result in a merge list of definitions.
  if (astExpr != 0 && astExpr->isA<Core::Ast::List>()) {
    auto list = static_cast<Core::Ast::List*>(astExpr);
    auto within = Core::Ast::Within::create({
      { "sourceLocation", expr->findSourceLocation() }
    });
    auto defs = Core::Ast::List::create();
    within->setSubject(defs);
    for (Int i = 0; i < list->getCount(); ++i) {
      auto var = this->createVar(list->getElement(i), expr, state);
      if (var == 0) {
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
      if (!var->isDerivedFrom<Core::Ast::Definition>()) {
        state->addNotice(newSrdObj<Spp::Notices::InvalidVarCommandNotice>(expr->findSourceLocation()));
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
      defs->add(var);
    }
    state->setData(within);
  } else {
    state->setData(this->createVar(astExpr, expr, state));
  }
}


Bool VarParsingHandler::onIncomingModifier(
  Processing::Parser *parser, Processing::ParserState *state,
  SharedPtr<Core::Ast::Node> const &modifierData, Bool prodProcessingComplete
) {
  if (!GenericParsingHandler::onIncomingModifier(parser, state, modifierData, prodProcessingComplete)) return false;

  // A within object only passes its modifiers to its first element, but the modifiers of a var command that defines
  // multiple variables apply to all of them, so we move the modifiers to each of the definitions.
  Int levelOffset = -state->getTopProdTermLevelCount();
  auto within = state->getData(levelOffset).ti_cast_get<Core::Ast::Within>();
  if (within != 0) {
    auto defs = within->getSubject().ti_cast_get<Core::Ast::List>();
    for (Int i = 0; defs != 0 && i < defs->getCount(); ++i) {
      auto element = defs->getElement(i);
      if (within->getModifiers() != 0) element->addModifiers(Core::Ast::clone(within->getModifiers().get()));
      if (within->getMetadata() != 0) element->addMetadata(Core::Ast::clone(within->getMetadata().get()));
    }
    within->setModifiers(SharedPtr<Core::Ast::List>());
    within->setMetadata(SharedPtr<Core::Ast::Map>());
  }
  return true;
}


SharedPtr<Core::Ast::Node> VarParsingHandler::createVar(
  Core::Ast::Node *astExpr, Core::Ast::Node *expr, Processing::ParserState *state
) {
  // Split the expression into its parts. The possible forms are:
  // var name: type
  // var name: type(args)
  // var name: type = value
  // var name := args
  // var name = value
  SharedPtr<Core::Ast::Node> nameNode;
  SharedPtr<Core::Ast::Node> initArgs;
  Spp::Ast::VarInitMode initMode;
  Bool valid = true;
  auto linkOp = ti_cast<Core::Ast::LinkOperator>(astExpr);
  auto assignmentOp = ti_cast<Core::Ast::AssignmentOperator>(astExpr);
  if (linkOp != 0 && linkOp->getType() == S(":")) {
    auto typeAssignmentOp = linkOp->getSecond().ti_cast_get<Core::Ast::AssignmentOperator>();
    if (typeAssignmentOp != 0 && typeAssignmentOp->getType() == S(":=")) {
      state->addNotice(newSrdObj<Spp::Notices::VarInitArgsWithExplicitTypeNotice>(linkOp->findSourceLocation()));
      return SharedPtr<Core::Ast::Node>::null;
    }
    auto def = createTypedVarDef(linkOp, expr->getProdId().get(), expr->findSourceLocation());
    if (def == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidVarCommandNotice>(expr->findSourceLocation()));
    }
    return def;
  } else if (assignmentOp != 0 && assignmentOp->getType() == S("=")) {
    initMode = Spp::Ast::VarInitMode::COPY;
    nameNode = assignmentOp->getFirst();
    initArgs = assignmentOp->getSecond();
    valid = initArgs != 0;
  } else if (assignmentOp != 0 && assignmentOp->getType() == S(":=")) {
    initMode = Spp::Ast::VarInitMode::BIND;
    nameNode = assignmentOp->getFirst();
    initArgs = assignmentOp->getSecond();
  } else if (assignmentOp == 0) {
    // `var expression`, where the expression can be either the type of the var or its initial value, which can only
    // be determined later, so we store it as the initial value and let type deduction decide.
    if (astExpr == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidVarCommandNotice>(expr->findSourceLocation()));
      return SharedPtr<Core::Ast::Node>::null;
    }
    return Spp::Ast::Variable::create({
      { "prodId", expr->getProdId() },
      { "sourceLocation", expr->findSourceLocation() },
      { "initMode", Spp::Ast::VarInitMode(Spp::Ast::VarInitMode::UNRESOLVED) }
    }, {
      { "initArgs", getSharedPtr(astExpr) }
    });
  }

  auto name = nameNode.ti_cast_get<Core::Ast::Identifier>();
  if (!valid || name == 0 || initArgs == 0) {
    state->addNotice(newSrdObj<Spp::Notices::InvalidVarCommandNotice>(expr->findSourceLocation()));
    return SharedPtr<Core::Ast::Node>::null;
  }

  auto var = Spp::Ast::Variable::create({
    { "prodId", expr->getProdId() },
    { "sourceLocation", expr->findSourceLocation() },
    { "initMode", initMode }
  }, {
    { "initArgs", initArgs }
  });

  return Core::Ast::Definition::create({
    { "prodId", expr->getProdId() },
    { "sourceLocation", expr->findSourceLocation() },
    { "name", name->getValue() }
  }, {
    { "target", var }
  });
}


SharedPtr<Core::Ast::Definition> VarParsingHandler::createTypedVarDef(
  Core::Ast::LinkOperator *linkOp, Word prodId, SharedPtr<Core::Ast::SourceLocation> const &sourceLocation
) {
  auto name = linkOp->getFirst().ti_cast_get<Core::Ast::Identifier>();
  auto typeRef = linkOp->getSecond();
  SharedPtr<Core::Ast::Node> initArgs;
  Spp::Ast::VarInitMode initMode = Spp::Ast::VarInitMode::BIND;
  auto typeAssignmentOp = typeRef.ti_cast_get<Core::Ast::AssignmentOperator>();
  if (typeAssignmentOp != 0 && typeAssignmentOp->getType() == S("=")) {
    initMode = Spp::Ast::VarInitMode::COPY;
    initArgs = typeAssignmentOp->getSecond();
    typeRef = typeAssignmentOp->getFirst();
    if (initArgs == 0) return SharedPtr<Core::Ast::Definition>::null;
  } else if (typeAssignmentOp != 0 && typeAssignmentOp->getType() == S(":=")) {
    // Constructor args can't be given with `:=` when the type is given explicitly.
    return SharedPtr<Core::Ast::Definition>::null;
  }
  // Constructor args, as in `var name: type(args)`, are left within the type ref and extracted during code
  // generation, since the type ref might only take that form after macro expansion.

  if (name == 0 || (typeRef == 0 && initArgs == 0)) return SharedPtr<Core::Ast::Definition>::null;

  auto var = Spp::Ast::Variable::create({
    { "prodId", TiWord(prodId) },
    { "sourceLocation", sourceLocation },
    { "initMode", initMode }
  }, {
    { "typeRef", typeRef },
    { "initArgs", initArgs }
  });

  return Core::Ast::Definition::create({
    { "prodId", TiWord(prodId) },
    { "sourceLocation", sourceLocation },
    { "name", name->getValue() }
  }, {
    { "target", var }
  });
}

} // namespace

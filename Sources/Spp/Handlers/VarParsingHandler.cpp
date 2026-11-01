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

  // Split the expression into its parts. The possible forms are:
  // var name: type
  // var name: type(args)
  // var name: type = value
  // var name :: args
  // var name = value
  SharedPtr<Core::Ast::Node> nameNode;
  SharedPtr<Core::Ast::Node> typeRef;
  SharedPtr<Core::Ast::Node> initArgs;
  Spp::Ast::VarInitMode initMode = Spp::Ast::VarInitMode::BIND;
  Bool valid = true;
  auto linkOp = ti_cast<Core::Ast::LinkOperator>(astExpr);
  auto assignmentOp = ti_cast<Core::Ast::AssignmentOperator>(astExpr);
  if (linkOp != 0 && linkOp->getType() == S("::")) {
    auto innerLinkOp = linkOp->getFirst().ti_cast_get<Core::Ast::LinkOperator>();
    if (innerLinkOp != 0 && innerLinkOp->getType() == S(":")) {
      state->addNotice(newSrdObj<Spp::Notices::VarInitArgsWithExplicitTypeNotice>(linkOp->findSourceLocation()));
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
    nameNode = linkOp->getFirst();
    initArgs = linkOp->getSecond();
  } else if (linkOp != 0 && linkOp->getType() == S(":")) {
    nameNode = linkOp->getFirst();
    typeRef = linkOp->getSecond();
    auto typeAssignmentOp = typeRef.ti_cast_get<Core::Ast::AssignmentOperator>();
    if (typeAssignmentOp != 0 && typeAssignmentOp->getType() == S("=")) {
      initMode = Spp::Ast::VarInitMode::COPY;
      initArgs = typeAssignmentOp->getSecond();
      typeRef = typeAssignmentOp->getFirst();
      valid = initArgs != 0;
    }
    // Constructor args, as in `var name: type(args)`, are left within the type ref and extracted during code
    // generation, since the type ref might only take that form after macro expansion.
  } else if (assignmentOp != 0 && assignmentOp->getType() == S("=")) {
    initMode = Spp::Ast::VarInitMode::COPY;
    nameNode = assignmentOp->getFirst();
    initArgs = assignmentOp->getSecond();
    valid = initArgs != 0;
  } else if (assignmentOp == 0) {
    // `var expression`, where the expression can be either the type of the var or its initial value, which can only
    // be determined later, so we store it as the initial value and let type deduction decide.
    if (astExpr == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidVarCommandNotice>(expr->findSourceLocation()));
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
    state->setData(Spp::Ast::Variable::create({
      { "prodId", expr->getProdId() },
      { "sourceLocation", expr->findSourceLocation() },
      { "initMode", Spp::Ast::VarInitMode(Spp::Ast::VarInitMode::UNRESOLVED) }
    }, {
      { "initArgs", getSharedPtr(astExpr) }
    }));
    return;
  }

  auto name = nameNode.ti_cast_get<Core::Ast::Identifier>();
  if (!valid || name == 0 || (typeRef == 0 && initArgs == 0)) {
    state->addNotice(newSrdObj<Spp::Notices::InvalidVarCommandNotice>(expr->findSourceLocation()));
    state->setData(SharedPtr<Core::Ast::Node>(0));
    return;
  }

  auto var = Spp::Ast::Variable::create({
    { "prodId", expr->getProdId() },
    { "sourceLocation", expr->findSourceLocation() },
    { "initMode", initMode }
  }, {
    { "typeRef", typeRef },
    { "initArgs", initArgs }
  });

  state->setData(Core::Ast::Definition::create({
    { "prodId", expr->getProdId() },
    { "sourceLocation", expr->findSourceLocation() },
    { "name", name->getValue() }
  }, {
    { "target", var }
  }));
}

} // namespace

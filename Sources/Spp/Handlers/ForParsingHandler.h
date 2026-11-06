/**
 * @file Spp/Handlers/ForParsingHandler.h
 * Contains the header of class Spp::Handlers::ForParsingHandler
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_FORPARSINGHANDLER_H
#define SPP_HANDLERS_FORPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class ForParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ForParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.org");


  //============================================================================
  // Constructor

  public: ForParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state)
  {
    GenericParsingHandler::onProdEnd(parser, state);

    auto expr = state->getData().ti_cast_get<Core::Ast::List>();
    ASSERT(expr != 0);

    if (expr->getCount() != 3) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidForStatementNotice>(expr->findSourceLocation()));
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }

    // The head can optionally be enclosed in brackets, as in `for (i = 0, i < 10, ++i)`.
    auto headBracket = ti_cast<Core::Ast::Bracket>(expr->getElement(1));
    auto head = headBracket != 0 && headBracket->getType() == Core::Ast::BracketType::ROUND
      ? headBracket->getOperand().ti_cast_get<Core::Ast::List>()
      : ti_cast<Core::Ast::List>(expr->getElement(1));
    if (head == 0 || head->getCount() != 3) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidForStatementNotice>(expr->findSourceLocation()));
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return;
    }
    for (Int i = 1; i < head->getCount(); ++i) {
      if (head->get(i) == 0) {
        state->addNotice(newSrdObj<Spp::Notices::InvalidForStatementNotice>(expr->findSourceLocation()));
        state->setData(SharedPtr<Core::Ast::Node>(0));
        return;
      }
    }

    // Variables defined in the initializer, as in `for i: Int = 0, ...` or `for (i: Int = 0, j: Int = 1), ...`, are
    // moved into definitions preceding the for statement, and both are wrapped in a block to limit the scope of
    // these variables to the loop.
    auto initializer = head->get(0);
    SharedPtr<Spp::Ast::Block> block;
    if (this->isVarDef(initializer.get())) {
      block = this->createBlock(expr);
      if (!this->addVarDef(initializer.get(), block.get(), state)) return;
      initializer.reset();
    } else if (initializer.ti_cast_get<Core::Ast::Bracket>() != 0) {
      auto operand = initializer.s_cast_get<Core::Ast::Bracket>()->getOperand();
      auto list = operand.ti_cast_get<Core::Ast::List>();
      if (this->isVarDef(operand.get())) {
        block = this->createBlock(expr);
        if (!this->addVarDef(operand.get(), block.get(), state)) return;
        initializer.reset();
      } else if (list != 0 && this->hasVarDef(list)) {
        // Other initializers are added to the block as statements, in their original order.
        block = this->createBlock(expr);
        for (Int i = 0; i < list->getCount(); ++i) {
          if (this->isVarDef(list->getElement(i))) {
            if (!this->addVarDef(list->getElement(i), block.get(), state)) return;
          } else {
            block->add(list->get(i));
          }
        }
        initializer.reset();
      }
    }

    auto newObj = newSrdObj<Spp::Ast::ForStatement>();
    newObj->setSourceLocation(expr->findSourceLocation());
    newObj->setProdId(expr->getProdId());
    newObj->setInitializer(initializer);
    newObj->setCondition(head->get(1));
    newObj->setUpdater(head->get(2));
    newObj->setBody(expr->get(2));

    if (block != 0) {
      block->add(newObj);
      state->setData(block);
    } else {
      state->setData(newObj);
    }
  }

  // Var definitions can be enclosed in brackets to separate them from the initializers that follow, since the var
  // command consumes the rest of the comma separated list, as in `((var i = 0), j = i)`.
  private: Core::Ast::Node* unwrapBrackets(Core::Ast::Node *node)
  {
    auto bracket = ti_cast<Core::Ast::Bracket>(node);
    while (bracket != 0 && bracket->getType() == Core::Ast::BracketType::ROUND) {
      node = bracket->getOperand().get();
      bracket = ti_cast<Core::Ast::Bracket>(node);
    }
    return node;
  }

  private: Bool isVarDef(Core::Ast::Node *node)
  {
    node = this->unwrapBrackets(node);
    if (ti_cast<Core::Ast::Definition>(node) != 0 || ti_cast<Core::Ast::Within>(node) != 0) return true;
    auto linkOp = ti_cast<Core::Ast::LinkOperator>(node);
    return linkOp != 0 && linkOp->getType() == S(":");
  }

  private: Bool hasVarDef(Core::Ast::List *list)
  {
    for (Int i = 0; i < list->getCount(); ++i) {
      if (this->isVarDef(list->getElement(i))) return true;
    }
    return false;
  }

  private: SharedPtr<Spp::Ast::Block> createBlock(Core::Ast::Node *expr)
  {
    auto block = Spp::Ast::Block::create();
    block->setSourceLocation(expr->findSourceLocation());
    return block;
  }

  private: Bool addVarDef(
    Core::Ast::Node *node, Spp::Ast::Block *block, Core::Processing::ParserState *state
  ) {
    node = this->unwrapBrackets(node);
    if (node->isDerivedFrom<Core::Ast::Definition>()) {
      block->add(getSharedPtr(node));
      return true;
    }
    if (node->isDerivedFrom<Core::Ast::Within>()) {
      // Multiple variables defined by a single var command.
      auto defs = static_cast<Core::Ast::Within*>(node)->getSubject().ti_cast_get<Core::Ast::List>();
      for (Int i = 0; defs != 0 && i < defs->getCount(); ++i) block->add(defs->get(i));
      return true;
    }
    auto def = VarParsingHandler::createTypedVarDef(
      static_cast<Core::Ast::LinkOperator*>(node), UNKNOWN_ID, node->findSourceLocation()
    );
    if (def == 0) {
      state->addNotice(newSrdObj<Spp::Notices::InvalidForStatementNotice>(node->findSourceLocation()));
      state->setData(SharedPtr<Core::Ast::Node>(0));
      return false;
    }
    block->add(def);
    return true;
  }

}; // class

} } // namespace

#endif

/**
 * @file Spp/Handlers/handlers.cpp
 * Contains the global implementations of Spp::Handlers namespace's declarations.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Handlers
{

Bool processFunctionArgPacks(Spp::Ast::FunctionType *funcType, Core::Notices::Store *noticeStore)
{
  if (funcType == 0) {
    throw EXCEPTION(GenericException, S("Function is missing a function type."));
  }
  if (funcType->getArgTypes() == 0) return true;
  for (Int i = 0; i < funcType->getArgTypes()->getCount(); ++i) {
    auto type = funcType->getArgTypes()->getElement(i);
    if (type->isA<Core::Data::Ast::PrefixOperator>()) {
      auto prefixOp = static_cast<Core::Data::Ast::PrefixOperator*>(type);
      if (prefixOp->getType() != S("...")) {
        noticeStore->add(
          newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(prefixOp->findSourceLocation())
        );
        return false;
      }
      TioSharedPtr packType;
      TiWord packMin = 0;
      TiWord packMax = 0;
      auto operand = prefixOp->getOperand();
      if (operand->isA<Core::Data::Ast::Bracket>()) {
        auto bracket = operand.s_cast<Core::Data::Ast::Bracket>();
        if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
          noticeStore->add(
            newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(bracket->findSourceLocation())
          );
          return false;
        }
        auto bracketOperand = bracket->getOperand();
        if (bracketOperand == 0) {
          noticeStore->add(
            newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(bracket->findSourceLocation())
          );
          return false;
        }
        if (bracketOperand->isA<Core::Data::Ast::List>()) {
          auto bracketList = bracketOperand.s_cast<Core::Data::Ast::List>();
          if (bracketList->getCount() == 0 || bracketList->getCount() > 3) {
            noticeStore->add(
              newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(bracketList->findSourceLocation())
            );
            return false;
          }
          packType = bracketList->get(0);
          if (bracketList->getCount() > 1) {
            if (!parseNumber(bracketList->getElement(1), packMin, bracketList.get(), noticeStore)) return false;
          }
          if (bracketList->getCount() > 2) {
            if (!parseNumber(bracketList->getElement(2), packMax, bracketList.get(), noticeStore)) return false;
          }
        } else {
          packType = bracketOperand;
        }
      } else {
        packType = operand;
      }
      if (packType->isA<Core::Data::Ast::Identifier>()) {
        auto packIdentifier = packType.s_cast<Core::Data::Ast::Identifier>();
        if (packIdentifier->getValue() == S("any")) {
          packType = 0;
        }
      }
      funcType->getArgTypes()->set(i, Ast::ArgPack::create({
        { S("min"), &packMin },
        { S("max"), &packMax }
      }, {
        { S("argType"), packType }
      }));
    } else {
      // Do we have an arg pack prior to this arg?
      for (Int j = 0; j < i; ++j) {
        if (funcType->getArgTypes()->getElement(j)->isDerivedFrom<Ast::ArgPack>()) {
          // We cannot have a normal argument following an arg pack.
          noticeStore->add(
            newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(Core::Data::Ast::findSourceLocation(type))
          );
          return false;
        }
      }
    }
  }
  return true;
}


Bool parseNumber(
  TiObject *ast, TiWord &result, Core::Data::Ast::MetaHaving *parentMetadata, Core::Notices::Store *noticeStore
) {
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(ast);
  if (ast->isA<Core::Data::Ast::IntegerLiteral>()) {
    result = std::stol(static_cast<Core::Data::Ast::IntegerLiteral*>(ast)->getValue().get());
    return true;
  } else {
    if (metadata) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(metadata->findSourceLocation())
      );
    } else {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidFunctionArgTypeNotice>(parentMetadata->findSourceLocation())
      );
    }
    return false;
  }
}

} // namespace

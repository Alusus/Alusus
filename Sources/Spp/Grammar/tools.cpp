/**
 * @file Spp/Grammar/tools.cpp
 * Contains implementations of grammar utility functions.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Grammar
{

using namespace Core::Grammar;
using namespace Core::Processing;

using Map = Core::Grammar::Map;

Bool parseCommandSection(
  Core::Ast::Node *ast, CommandSection &section, Core::Notices::Store *noticeStore
) {
  Core::Ast::Node *sectionAst;
  if (!parseMinMax(ast, sectionAst, section.min, section.max, noticeStore)) return false;
  auto scope = ti_cast<Core::Ast::Scope>(sectionAst);
  if (scope == 0) {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(sectionAst))
    );
    return false;
  }
  for (Int i = 0; i < scope->getCount(); ++i) {
    auto linkOp = ti_cast<Core::Ast::LinkOperator>(scope->getElement(i));
    if (linkOp == 0 || linkOp->getType() != S(":")) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(scope->getElement(i)))
      );
      return false;
    }
    auto id = linkOp->getFirst().ti_cast_get<Core::Ast::Identifier>();
    if (id == 0) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(linkOp))
      );
      return false;
    }
    if (id->getValue() == S("keywords") || id->getValue() == S("مفاتيح")) {
      if (!parseCommandKeywords(linkOp->getSecond().get(), section.keywords, noticeStore)) return false;
    } else if (id->getValue() == S("args") || id->getValue() == S("معطيات")) {
      Array<Core::Ast::Node*> argList;
      convertInfixOpIntoList(linkOp->getSecond().get(), S("+"), argList);
      for (Int j = 0; j < argList.getLength(); ++j) {
        CommandArg arg;
        if (!parseCommandArg(argList.at(j), arg, noticeStore)) return false;
        section.args.push_back(arg);
      }
    } else {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(id))
      );
      return false;
    }
  }
  if (section.keywords == 0) {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(scope))
    );
    return false;
  }
  section.flags = TiInt::create(ParsingFlags::PASS_ITEMS_UP);
  return true;
}


Bool parseCommandKeywords(
  Core::Ast::Node *ast, SharedPtr<Map> &keywords, Core::Notices::Store *noticeStore
) {
  Array<Core::Ast::Node*> keywordList;
  convertInfixOpIntoList(ast, S("|"), keywordList);
  if (keywordList.getLength() == 0) {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(ast))
    );
    return false;
  }
  keywords = Map::create();
  for (Int i = 0; i < keywordList.getLength(); ++i) {
    auto strLiteral = ti_cast<Core::Ast::StringLiteral>(keywordList.at(i));
    if (strLiteral == 0) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(keywordList.at(i)))
      );
      return false;
    }
    keywords->add(strLiteral->getValue().get(), SharedPtr<Core::Ast::Node>::null);
  }
  return true;
}


Bool parseCommandArg(
  Core::Ast::Node *ast, CommandArg &arg, Core::Notices::Store *noticeStore
) {
  Core::Ast::Node *argAst;
  if (!parseMinMax(ast, argAst, arg.min, arg.max, noticeStore)) return false;
  Str qualifier;
  if (!parseQualifier(argAst, qualifier, noticeStore)) return false;
  arg.prod = PARSE_REF(qualifier);
  arg.flags = TiInt::create(ParsingFlags::PASS_ITEMS_UP);
  return true;
}


Bool parseMinMax(
  Core::Ast::Node *ast, Core::Ast::Node *&resultAst, SharedPtr<TiInt> &min, SharedPtr<TiInt> &max, Core::Notices::Store *noticeStore
) {
  auto mulOp = ti_cast<Core::Ast::MultiplicationOperator>(ast);
  if (mulOp == 0 || mulOp->getType() != S("*")) {
    resultAst = ast;
    return true;
  }
  resultAst = mulOp->getFirst().get();
  auto intLiteral = mulOp->getSecond().ti_cast_get<Core::Ast::IntegerLiteral>();
  if (intLiteral != 0) {
    auto num = std::stoi(intLiteral->getValue().get());
    min = newSrdObj<TiInt>(num);
    max = newSrdObj<TiInt>(num);
    return true;
  }
  auto bracket = mulOp->getSecond().ti_cast_get<Core::Ast::Bracket>();
  if (bracket == 0 || bracket->getType() != Core::Ast::BracketType::ROUND) {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(mulOp->getSecond().get()))
    );
    return false;
  }
  auto list = bracket->getOperand().ti_cast_get<Core::Ast::List>();
  if (list == 0 || list->getCount() != 2) {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(bracket))
    );
    return false;
  }
  auto first = list->get(0).get();
  if (first != 0) {
    intLiteral = ti_cast<Core::Ast::IntegerLiteral>(first);
    if (intLiteral == 0) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(first))
      );
      return false;
    }
    auto num = std::stoi(intLiteral->getValue().get());
    min = newSrdObj<TiInt>(num);
  }
  auto second = list->get(1).get();
  if (second != 0) {
    intLiteral = ti_cast<Core::Ast::IntegerLiteral>(second);
    if (intLiteral == 0) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(second))
      );
      return false;
    }
    auto num = std::stoi(intLiteral->getValue().get());
    max = newSrdObj<TiInt>(num);
  }
  return true;
}


Bool parseQualifier(
  Core::Ast::Node *ast, Str &qualifier, Core::Notices::Store *noticeStore
) {
  if (ast->isDerivedFrom<Core::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Ast::Identifier*>(ast);
    qualifier += identifier->getValue().get();
  } else if (ast->isDerivedFrom<Core::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Ast::StringLiteral*>(ast);
    qualifier += stringLiteral->getValue().get();
  } else if (ast->isDerivedFrom<Core::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Ast::LinkOperator*>(ast);
    if (!parseQualifier(linkOp->getFirst().get(), qualifier, noticeStore)) return false;
    qualifier += S(".");
    if (!parseQualifier(linkOp->getSecond().get(), qualifier, noticeStore)) return false;
  } else {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidCommandDefAstNotice>(Core::Ast::findSourceLocation(ast))
    );
    return false;
  }
  return true;
}


void convertInfixOpIntoList(Core::Ast::Node *ast, Char const *op, Array<Core::Ast::Node*> &list)
{
  auto infixOp = ti_cast<Core::Ast::InfixOperator>(ast);
  if (infixOp == 0 || infixOp->getType() != op) {
    list.add(ast);
  } else {
    convertInfixOpIntoList(infixOp->getFirst().get(), op, list);
    convertInfixOpIntoList(infixOp->getSecond().get(), op, list);
  }
}


Bool overrideTree(
  TiObject *target, Str baseRefQualifier, Core::Ast::Node *qualifierAst, Core::Ast::Node *valueAst, Core::Notices::Store *noticeStore
) {
  if (qualifierAst->isDerivedFrom<Core::Ast::Identifier>()) {
    if (valueAst == 0) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(Core::Ast::findSourceLocation(qualifierAst))
      );
      return false;
    }
    auto identifier = static_cast<Core::Ast::Identifier*>(qualifierAst);
    TioSharedPtr value;
    if (!parseValueAst(valueAst, noticeStore, value)) return false;
    auto mapContaining = ti_cast<MapContaining<TiObject>>(target);
    if (mapContaining == 0) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(Core::Ast::findSourceLocation(qualifierAst))
      );
      return false;
    }
    mapContaining->setElement(identifier->getValue().get(), value.get());
  } else if (qualifierAst->isDerivedFrom<Core::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Ast::LinkOperator*>(qualifierAst);
    if (linkOp->getType() == S(":")) {
      if (!overrideTree(
        target, baseRefQualifier, linkOp->getFirst().get(), linkOp->getSecond().get(), noticeStore
      )) return false;
    } else if (linkOp->getType() == S(".")) {
      auto first = linkOp->getFirst().get();
      auto second = linkOp->getSecond().get();
      TiObject *clone;
      if (!cloneChain(target, first, baseRefQualifier, noticeStore, clone)) return false;
      if (!overrideTree(clone, baseRefQualifier, second, valueAst, noticeStore)) return false;
    } else {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(Core::Ast::findSourceLocation(qualifierAst))
      );
      return false;
    }
  } else if (qualifierAst->isDerivedFrom<Core::Ast::Scope>()) {
    auto scope = static_cast<Core::Ast::Scope*>(qualifierAst);
    for (Int i = 0; i < scope->getCount(); ++i) {
      if (!overrideTree(target, baseRefQualifier, scope->getElement(i), 0, noticeStore)) return false;
    }
  } else {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(Core::Ast::findSourceLocation(qualifierAst))
    );
    return false;
  }
  return true;
}


Bool cloneChain(
  TiObject *target, Core::Ast::Node *qualifier, Str &baseRefQualifier, Core::Notices::Store *noticeStore, TiObject *&result
) {
  if (qualifier->isDerivedFrom<Core::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Ast::Identifier*>(qualifier);
    auto mapContaining = ti_cast<MapContaining<TiObject>>(target);
    if (mapContaining == 0) {
      noticeStore->add(
        newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(Core::Ast::findSourceLocation(qualifier))
      );
      return false;
    }
    baseRefQualifier += S(".");
    baseRefQualifier += identifier->getValue();
    auto object = mapContaining->getElement(identifier->getValue());
    // We shouldn't inherit from the object that we are about to delete, instead, we should inherit from the upstream
    // object. If this object is not an Inheriting object then we just go ahead and clone it as that would be a mere
    // copy operation.
    auto inheriting = ti_cast<Inheriting>(object);
    auto clone = cloneInherited(inheriting == 0 ? object : inheriting->getBase());
    auto cloneBinding = ti_cast<Binding>(target);
    if (cloneBinding != 0) {
      cloneBinding->setMember(S("baseRef"), PARSE_REF(baseRefQualifier));
    }
    mapContaining->setElement(identifier->getValue().get(), clone.get());
    result = clone.get();
    return true;
  } else if (qualifier->isDerivedFrom<Core::Ast::LinkOperator>()) {
    auto linkOp = static_cast<Core::Ast::LinkOperator*>(qualifier);
    if (!cloneChain(target, linkOp->getFirst().get(), baseRefQualifier, noticeStore, target)) return false;
    if (!cloneChain(target, linkOp->getSecond().get(), baseRefQualifier, noticeStore, result)) return false;
    return true;
  } else {
    noticeStore->add(
      newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(Core::Ast::findSourceLocation(qualifier))
    );
    return false;
  }
}


Bool parseValueAst(Core::Ast::Node *valueAst, Core::Notices::Store *noticeStore, TioSharedPtr &result)
{
  if (
    valueAst->isDerivedFrom<Core::Ast::LinkOperator>() ||
    valueAst->isDerivedFrom<Core::Ast::Identifier>()
  ) {
    Str qualifier;
    if (!parseQualifier(valueAst, qualifier, noticeStore)) return false;
    result = PARSE_REF(qualifier);
    return true;
  } else if (valueAst->isDerivedFrom<Core::Ast::IntegerLiteral>()) {
    result = TiInt::create(std::stol(static_cast<Core::Ast::IntegerLiteral*>(valueAst)->getValue().get()));
    return true;
  } else if (valueAst->isDerivedFrom<Core::Ast::IntegerLiteral>()) {
    result = TiStr::create(static_cast<Core::Ast::StringLiteral*>(valueAst)->getValue());
    return true;
  } else if (valueAst->isDerivedFrom<Core::Ast::Scope>()) {
    auto scope = static_cast<Core::Ast::Scope*>(valueAst);
    auto map = Map::create();
    for (Int i = 0; i < scope->getCount(); ++i) {
      auto element = scope->get(i).get();
      auto linkOp = ti_cast<Core::Ast::LinkOperator>(element);
      if (linkOp == 0 || linkOp->getType() != S(":")) {
        noticeStore->add(
          newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(Core::Ast::findSourceLocation(element))
        );
        return false;
      }
      auto name = linkOp->getFirst().ti_cast_get<Core::Ast::Identifier>();
      if (name == 0) {
        noticeStore->add(newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(
          Core::Ast::findSourceLocation(linkOp->getFirst().get())
        ));
        return false;
      }
      TioSharedPtr value;
      if (!parseValueAst(linkOp->getSecond().get(), noticeStore, value)) return false;
      map->set(name->getValue().get(), value);
    }
    result = map;
    return true;
  } else {
    noticeStore->add(newSrdObj<Spp::Notices::InvalidGrammarAstNotice>(
      Core::Ast::findSourceLocation(valueAst)
    ));
    return false;
  }
}

} // namespace

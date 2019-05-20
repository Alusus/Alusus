/**
 * @file Core/Processing/Handlers/GenericParsingHandler.cpp
 * Contains the implementation of class Core::Processing::Handlers::GenericParsingHandler.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Processing::Handlers
{

using namespace Data;

//==============================================================================
// Overloaded Abstract Functions

void GenericParsingHandler::onProdEnd(Parser *parser, ParserState *state)
{
  Ast::MetaHaving *item = state->getData().ti_cast_get<Ast::MetaHaving>();
  Grammar::SymbolDefinition *prod = state->refTopProdLevel().getProd();
  if (item != 0 && item->getProdId() == UNKNOWN_ID) {
    // We need to set the production id now.
    this->prepareToModifyData(state, -1);
    item = state->getData().ti_cast_get<Ast::MetaHaving>();
    item->setProdId(prod->getId());
  } else if (this->isProdObjEnforced(state)) {
    // We need to create a container data object for this production root.
    SharedPtr<TiObject> data = this->createEnforcedProdNode(state);
    // Set the production id for this data item.
    Ast::MetaHaving *dataMeta = data.ti_cast_get<Ast::MetaHaving>();
    if (dataMeta == 0) {
      throw EXCEPTION(GenericException,
                      S("Production root objects must implement Ast::MetaHaving interface."));
    }
    dataMeta->setProdId(prod->getId());
    // Set the line and column, if any.
    if (item != 0) {
      dataMeta->setSourceLocation(item->findSourceLocation());
    }
    // Set the data to this production's state level.
    state->setData(data);
  }
}


void GenericParsingHandler::onTermEnd(Parser *parser, ParserState *state)
{
  // Skip if this term passes its data up.
  if (this->isRouteTerm(state, -1)) {
    if (!this->isRouteObjEnforced(state, -1)) return;
  } else if (this->isListTerm(state, -1)) {
    if (this->isPassUpList(state, -1)) return;
  }

  SharedPtr<TiObject> data = state->getData();
  Int levelIndex = -2;
  while (true) {
    if (this->isRouteTerm(state, levelIndex)) {
      if (this->isRouteObjEnforced(state, levelIndex) ||  state->isAProdRoot(levelIndex)) {
        this->addData(data, parser, state, levelIndex);
        return;
      } else {
        --levelIndex;
        continue;
      }
    } else if (this->isListTerm(state, levelIndex)) {
      if (data == 0 && !this->isListItemEnforced(state, levelIndex)) return;
      if (this->isPassUpList(state, levelIndex)) {
        // This data should be passed up.
        // But we shouldn't be at the root level already.
        if (state->isAProdRoot(levelIndex)) {
          throw EXCEPTION(GenericException, S("Pass-up list terms are not allowed at produciton roots."));
        }
        ASSERT(state->getData(levelIndex) == 0);
        --levelIndex;
        continue;
      } else {
        this->addData(data, parser, state, levelIndex);
      }
    }
    return;
  }
}


void GenericParsingHandler::onLevelExit(Parser *parser, ParserState *state,
                                        SharedPtr<TiObject> const &data)
{
  if (state->refTopTermLevel().getTerm()->isA<Grammar::ReferenceTerm>()) {
    ASSERT(state->getData() == 0);
    state->setData(data);
  }
}


void GenericParsingHandler::onNewToken(Parser *parser, ParserState *state,
                                       const Token *token)
{
  TiObject *matchText = state->getTokenTermText();
  // Skip if the term should be omitted.
  TiInt *flags = state->getTermFlags();
  if ((flags == 0 ? 0 : flags->get()) & ParsingFlags::ENFORCE_TOKEN_OMIT) return;
  else if (
    !((flags == 0 ? 0 : flags->get()) & ParsingFlags::ENFORCE_TOKEN_OBJ) &&
    matchText != 0 && matchText->isA<TiStr>()
  ) {
    return;
  }

  // We shouldn't have any data on this level at this point.
  ASSERT(state->getData() == 0);
  // Create a new item.

  Char const *tokenText = S("");
  // If the token term defines a map as its match criteria then we'll use the value of the matched
  // entry as the value of our Ast::Token text, otherwise we'll just use the actual token text
  // captured by the lexer.
  if (matchText != 0 && matchText->isA<Data::Grammar::Map>()) {
    TiObject *mappedText = static_cast<Data::Grammar::Map*>(matchText)->getElement(token->getText().c_str());
    if (mappedText != 0 && mappedText->isA<TiStr>()) {
      tokenText = static_cast<TiStr*>(mappedText)->get();
    } else {
      tokenText = token->getText().c_str();
    }
  } else {
    tokenText = token->getText().c_str();
  }
  // TODO: Implement control character parsing for character literals.

  // Create the token item.
  SharedPtr<TiObject> tokenItem = this->createTokenNode(state, -1, token->getId(), tokenText);
  auto metadata = tokenItem.ti_cast_get<Ast::MetaHaving>();
  if (metadata) {
    metadata->setSourceLocation(std::make_shared<Data::SourceLocationRecord>(token->getSourceLocation()));
  }
  state->setData(tokenItem);
}


void GenericParsingHandler::onConcatStep(Parser *parser, ParserState *state,
                                         Int newPos, Data::Token const *token)
{
  // If this term pass data up we can skip.
  if (this->isPassUpList(state, -1)) {
    // We shouldn't be at the root level.
    if (state->isAtProdRoot()) {
      throw EXCEPTION(GenericException, S("Passing items up is not allowed for produciton root list terms."));
    }
    if (this->isListObjEnforced(state, -1)) {
      throw EXCEPTION(GenericException, S("Enforcing list object cannot be used with pass-up terms."));
    }
    return;
  }

  // Should we create a list?
  if (this->isListObjEnforced(state, -1) && (state->getData() == 0)) {
    state->setData(this->createListNode(state, -1));
  }
}


void GenericParsingHandler::onAlternateRouteDecision(Parser *parser, ParserState *state,
                                                     Int route, Data::Token const *token)
{
  ASSERT(this->isRouteTerm(state, -1));
  if (!this->isRouteObjEnforced(state, -1)) return;

  ASSERT(state->getData() == 0);
  state->setData(this->createRouteNode(state, -1, route));
}


void GenericParsingHandler::onMultiplyRouteDecision(Parser *parser, ParserState *state,
                                                    Int route, Data::Token const *token)
{
  TiInt *min = state->getMultiplyTermMin();
  TiInt *max = state->getMultiplyTermMax();
  if ((min == 0 || min->get() == 0) && max != 0 && max->get() == 1) {
    // This is an optional term, so we'll just treat it the same way as alternate terms.
    this->onAlternateRouteDecision(parser, state, route, token);
  } else {
    // If this term pass data up we can skip.
    if (this->isPassUpList(state, -1)) {
      // We shouldn't be at the root level.
      if (state->isAtProdRoot()) {
        throw EXCEPTION(GenericException, S("Passing terms up is not allowed for produciton root list terms."));
      }
      if (this->isListObjEnforced(state, -1)) {
        throw EXCEPTION(GenericException, S("Enforcing list object cannot be used with pass-up terms."));
      }
      return;
    }

    // Should we create a list?
    if (this->isListObjEnforced(state, -1)) {
      state->setData(this->createListNode(state, -1));
    }
  }
}


void GenericParsingHandler::onTermCancelling(Parser *parser, ParserState *state)
{
  state->setData(SharedPtr<TiObject>(0));
}


void GenericParsingHandler::onProdCancelling(Parser *parser, ParserState *state)
{
  this->onTermCancelling(parser, state);
}


//==============================================================================
// Member Functions

void GenericParsingHandler::addData(
  SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex
) {
  if (this->isRouteTerm(state, levelIndex)) {
    TiObject *currentData = state->getData(levelIndex).get();
    auto container= ti_cast<Containing<TiObject>>(currentData);
    if (currentData == 0) {
      state->setData(data, levelIndex);
    } else if (container != 0 && container->getElement(0) == 0) {
      this->prepareToModifyData(state, levelIndex);
      container = state->getData(levelIndex).ti_cast_get<Containing<TiObject>>();
      container->setElement(0, data.get());
    } else {
      throw EXCEPTION(GenericException,
                      S("Trying to set data to an alternative or optional term that already has"
                        " data. Is a concat or duplicate term trying to pass-up multiple data "
                        "to an upper alternative or optional term?"));
    }
  } else if (this->isListTerm(state, levelIndex)) {
    // Add the given data to this list term.
    TiObject *currentData = state->getData(levelIndex).get();
    if (currentData == 0) {
      // Set the given data as the level's data.
      // If this term has FORCE_LIST flag the data should never be null in the first place.
      ASSERT(!this->isListObjEnforced(state, levelIndex));
      if (this->isListItemEnforced(state, levelIndex) && state->refTermLevel(levelIndex).getPosId() > 1) {
        // We have an enforced-item list, and this is not the first item in the list, so we'll create
        // a list whose first item is null.
        SharedPtr<TiObject> list = this->createListNode(state, levelIndex);
        auto newContainer = list.ti_cast_get<DynamicContaining<TiObject>>();
        Ast::MetaHaving *metadata = data.ti_cast_get<Ast::MetaHaving>();
        Ast::MetaHaving *newMetadata = list.ti_cast_get<Ast::MetaHaving>();
        if (newMetadata != 0 && metadata != 0) {
          newMetadata->setSourceLocation(metadata->findSourceLocation());
        }
        newContainer->addElement(currentData);
        newContainer->addElement(data.get());
        state->setData(list, levelIndex);
      } else {
        state->setData(data, levelIndex);
      }
    } else {
      // There is three possible situations at this point: Either the list was enforced, or
      // a child data was set into this level, or this level was visited more than once causing
      // a list to be created.
      auto container = ti_cast<DynamicContaining<TiObject>>(currentData);
      Ast::MetaHaving *metadata = ti_cast<Ast::MetaHaving>(currentData);
      if (container != 0 && (metadata == 0 || metadata->getProdId() == UNKNOWN_ID)) {
        // This level already has a list that belongs to this production, so we can just add the new data
        // to this list.
        this->prepareToModifyData(state, levelIndex);
        auto container = state->getData(levelIndex).ti_cast_get<DynamicContaining<TiObject>>();
        container->addElement(data.get());
      } else {
        // The term isn't a list, or it's a list that belongs to another production. So we'll create a new list.
        SharedPtr<TiObject> list = this->createListNode(state, levelIndex);
        auto newContainer = list.ti_cast_get<DynamicContaining<TiObject>>();
        Ast::MetaHaving *newMetadata = list.ti_cast_get<Ast::MetaHaving>();
        if (newMetadata != 0 && metadata != 0) {
          newMetadata->setSourceLocation(metadata->findSourceLocation());
        }
        newContainer->addElement(currentData);
        newContainer->addElement(data.get());
        state->setData(list, levelIndex);
      }
    }
  } else {
    // This should never be reachable.
    ASSERT(false);
  }
}


Bool GenericParsingHandler::isListObjEnforced(ParserState *state, Int levelIndex)
{
  TiInt *flags = state->getTermFlags(levelIndex);
  return (flags == 0 ? 0 : flags->get()) & ParsingFlags::ENFORCE_LIST_OBJ;
}


Bool GenericParsingHandler::isListItemEnforced(ParserState *state, Int levelIndex)
{
  TiInt *flags = state->getTermFlags(levelIndex);
  return (flags == 0 ? 0 : flags->get()) & ParsingFlags::ENFORCE_LIST_ITEM;
}


Bool GenericParsingHandler::isRouteObjEnforced(ParserState *state, Int levelIndex)
{
  TiInt *flags = state->getTermFlags(levelIndex);
  return (flags == 0 ? 0 : flags->get()) & ParsingFlags::ENFORCE_ROUTE_OBJ;
}


Bool GenericParsingHandler::isPassUpList(ParserState *state, Int levelIndex)
{
  TiInt *flags = state->getTermFlags(levelIndex);
  return (flags == 0 ? 0 : flags->get()) & ParsingFlags::PASS_ITEMS_UP;
}


Bool GenericParsingHandler::isProdObjEnforced(ParserState *state)
{
  TiInt *flags = state->getProdFlags();
  return (flags == 0 ? 0 : flags->get()) & ParsingFlags::ENFORCE_PROD_OBJ;
}


SharedPtr<TiObject> GenericParsingHandler::createListNode(ParserState *state, Int levelIndex)
{
  return std::make_shared<Ast::List>();
}


SharedPtr<TiObject> GenericParsingHandler::createRouteNode(ParserState *state, Int levelIndex, Int route)
{
  auto routeItem = std::make_shared<Ast::Route>();
  routeItem->setRoute(route);
  return routeItem;
}


SharedPtr<TiObject> GenericParsingHandler::createTokenNode(ParserState *state, Int levelIndex,
                                                                     Word tokenId, Char const *tokenText)
{
  auto token = std::make_shared<Ast::Token>();
  token->setId(tokenId);
  token->setText(tokenText);
  return token;
}


SharedPtr<TiObject> GenericParsingHandler::createEnforcedProdNode(ParserState *state)
{
  // We need to create a container data object for this production root.
  SharedPtr<TiObject> data;
  Grammar::Term *term = state->refTopTermLevel().getTerm();
  if (term->isA<Grammar::AlternateTerm>()) {
    data = this->createRouteNode(state, -1, -1);
    data.s_cast_get<Ast::Route>()->setData(state->getData());
  } else if (term->isA<Grammar::MultiplyTerm>()) {
    TiInt *min = state->getMultiplyTermMin();
    TiInt *max = state->getMultiplyTermMax();
    if ((min == 0 || min->get() == 0) && max != 0 && max->get() == 1) {
      // Optional term.
      data = this->createRouteNode(state, -1, -1);
      data.s_cast_get<Ast::Route>()->setData(state->getData());
    } else {
      // Duplicate term.
      data = this->createListNode(state, -1);
      if (state->getData() != 0 || this->isListItemEnforced(state, -1)) {
        data.s_cast_get<Ast::List>()->add(state->getData());
      }
    }
  } else if (term->isA<Grammar::ConcatTerm>()) {
    data = this->createListNode(state, -1);
    if (state->getData() != 0 || this->isListItemEnforced(state, -1)) {
      data.s_cast_get<Ast::List>()->add(state->getData());
    }
  } else if (term->isA<Grammar::ReferenceTerm>()) {
    throw EXCEPTION(GenericException,
                    S("Enforcing a production object on an alias production (a production that is merely a "
                      "reference to another production) is not allowed."));
  } else {
    // This should never be reachable.
    ASSERT(false);
  }
  return data;
}


Bool GenericParsingHandler::isRouteTerm(ParserState *state, Int levelIndex)
{
  ParserTermLevel &termLevel = state->refTermLevel(levelIndex);
  return termLevel.getTerm()->isA<Grammar::AlternateTerm>() ||
         (termLevel.getTerm()->isA<Grammar::MultiplyTerm>() &&
          (state->getMultiplyTermMin(levelIndex) == 0 || state->getMultiplyTermMin(levelIndex)->get() == 0) &&
          (state->getMultiplyTermMax(levelIndex) != 0 && state->getMultiplyTermMax(levelIndex)->get() == 1));
}


Bool GenericParsingHandler::isListTerm(ParserState *state, Int levelIndex)
{
  ParserTermLevel &termLevel = state->refTermLevel(levelIndex);
  return termLevel.getTerm()->isA<Grammar::MultiplyTerm>() || termLevel.getTerm()->isA<Grammar::ConcatTerm>();
}


void GenericParsingHandler::prepareToModifyData(ParserState *state, Int levelIndex)
{
  if (state->isDataShared(levelIndex)) {
    // Duplicate the data.
    auto data = state->getData(levelIndex).get();
    auto clonable = ti_cast<Clonable>(data);
    if (data != 0 && clonable == 0) {
      throw EXCEPTION(GenericException,
                      S("State branching requires that state data is Clonable."));
    }
    if (clonable) {
      state->setData(clonable->clone(), levelIndex);
    }
  }
}

} // namespace

/**
 * @file Core/Processing/GenericParsingHandler.cpp
 * Contains the implementation of class Core::Processing::GenericParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Processing
{

using namespace Data;

//==============================================================================
// Overloaded Abstract Functions

/**
 * Assign the produciton id to the production data. If the data is null or
 * belongs to another production and this production is not omissible, then this
 * function will create a new data item (of a type matching the root term type)
 * and assign the production to it after assigning the existing data as a child
 * of the newly created data.
 *
 * @sa ParsingHandler::onProdEnd()
 */
void GenericParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  ParsingMetadataHolder *item = state->getData().ii_cast_get<ParsingMetadataHolder>();
  SymbolDefinition *prod = state->refTopProdLevel().getProd();
  if (item != 0 && item->getProdId() == UNKNOWN_ID) {
    // We need to set the production id now.
    this->prepareToModifyData(state, -1);
    item = state->getData().ii_cast_get<ParsingMetadataHolder>();
    item->setProdId(prod->getId());
  } else if (prod->getFlags() & ParsingFlags::OMISSIBLE) {
    // We don't need to set the production id since it's omissible.
    return;
  } else {
    // We need to create a container data object for this production root.
    SharedPtr<IdentifiableObject> data;
    Term *term = state->refTopTermLevel().getTerm();
    if (term->isA<AlternateTerm>()) {
      data = std::make_shared<ParsedRoute>();
      data.s_cast_get<ParsedRoute>()->setData(state->getData());
    } else if (term->isA<MultiplyTerm>()) {
      Integer *min = state->getMultiplyTermMin();
      Integer *max = state->getMultiplyTermMax();
      if ((min == 0 || min->get() == 0) && max != 0 && max->get() == 1) {
        // Optional term.
        data = std::make_shared<ParsedRoute>();
        data.s_cast_get<ParsedRoute>()->setData(state->getData());
      } else {
        // Duplicate term.
        data = std::make_shared<ParsedList>();
        if (!(state->getData() == 0 &&
              (state->refTopTermLevel().getTerm()->getFlags() & ParsingFlags::OMISSIBLE))) {
          data.s_cast_get<ParsedList>()->add(state->getData());
        }
      }
    } else if (term->isA<ConcatTerm>()) {
      data = std::make_shared<ParsedList>();
      if (!(state->getData() == 0 &&
            (state->refTopTermLevel().getTerm()->getFlags() & ParsingFlags::OMISSIBLE))) {
        data.s_cast_get<ParsedList>()->add(state->getData());
      }
    } else if (term->isA<TokenTerm>()) {
      // This should never be reachable.
      ASSERT(false);
    } else if (term->isA<ReferenceTerm>()) {
      // Reference production are just aliases and will always be considered omissible.
      return;
    }
    // Set the production id for this data item.
    ParsingMetadataHolder *dataMeta = data.ii_cast_get<ParsingMetadataHolder>();
    dataMeta->setProdId(prod->getId());
    // Set the line and column, if any.
    if (item != 0) {
      dataMeta->setLine(item->getLine());
      dataMeta->setColumn(item->getColumn());
    }
    // Set the data to this production's state level.
    state->setData(data);
  }
}


/**
 * Passes the data of the top level to the level above it by calling
 * setChildData(). If the top level has the PASS_UP flag then we will skip
 * because the data would already be passed up by now.
 *
 * @sa setChildData()
 * @sa ParsingHandler::onTermEnd()
 */
void GenericParsingHandler::onTermEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  if (!(state->refTopTermLevel().getTerm()->getFlags() & ParsingFlags::PASS_UP)) {
    this->setChildData(state->getData(), state, -2);
  }
}


/**
 * Set the data of a reference term with the data received from the referenced
 * production. If the term type at the top state level is not reference this
 * function will do nothing.
 *
 * @sa ParsingHandler::onLevelExit()
 */
void GenericParsingHandler::onLevelExit(Processing::Parser *parser, Processing::ParserState *state,
                                        SharedPtr<IdentifiableObject> const &data)
{
  if (state->refTopTermLevel().getTerm()->isA<ReferenceTerm>()) {
    ASSERT(state->getData() == 0);
    state->setData(data);
  }
}


/**
 * Create a ParsedToken object and associate it with the current state level.
 * If the token term is omissible and the token is a constant token, no data
 * will be created.
 *
 * @sa ParsingHandler::onNewToken()
 */
void GenericParsingHandler::onNewToken(Processing::Parser *parser, Processing::ParserState *state,
                                       const Token *token)
{
  // Get the term object.
  TokenTerm *term = static_cast<TokenTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(term->isA<TokenTerm>());

  IdentifiableObject *matchText = state->getTokenTermText();
  // Skip if the term is omissible.
  if ((term->getFlags() & ParsingFlags::OMISSIBLE) && matchText != 0 && matchText->isA<Data::String>()) return;

  // We shouldn't have any data on this level if we are not a production.
  ASSERT(state->getData() == 0);
  // Create a new item.
  SharedPtr<ParsedToken> tokenItem = std::make_shared<ParsedToken>();
  state->setData(tokenItem);

  // Now we can modify the data of the token.
  tokenItem->setId(token->getId());
  tokenItem->setLine(token->getLine());
  tokenItem->setColumn(token->getColumn());
  // If the token term defines a map as its match criteria then we'll use the value of the matched
  // entry as the value of our ParsedToken text, otherwise we'll just use the actual token text
  // captured by the lexer.
  if (matchText != 0 && matchText->isA<Data::SharedMap>()) {
    IdentifiableObject *mappedText = static_cast<Data::SharedMap*>(matchText)->get(token->getText().c_str());
    if (mappedText != 0 && mappedText->isA<Data::String>()) {
      tokenItem->setText(static_cast<Data::String*>(mappedText)->get());
    } else {
      tokenItem->setText(token->getText().c_str());
    }
  } else {
    tokenItem->setText(token->getText().c_str());
  }
  // TODO: Implement control character parsing for character literals.
}


/**
 * Create a ParsedList object if this level's term has a FORCE_LIST parsing
 * flag set and no list is already created. If this term has the PASS_UP flag
 * nothing will be done.
 *
 * @sa ParsingHandler::onConcatStep()
 */
void GenericParsingHandler::onConcatStep(Processing::Parser *parser, Processing::ParserState *state,
                                         Int newPos)
{
  // Get the term object.
  ConcatTerm *term = static_cast<ConcatTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(term->isA<ConcatTerm>());

  // If this term pass data up we can skip.
  if (term->getFlags() & ParsingFlags::PASS_UP) {
    // We shouldn't be at the root level.
    if (state->isAtProdRoot()) {
      throw GeneralException(STR("PASS_UP parsing flag is not allowed for produciton root list terms."),
                             STR("Core::Standard::GenericParsingHandler::onConcatStep"));
    }
    if (term->getFlags() & ParsingFlags::FORCE_LIST) {
      throw GeneralException(STR("FORCE_LIST parsing flag cannot be used with PASS_UP"),
                             STR("Core::Standard::GenericParsingHandler::onConcatStep"));
    }
    return;
  }

  // Should we create a list?
  if ((term->getFlags() & ParsingFlags::FORCE_LIST) && (state->getData() == 0)) {
    state->setData(std::make_shared<ParsedList>());
  }
}


/**
 * Create a ParsedRoute object and associate it with the current state level
 * if this level's term is not PASS_UP flagged.
 *
 * @sa ParsingHandler::onAlternateRouteDecision()
 */
void GenericParsingHandler::onAlternateRouteDecision(Processing::Parser *parser, Processing::ParserState *state,
                                                     Int route)
{
  // Get the term object.
  Term *term = state->refTopTermLevel().getTerm();
  ASSERT(term->isA<AlternateTerm>() ||
         (term->isA<MultiplyTerm>() &&
          (state->getMultiplyTermMin()==0 || state->getMultiplyTermMin()->get() == 0) &&
          (state->getMultiplyTermMax()!=0 && state->getMultiplyTermMax()->get() == 1)));

  if (term->getFlags() & ParsingFlags::PASS_UP) return;

  ASSERT(state->getData() == 0);

  // Create a new item.
  SharedPtr<ParsedRoute> routeItem = std::make_shared<ParsedRoute>();
  state->setData(routeItem);

  // Now we can modify the data of the token.
  routeItem->setRoute(route);
}


/**
 * For optional multiply terms (min==0 && max==1):<br>
 * This function simply calls onAlternateRouteDecision since the
 * implementation for optional terms is similar to that of alternative terms.
 * <br>
 * For other multiply terms:<br>
 * Create a ParsedList object if this level's term has a FORCE_LIST parsing
 * flag set and no list is already created. If this term has the PASS_UP flag
 * nothing will be done.
 *
 * @sa ParsingHandler::onMultiplyRouteDecision()
 */
void GenericParsingHandler::onMultiplyRouteDecision(Processing::Parser *parser, Processing::ParserState *state,
                                                    Int route)
{
  // Get the term object.
  MultiplyTerm *term = static_cast<MultiplyTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(term->isA<MultiplyTerm>());

  Integer *min = state->getMultiplyTermMin();
  Integer *max = state->getMultiplyTermMax();
  if ((min == 0 || min->get() == 0) && max != 0 && max->get() == 1) {
    // This is an optional term, so we'll just treat it the same way as alternate terms.
    this->onAlternateRouteDecision(parser, state, route);
  } else {
    // If this term pass data up we can skip.
    if (term->getFlags() & ParsingFlags::PASS_UP) {
      // We shouldn't be at the root level.
      if (state->isAtProdRoot()) {
        throw GeneralException(STR("PASS_UP parsing flag is not allowed for produciton root list terms."),
                               STR("Core::Standard::GenericParsingHandler::onMultiplyRouteDecision"));
      }
      if (term->getFlags() & ParsingFlags::FORCE_LIST) {
        throw GeneralException(STR("FORCE_LIST parsing flag cannot be used with PASS_UP"),
                               STR("Core::Standard::GenericParsingHandler::onMultiplyRouteDecision"));
      }
      return;
    }

    // Should we create a list?
    if ((term->getFlags() & ParsingFlags::FORCE_LIST) && (state->getData() == 0)) {
      state->setData(std::make_shared<ParsedList>());
    }
  }
}


/**
 * Wipe out any generated data from the canceled term level.
 * @sa ParsingHandler::onTermCancelling()
 */
void GenericParsingHandler::onTermCancelling(Processing::Parser *parser, Processing::ParserState *state)
{
  state->setData(SharedPtr<IdentifiableObject>(0));
}


/**
 * Wipe out any generated data from the canceled production level.
 * This function will simply call onTermCancelling.
 * @sa ParsingHandler::onProdCancelling()
 */
void GenericParsingHandler::onProdCancelling(Processing::Parser *parser, Processing::ParserState *state)
{
  this->onTermCancelling(parser, state);
}


/**
 * Set the given data object to the specified state level. If the term at that
 * state level need to pass the data to the upper level (has the PASS_UP flag
 * or a route term with the OMISSIBLE flag) the function will call itself with
 * the index of the upper level, and will keep doing so recursively until it
 * hits the production's root or a term that can accept the new data.
 */
void GenericParsingHandler::setChildData(SharedPtr<IdentifiableObject> const &data, Processing::ParserState *state,
                                         Int levelIndex)
{
  // Get the state level.
  Processing::ParserTermLevel &termLevel = state->refTermLevel(levelIndex);

  if (termLevel.getTerm()->isA<AlternateTerm>() ||
      (termLevel.getTerm()->isA<MultiplyTerm>() &&
       (state->getMultiplyTermMin(levelIndex) == 0 || state->getMultiplyTermMin(levelIndex)->get() == 0) &&
       (state->getMultiplyTermMax(levelIndex) != 0 && state->getMultiplyTermMax(levelIndex)->get() == 1))) {
    if ((termLevel.getTerm()->getFlags() & ParsingFlags::FORCE_LIST)) {
      throw GeneralException(STR("FORCE_LIST parsing flag is not allowed for optional or "
                                 "alternative terms."),
                             STR("Core::Standard::GenericParsingHandler::setChildData"));
    }
    if (termLevel.getTerm()->getFlags() & ParsingFlags::PASS_UP) {
      // This is a pass up route based term, so set the data to the upper level directly.
      // But only if the data is not omissible in the first place.
      if ((data == 0) && (termLevel.getTerm()->getFlags() & ParsingFlags::OMISSIBLE)) return;
      if (state->getData(levelIndex) != 0) {
        throw GeneralException(STR("Trying to set data to an alternative or optional term that already has"
                                   " data. Is a concat or duplicate term trying to PASS_UP multiple data "
                                   "to an upper alternative or optional term?"),
                               STR("Core::Standard::GenericParsingHandler::setChildData"));
      }
      ASSERT(state->getData(levelIndex) == 0);
      if (state->isAProdRoot(levelIndex)) {
        // This is the root of the production, so we can't go up anymore.
        state->setData(data, levelIndex);
      } else {
        // Try the upper level.
        this->setChildData(data, state, levelIndex - 1);
      }
    } else {
      //This is not an omissible term, we'll set this as the data of this term.
      ASSERT(state->getData(levelIndex) != 0);
      ASSERT(state->getData(levelIndex).io_cast_get<ParsedRoute>() != 0);
      this->prepareToModifyData(state, levelIndex);
      ParsedRoute *route = state->getData(levelIndex).io_cast_get<ParsedRoute>();
      if (route->getData() != 0) {
        throw GeneralException(STR("Trying to set data to an alternative or optional term that already has"
                                   " data. Is a concat or duplicate term trying to PASS_UP multiple data "
                                   "to an upper alternative or optional term?"),
                               STR("Core::Standard::GenericParsingHandler::setChildData"));
      }
      route->setData(data);
    }
  } else if (termLevel.getTerm()->isA<MultiplyTerm>() ||
             termLevel.getTerm()->isA<ConcatTerm>()) {
    if ((termLevel.getTerm()->getFlags() & ParsingFlags::OMISSIBLE) &&
        (data == 0)) {
      // This is an omissible list based term and the data is null, so we can skip.
      return;
    }
    if (termLevel.getTerm()->getFlags() & ParsingFlags::PASS_UP) {
      // This data should be passed up.
      // But we shouldn't be at the root level already.
      if (state->isAProdRoot(levelIndex)) {
        throw GeneralException(STR("PASS_UP parsing flag is not allowed for produciton root list terms."),
                               STR("Core::Standard::GenericParsingHandler::setChildData"));
      }
      if (termLevel.getTerm()->getFlags() & ParsingFlags::FORCE_LIST) {
        throw GeneralException(STR("FORCE_LIST parsing flag cannot be used with PASS_UP"),
                               STR("Core::Standard::GenericParsingHandler::setChildData"));
      }
      ASSERT(state->getData(levelIndex) == 0);
      this->setChildData(data, state, levelIndex - 1);
    } else {
      // Add the given data to this list term.
      IdentifiableObject *currentData = state->getData(levelIndex).get();
      if (currentData == 0) {
        // Set the given data as the level's data.
        // If this term has FORCE_LIST flag the data should never be null in the first place.
        ASSERT((termLevel.getTerm()->getFlags() & ParsingFlags::FORCE_LIST) == 0);
        state->setData(data, levelIndex);
      } else if (io_cast<ParsedList>(currentData) != 0 &&
                 static_cast<ParsedList*>(currentData)->getProdId() == UNKNOWN_ID) {
        // This level already has a list that belongs to this production, so we can just add the new data
        // to this list.
        this->prepareToModifyData(state, levelIndex);
        currentData = state->getData(levelIndex).get();
        // The element is not created yet.
        static_cast<ParsedList*>(currentData)->add(data);
      } else {
        // The term isn't a list, or it's a list that belongs to another production. So we'll create a new list.
        ASSERT((termLevel.getTerm()->getFlags() & ParsingFlags::FORCE_LIST) == 0);
        SharedPtr<ParsedList> list = std::make_shared<ParsedList>();
        ParsingMetadataHolder *currentDataMeta = currentData->getInterface<ParsingMetadataHolder>();
        list->setLine(currentDataMeta->getLine());
        list->setColumn(currentDataMeta->getColumn());
        list->add(state->getData(levelIndex));
        list->add(data);
        state->setData(list, levelIndex);
      }
    }
  } else if (termLevel.getTerm()->isA<ReferenceTerm>() ||
             termLevel.getTerm()->isA<TokenTerm>()) {
    // This should never be reachable.
    ASSERT(false);
  }
}


/**
 * If the top level is shared (the shared pointer is not unique) this function
 * will duplicate that term.
 */
void GenericParsingHandler::prepareToModifyData(Processing::ParserState *state, Int levelIndex)
{
  if (state->isDataShared(levelIndex)) {
    // Duplicate the data.
    SharedPtr<IdentifiableObject> data = state->getData(levelIndex);
    if (data->isDerivedFrom<ParsedToken>()) {
      // Duplicate token data.
      ParsedToken *sourceToken = data.s_cast_get<ParsedToken>();
      SharedPtr<ParsedToken> newToken = std::make_shared<ParsedToken>();
      newToken->setProdId(sourceToken->getProdId());
      newToken->setLine(sourceToken->getLine());
      newToken->setColumn(sourceToken->getColumn());
      newToken->setId(sourceToken->getId());
      newToken->setText(sourceToken->getText().c_str());
      state->setData(newToken, levelIndex);
    } else if (data->isDerivedFrom<ParsedRoute>()) {
      // Duplicate route data.
      ParsedRoute *sourceRoute = data.s_cast_get<ParsedRoute>();
      SharedPtr<ParsedRoute> newRoute = std::make_shared<ParsedRoute>();
      newRoute->setProdId(sourceRoute->getProdId());
      newRoute->setLine(sourceRoute->getLine());
      newRoute->setColumn(sourceRoute->getColumn());
      newRoute->setRoute(sourceRoute->getRoute());
      newRoute->setData(sourceRoute->getData());
      state->setData(newRoute, levelIndex);
    } else if (data->isDerivedFrom<ParsedList>()) {
      // Duplicate list data.
      ParsedList *sourceList = data.s_cast_get<ParsedList>();
      SharedPtr<ParsedList> newList = std::make_shared<ParsedList>();
      newList->setProdId(sourceList->getProdId());
      newList->setLine(sourceList->getLine());
      newList->setColumn(sourceList->getColumn());
      for (Word i = 0; i < sourceList->getCount(); ++i) {
        newList->add(sourceList->get(i));
      }
      state->setData(newList, levelIndex);
    } else {
      // This should never be reachable.
      ASSERT(false);
    }
  }
}

} } // namespace

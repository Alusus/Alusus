/**
 * @file Core/Standard/GenericParsingHandler.cpp
 * Contains the implementation of class Standard::GenericParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Standard
{

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
void GenericParsingHandler::onProdEnd(Parser::StateMachine *machine, Parser::State *state)
{
  ParsedItem *item = state->refTopTermLevel().getData().io_cast_get<ParsedItem>();
  Data::SymbolDefinition *prod = state->refTopProdLevel().getProd();
  if (item != 0 && item->getProdId() == UNKNOWN_ID) {
    // We need to set the production id now.
    this->prepareToModifyData(&state->refTopTermLevel());
    item = state->refTopTermLevel().getData().s_cast_get<ParsedItem>();
    item->setProdId(prod->getId());
  } else if (prod->getFlags() & ParsingFlags::OMISSIBLE) {
    // We don't need to set the production id since it's omissible.
    return;
  } else {
    // We need to create a container data object for this production root.
    SharedPtr<ParsedItem> data;
    Data::Term *term = state->refTopTermLevel().getTerm();
    if (term->isA<Data::AlternateTerm>()) {
      data = std::make_shared<ParsedRoute>();
      data.s_cast_get<ParsedRoute>()->setData(state->refTopTermLevel().getData());
    } else if (term->isA<Data::MultiplyTerm>()) {
      Data::Integer *min = state->getMultiplyTermMin();
      Data::Integer *max = state->getMultiplyTermMax();
      if ((min == 0 || min->get() == 0) && max != 0 && max->get() == 1) {
        // Optional term.
        data = std::make_shared<ParsedRoute>();
        data.s_cast_get<ParsedRoute>()->setData(state->refTopTermLevel().getData());
      } else {
        // Duplicate term.
        data = std::make_shared<ParsedList>();
        if (!(state->refTopTermLevel().getData() == 0 &&
              (state->refTopTermLevel().getTerm()->getFlags() & ParsingFlags::OMISSIBLE))) {
          data.s_cast_get<ParsedList>()->pushElement(state->refTopTermLevel().getData());
        }
      }
    } else if (term->isA<Data::ConcatTerm>()) {
      data = std::make_shared<ParsedList>();
      if (!(state->refTopTermLevel().getData() == 0 &&
            (state->refTopTermLevel().getTerm()->getFlags() & ParsingFlags::OMISSIBLE))) {
        data.s_cast_get<ParsedList>()->pushElement(state->refTopTermLevel().getData());
      }
    } else if (term->isA<Data::TokenTerm>()) {
      // This should never be reachable.
      ASSERT(false);
    } else if (term->isA<Data::ReferenceTerm>()) {
      // Reference production are just aliases and will always be considered omissible.
      return;
    }
    // Set the production id for this data item.
    data->setProdId(prod->getId());
    // Set the line and column, if any.
    if (item != 0) {
      data->setLine(item->getLine());
      data->setColumn(item->getColumn());
    }
    // Set the data to this production's state level.
    state->refTopTermLevel().setData(data);
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
void GenericParsingHandler::onTermEnd(Parser::StateMachine *machine, Parser::State *state)
{
  if (!(state->refTopTermLevel().getTerm()->getFlags() & ParsingFlags::PASS_UP)) {
    this->setChildData(state->refTopTermLevel().getData(), state, -2);
  }
}


/**
 * Set the data of a reference term with the data received from the referenced
 * production. If the term type at the top state level is not reference this
 * function will do nothing.
 *
 * @sa ParsingHandler::onLevelExit()
 */
void GenericParsingHandler::onLevelExit(Parser::StateMachine *machine, Parser::State *state,
                                        const SharedPtr<IdentifiableObject> &data)
{
  if (state->refTopTermLevel().getTerm()->isA<Data::ReferenceTerm>()) {
    ASSERT(state->refTopTermLevel().getData() == 0);
    state->refTopTermLevel().setData(data);
  }
}


/**
 * Create a ParsedToken object and associate it with the current state level.
 * If the token term is omissible and the token is a constant token, no data
 * will be created.
 *
 * @sa ParsingHandler::onNewToken()
 */
void GenericParsingHandler::onNewToken(Parser::StateMachine *machine, Parser::State *state,
                                       const Common::Token *token)
{
  // Get the term object.
  Data::TokenTerm *term = static_cast<Data::TokenTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(term->isA<Data::TokenTerm>());

  // Skip if the term is omissible.
  if ((term->getFlags() & ParsingFlags::OMISSIBLE) && state->getTokenTermText() != 0) return;

  // We shouldn't have any data on this level if we are not a production.
  ASSERT(state->refTopTermLevel().getData() == 0);
  // Create a new item.
  SharedPtr<ParsedToken> tokenItem = std::make_shared<ParsedToken>();
  state->refTopTermLevel().setData(tokenItem);

  // Now we can modify the data of the token.
  tokenItem->setId(token->getId());
  tokenItem->setLine(token->getLine());
  tokenItem->setColumn(token->getColumn());
  // If we have a string literal then we should parse escape sequences.
  static Word stringLiteralId = 0;
  if (stringLiteralId == 0) {
    stringLiteralId = ID_GENERATOR->getId(STR("STRING_LITERAL_TOKEN"));
  }
  if (token->getId() == stringLiteralId) {
    Str txt;
    this->parseStringLiteralControlCharacters(token->getText(), txt);
    tokenItem->setText(txt.c_str());
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
void GenericParsingHandler::onConcatStep(Parser::StateMachine *machine, Parser::State *state,
                                         Int newPos)
{
  // Get the term object.
  Data::ConcatTerm *term = static_cast<Data::ConcatTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(term->isA<Data::ConcatTerm>());

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
  if ((term->getFlags() & ParsingFlags::FORCE_LIST) && (state->refTopTermLevel().getData() == 0)) {
    state->refTopTermLevel().setData(std::make_shared<ParsedList>());
  }
}


/**
 * Create a ParsedRoute object and associate it with the current state level
 * if this level's term is not PASS_UP flagged.
 *
 * @sa ParsingHandler::onAlternateRouteDecision()
 */
void GenericParsingHandler::onAlternateRouteDecision(Parser::StateMachine *machine, Parser::State *state,
                                                     Int route)
{
  // Get the term object.
  Data::Term *term = state->refTopTermLevel().getTerm();
  ASSERT(term->isA<Data::AlternateTerm>() ||
         (term->isA<Data::MultiplyTerm>() &&
          (state->getMultiplyTermMin()==0 || state->getMultiplyTermMin()->get() == 0) &&
          (state->getMultiplyTermMax()!=0 && state->getMultiplyTermMax()->get() == 1)));

  if (term->getFlags() & ParsingFlags::PASS_UP) return;

  ASSERT(state->refTopTermLevel().getData() == 0);

  // Create a new item.
  SharedPtr<ParsedRoute> routeItem = std::make_shared<ParsedRoute>();
  state->refTopTermLevel().setData(routeItem);

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
void GenericParsingHandler::onMultiplyRouteDecision(Parser::StateMachine *machine, Parser::State *state,
                                                    Int route)
{
  // Get the term object.
  Data::MultiplyTerm *term = static_cast<Data::MultiplyTerm*>(state->refTopTermLevel().getTerm());
  ASSERT(term->isA<Data::MultiplyTerm>());

  Data::Integer *min = state->getMultiplyTermMin();
  Data::Integer *max = state->getMultiplyTermMax();
  if ((min == 0 || min->get() == 0) && max != 0 && max->get() == 1) {
    // This is an optional term, so we'll just treat it the same way as alternate terms.
    this->onAlternateRouteDecision(machine, state, route);
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
    if ((term->getFlags() & ParsingFlags::FORCE_LIST) && (state->refTopTermLevel().getData() == 0)) {
      state->refTopTermLevel().setData(std::make_shared<ParsedList>());
    }
  }
}


/**
 * Wipe out any generated data from the canceled term level.
 * @sa ParsingHandler::onTermCancelling()
 */
void GenericParsingHandler::onTermCancelling(Parser::StateMachine *machine, Parser::State *state)
{
  state->refTopTermLevel().setData(SharedPtr<IdentifiableObject>(0));
}


/**
 * Wipe out any generated data from the canceled production level.
 * This function will simply call onTermCancelling.
 * @sa ParsingHandler::onProdCancelling()
 */
void GenericParsingHandler::onProdCancelling(Parser::StateMachine *machine, Parser::State *state)
{
  this->onTermCancelling(machine, state);
}


/**
 * Set the given data object to the specified state level. If the term at that
 * state level need to pass the data to the upper level (has the PASS_UP flag
 * or a route term with the OMISSIBLE flag) the function will call itself with
 * the index of the upper level, and will keep doing so recursively until it
 * hits the production's root or a term that can accept the new data.
 */
void GenericParsingHandler::setChildData(const SharedPtr<IdentifiableObject> &data, Parser::State *state,
                                         Int levelIndex)
{
  // Get the state level.
  Parser::TermLevel &termLevel = state->refTermLevel(levelIndex);

  if (termLevel.getTerm()->isA<Data::AlternateTerm>() ||
      (termLevel.getTerm()->isA<Data::MultiplyTerm>() &&
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
      if (termLevel.getData() != 0) {
        throw GeneralException(STR("Trying to set data to an alternative or optional term that already has"
                                   " data. Is a concat or duplicate term trying to PASS_UP multiple data "
                                   "to an upper alternative or optional term?"),
                               STR("Core::Standard::GenericParsingHandler::setChildData"));
      }
      ASSERT(termLevel.getData() == 0);
      if (state->isAProdRoot(levelIndex)) {
        // This is the root of the production, so we can't go up anymore.
        termLevel.setData(data);
      } else {
        // Try the upper level.
        this->setChildData(data, state, levelIndex - 1);
      }
    } else {
      //This is not an omissible term, we'll set this as the data of this term.
      ASSERT(termLevel.getData() != 0);
      ASSERT(termLevel.getData().io_cast_get<ParsedRoute>() != 0);
      this->prepareToModifyData(&termLevel);
      ParsedRoute *route = termLevel.getData().io_cast_get<ParsedRoute>();
      if (route->getData() != 0) {
        throw GeneralException(STR("Trying to set data to an alternative or optional term that already has"
                                   " data. Is a concat or duplicate term trying to PASS_UP multiple data "
                                   "to an upper alternative or optional term?"),
                               STR("Core::Standard::GenericParsingHandler::setChildData"));
      }
      route->setData(data);
    }
  } else if (termLevel.getTerm()->isA<Data::MultiplyTerm>() ||
             termLevel.getTerm()->isA<Data::ConcatTerm>()) {
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
      ASSERT(termLevel.getData() == 0);
      this->setChildData(data, state, levelIndex - 1);
    } else {
      // Add the given data to this list term.
      IdentifiableObject *currentData = termLevel.getData().get();
      if (currentData == 0) {
        // Set the given data as the level's data.
        // If this term has FORCE_LIST flag the data should never be null in the first place.
        ASSERT((termLevel.getTerm()->getFlags() & ParsingFlags::FORCE_LIST) == 0);
        termLevel.setData(data);
      } else if (io_cast<ParsedList>(currentData) != 0 &&
                 static_cast<ParsedList*>(currentData)->getProdId() == UNKNOWN_ID) {
        // This level already has a list that belongs to this production, so we can just add the new data
        // to this list.
        this->prepareToModifyData(&termLevel);
        currentData = termLevel.getData().get();
        // The element is not created yet.
        static_cast<ParsedList*>(currentData)->pushElement(data);
      } else {
        // The term isn't a list, or it's a list that belongs to another production. So we'll create a new list.
        ASSERT((termLevel.getTerm()->getFlags() & ParsingFlags::FORCE_LIST) == 0);
        SharedPtr<ParsedList> list = std::make_shared<ParsedList>();
        list->setLine(static_cast<ParsedItem*>(currentData)->getLine());
        list->setColumn(static_cast<ParsedItem*>(currentData)->getColumn());
        list->pushElement(termLevel.getData());
        list->pushElement(data);
        termLevel.setData(list);
      }
    }
  } else if (termLevel.getTerm()->isA<Data::ReferenceTerm>() ||
             termLevel.getTerm()->isA<Data::TokenTerm>()) {
    // This should never be reachable.
    ASSERT(false);
  }
}


/**
 * Conver a string literal text to the actual string by converting all control
 * characters liked escaped sequences to the actual characters they represent.
 * Also merges separate string literal segments together.
 */
void GenericParsingHandler::parseStringLiteralControlCharacters(const Str &src, Str &dest)
{
  Word i = 0;
  Bool inStr = false;
  dest.reserve(src.size());
  while (i < src.size()) {
    if (inStr) {
      if (src[i] == '"') inStr = false;
      else if (src[i] == '\\') {
        ++i;
        if (src[i] == 'n') dest.push_back('\n');
        else if (src[i] == 'r') dest.push_back('\r');
        else if (src[i] == 't') dest.push_back('\t');
        // TODO: Parse other escape sequences.
      } else {
        dest.push_back(src[i]);
      }
    } else {
      if (src[i] == '"') inStr = true;
    }
    ++i;
  }
}


/**
 * If the top level is shared (the shared pointer is not unique) this function
 * will duplicate that term.
 */
void GenericParsingHandler::prepareToModifyData(Parser::TermLevel *level)
{
  if (level->isDataShared()) {
    // Duplicate the data.
    SharedPtr<IdentifiableObject> data = level->getData();
    if (data->isDerivedFrom<ParsedToken>()) {
      // Duplicate token data.
      ParsedToken *sourceToken = data.s_cast_get<ParsedToken>();
      SharedPtr<ParsedToken> newToken = std::make_shared<ParsedToken>();
      newToken->setProdId(sourceToken->getProdId());
      newToken->setLine(sourceToken->getLine());
      newToken->setColumn(sourceToken->getColumn());
      newToken->setId(sourceToken->getId());
      newToken->setText(sourceToken->getText().c_str());
      level->setData(newToken);
    } else if (data->isDerivedFrom<ParsedRoute>()) {
      // Duplicate route data.
      ParsedRoute *sourceRoute = data.s_cast_get<ParsedRoute>();
      SharedPtr<ParsedRoute> newRoute = std::make_shared<ParsedRoute>();
      newRoute->setProdId(sourceRoute->getProdId());
      newRoute->setLine(sourceRoute->getLine());
      newRoute->setColumn(sourceRoute->getColumn());
      newRoute->setRoute(sourceRoute->getRoute());
      newRoute->setData(sourceRoute->getData());
      level->setData(newRoute);
    } else if (data->isDerivedFrom<ParsedList>()) {
      // Duplicate list data.
      ParsedList *sourceList = data.s_cast_get<ParsedList>();
      SharedPtr<ParsedList> newList = std::make_shared<ParsedList>();
      newList->setProdId(sourceList->getProdId());
      newList->setLine(sourceList->getLine());
      newList->setColumn(sourceList->getColumn());
      for (Word i = 0; i < sourceList->getElementCount(); ++i) {
        newList->pushElement(sourceList->getElement(i));
      }
      level->setData(newList);
    } else {
      // This should never be reachable.
      ASSERT(false);
    }
  }
}

} } // namespace

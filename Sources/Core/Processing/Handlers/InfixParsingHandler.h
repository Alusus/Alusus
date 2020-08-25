/**
 * @file Core/Processing/Handlers/InfixParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::InfixParsingHandler
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_INFIXPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_INFIXPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

template <class TYPE> class InfixParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(InfixParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org",
                     (TYPE));


  //============================================================================
  // Member Variables

  private: Bool forward;


  //============================================================================
  // Constructor

  public: InfixParsingHandler(Bool f) : forward(f)
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<TiObject> const &data, Parser *parser, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      SharedPtr<TiObject> currentData = state->getData(levelIndex);
      if (currentData != 0) {
        // Either a child data was set into this level, or this level was visited more than once
        // causing an infix obj to be created.
        if (
          this->forward && currentData->isDerivedFrom<TYPE>() &&
          currentData.s_cast_get<TYPE>()->getProdId() == UNKNOWN_ID
        ) {
          // We want to attach the new object to the right leaf of the already
          // existing infix tree.
          auto leaf = this->findRightLeaf(currentData.s_cast_get<TYPE>());
          leaf->setSecond(this->createInfixObj(leaf->getSecond(), data));
        } else {
          state->setData(this->createInfixObj(currentData, data), levelIndex);
        }
        return;
      }
    }
    GenericParsingHandler::addData(data, parser, state, levelIndex);
  }

  protected: virtual Bool isListObjEnforced(ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex)) return false;
    else return GenericParsingHandler::isListObjEnforced(state, levelIndex);
  }

  private: TYPE* findRightLeaf(TYPE *obj)
  {
    TYPE *leaf = 0;
    while (obj != 0 && obj->getProdId() == UNKNOWN_ID) {
      leaf = obj;
      obj = obj->getSecond().template ti_cast_get<TYPE>();
    }
    return leaf;
  }

  private: SharedPtr<TYPE> createInfixObj(SharedPtr<TiObject> const &currentData,
                                          SharedPtr<TiObject> const &data)
  {
    Data::Ast::List *list = data.ti_cast_get<Data::Ast::List>();
    // `list` may be null in cases of errors causing drop of levels.
    auto token = ti_cast<Data::Ast::Token>(list != 0 ? list->getElement(0) : data.get());
    if (token == 0) {
      throw EXCEPTION(InvalidArgumentException, S("data[0]"), S("Invalid op token object received."),
                      list->get(0)->getMyTypeInfo()->getUniqueName());
    }

    auto obj = newSrdObj<TYPE>();
    obj->setFirst(currentData);
    obj->setType(token->getText());
    obj->setSecond(list != 0 ? list->get(1) : TioSharedPtr::null);

    auto metadata = currentData.ti_cast_get<Data::Ast::MetaHaving>();
    if (metadata != 0) {
      obj->setSourceLocation(metadata->findSourceLocation());
    }
    return obj;
  }

  private: SharedPtr<TiObject> cloneInfixTree(SharedPtr<TiObject> const &obj)
  {
    TYPE *infixObj = obj.ti_cast_get<TYPE>();
    if (infixObj == 0 || infixObj->getProdId() != UNKNOWN_ID) return obj;

    SharedPtr<TYPE> newObj = newSrdObj<TYPE>();
    newObj->setFirst(this->cloneInfixTree(infixObj->getFirst()));
    newObj->setSecond(this->cloneInfixTree(infixObj->getSecond()));
    newObj->setType(infixObj->getType());
    newObj->setSourceLocation(infixObj->findSourceLocation());
    return newObj;
  }

}; // class

} } } // namespace

#endif

/**
 * @file Core/Processing/Handlers/InfixParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::InfixParsingHandler
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  TEMPLATE_TYPE_INFO(InfixParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     TYPE);


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

  protected: virtual void addData(SharedPtr<IdentifiableObject> const &data, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      SharedPtr<IdentifiableObject> currentData = state->getData(levelIndex);
      if (currentData != 0) {
        // Either a child data was set into this level, or this level was visited more than once
        // causing an infix obj to be created.
        if (this->forward && state->refTermLevel(levelIndex).getPosId() > 2) {
          // We want to attach the new object to the right leaf of the already
          // existing infix tree.
          this->prepareToModifyInfixTree(state, levelIndex);
          auto leaf = this->findRightLeaf(state->getData(levelIndex).s_cast_get<TYPE>());
          leaf->setRight(this->createInfixObj(leaf->getRight(), data));
        } else {
          state->setData(this->createInfixObj(currentData, data), levelIndex);
        }
        return;
      }
    }
    GenericParsingHandler::addData(data, state, levelIndex);
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
      obj = obj->getRight().template io_cast_get<TYPE>();
    }
    return leaf;
  }

  private: SharedPtr<TYPE> createInfixObj(SharedPtr<IdentifiableObject> const &currentData,
                                          SharedPtr<IdentifiableObject> const &data)
  {
    Data::PrtList *list = data.io_cast_get<Data::PrtList>();
    if (list == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("data"), STR("Invalid object type received."),
                      data->getMyTypeInfo()->getUniqueName());
    }
    auto token = list->get(0).template io_cast_get<Data::PrtToken>();
    if (token == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("data[0]"), STR("Invalid op token object received."),
                      list->get(0)->getMyTypeInfo()->getUniqueName());
    }

    auto obj = std::make_shared<TYPE>();
    obj->setLeft(currentData);
    obj->setType(token->getText().c_str());
    obj->setRight(list->get(1));

    auto metadata = currentData.ii_cast_get<Data::ParsingMetadataHolder>();
    if (metadata != 0) {
      obj->setSourceLocation(metadata->getSourceLocation());
    }
    return obj;
  }

  /// Copy the entire infix list tree, not just its root.
  private: void prepareToModifyInfixTree(Processing::ParserState *state, Int levelIndex)
  {
    if (state->isDataShared(levelIndex)) {
      // Duplicate the data.
      auto data = state->getData(levelIndex);
      state->setData(this->cloneInfixTree(data), levelIndex);
    }
  }

  private: SharedPtr<IdentifiableObject> cloneInfixTree(SharedPtr<IdentifiableObject> const &obj)
  {
    TYPE *infixObj = obj.io_cast_get<TYPE>();
    if (infixObj == 0 || infixObj->getProdId() != UNKNOWN_ID) return obj;

    SharedPtr<TYPE> newObj = std::make_shared<TYPE>();
    newObj->setLeft(this->cloneInfixTree(infixObj->getLeft()));
    newObj->setRight(this->cloneInfixTree(infixObj->getRight()));
    newObj->setType(infixObj->getType());
    newObj->setSourceLocation(infixObj->getSourceLocation());
    return newObj;
  }

}; // class

} } } // namespace

#endif

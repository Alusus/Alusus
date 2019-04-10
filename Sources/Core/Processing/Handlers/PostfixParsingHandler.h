/**
 * @file Core/Processing/Handlers/PostfixParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::PostfixParsingHandler
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_POSTFIXPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_POSTFIXPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

template <class TYPE> class PostfixParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(PostfixParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     (TYPE));


  //============================================================================
  // Constructor

  public: PostfixParsingHandler<TYPE>()
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      SharedPtr<TiObject> currentData = state->getData(levelIndex);
      if (currentData != 0) {
        state->setData(this->createPostfixObj(currentData, data), levelIndex);
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

  private: SharedPtr<TYPE> createPostfixObj(TioSharedPtr const &currentData,
                                            TioSharedPtr const &data)
  {
    auto token = data.ti_cast_get<Data::Ast::Token>();
    if (token == 0) {
      throw EXCEPTION(InvalidArgumentException, S("data"), S("Invalid op token object received."),
                      currentData->getMyTypeInfo()->getUniqueName());
    }

    auto obj = std::make_shared<TYPE>();
    obj->setOperand(currentData);
    obj->setType(token->getText());

    auto metadata = currentData.ti_cast_get<Data::Ast::MetaHaving>();
    if (metadata != 0) {
      obj->setSourceLocation(metadata->findSourceLocation());
    }
    return obj;
  }

}; // class

} } } // namespace

#endif

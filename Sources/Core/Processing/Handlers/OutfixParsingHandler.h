/**
 * @file Core/Processing/Handlers/OutfixParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::OutfixParsingHandler
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_OUTFIXPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_OUTFIXPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

template <class PREFIXTYPE, class POSTFIXTYPE>
  class OutfixParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(OutfixParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     (PREFIXTYPE, POSTFIXTYPE));


  //============================================================================
  // Member Variables

  private: Bool forward;


  //============================================================================
  // Constructor

  public: OutfixParsingHandler(Bool f = false) : forward(f)
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<TiObject> const &data, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      SharedPtr<TiObject> currentData = state->getData(levelIndex);
      if (currentData != 0) {
        if (state->refTermLevel(levelIndex).getPosId() == 2) {
          // Attach prefix
          state->setData(this->createPrefixObj(currentData, data), levelIndex);
        } else if (state->refTermLevel(levelIndex).getPosId() == 3) {
          // Attach postfix
          if (this->forward && currentData->isA<PREFIXTYPE>()) {
            this->prepareToModifyData(state, levelIndex);
            auto prefix = state->getData(levelIndex).s_cast_get<PREFIXTYPE>();
            prefix->setOperand(this->createPostfixObj(prefix->getOperand(), data));
          } else {
            state->setData(this->createPostfixObj(currentData, data), levelIndex);
          }
        } else {
          throw EXCEPTION(GenericException, S("Unexpected term length"));
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

  private: SharedPtr<PREFIXTYPE> createPrefixObj(TioSharedPtr const &currentData,
                                                 TioSharedPtr const &data)
  {
    auto token = currentData.ti_cast_get<Data::Ast::Token>();
    if (token == 0) {
      throw EXCEPTION(InvalidArgumentException, S("currentData"), S("Invalid op token object received."),
                      currentData->getMyTypeInfo()->getUniqueName());
    }

    auto obj = std::make_shared<PREFIXTYPE>();
    obj->setOperand(data);
    obj->setType(token->getText());

    auto metadata = currentData.ti_cast_get<Data::Ast::MetaHaving>();
    if (metadata != 0) {
      obj->setSourceLocation(metadata->findSourceLocation());
    }
    return obj;
  }

  private: SharedPtr<POSTFIXTYPE> createPostfixObj(TioSharedPtr const &currentData,
                                                   TioSharedPtr const &data)
  {
    auto token = data.ti_cast_get<Data::Ast::Token>();
    if (token == 0) {
      throw EXCEPTION(InvalidArgumentException, S("data"), S("Invalid op token object received."),
                      currentData->getMyTypeInfo()->getUniqueName());
    }

    auto obj = std::make_shared<POSTFIXTYPE>();
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

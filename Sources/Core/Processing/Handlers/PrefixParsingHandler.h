/**
 * @file Core/Processing/Handlers/PrefixParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::PrefixParsingHandler
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_PREFIXPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_PREFIXPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

template <class TYPE> class PrefixParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(PrefixParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org",
                     (TYPE));


  //============================================================================
  // Constructor

  public: PrefixParsingHandler<TYPE>()
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<Ast::Node> const &data, Parser *parser, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      auto currentData = state->getData(levelIndex);
      if (currentData != 0) {
        state->setData(this->createPrefixObj(currentData, data), levelIndex);
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

  private: SharedPtr<TYPE> createPrefixObj(SharedPtr<Core::Ast::Node> const &currentData,
                                           SharedPtr<Core::Ast::Node> const &data)
  {
    auto token = currentData.ti_cast_get<Ast::Token>();
    if (token == 0) {
      throw EXCEPTION(InvalidArgumentException, S("currentData"), S("Invalid op token object received."),
                      currentData->getMyTypeInfo()->getUniqueName());
    }

    auto obj = newSrdObj<TYPE>();
    obj->setOperand(data);
    obj->setType(token->getText());

    obj->setSourceLocation(currentData->findSourceLocation());
    return obj;
  }

}; // class

} } } // namespace

#endif

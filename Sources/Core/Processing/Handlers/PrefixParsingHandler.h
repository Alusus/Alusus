/**
 * @file Core/Processing/Handlers/PrefixParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::PrefixParsingHandler
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  TEMPLATE_TYPE_INFO(PrefixParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     TYPE);


  //============================================================================
  // Constructor

  public: PrefixParsingHandler<TYPE>()
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual void addData(SharedPtr<IdentifiableObject> const &data, ParserState *state, Int levelIndex)
  {
    if (state->isAProdRoot(levelIndex) && this->isListTerm(state, levelIndex)) {
      SharedPtr<IdentifiableObject> currentData = state->getData(levelIndex);
      if (currentData != 0) {
        state->setData(this->createPrefixObj(currentData, data), levelIndex);
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

  private: SharedPtr<TYPE> createPrefixObj(SharedIoPtr const &currentData,
                                           SharedIoPtr const &data)
  {
    auto token = currentData.io_cast_get<Data::PrtToken>();
    if (token == 0) {
      throw EXCEPTION(InvalidArgumentException, STR("currentData"), STR("Invalid op token object received."),
                      currentData->getMyTypeInfo()->getUniqueName());
    }

    auto obj = std::make_shared<TYPE>();
    obj->setOperand(data);
    obj->setType(token->getText().c_str());

    auto metadata = currentData.ii_cast_get<Data::ParsingMetadataHolder>();
    if (metadata != 0) {
      obj->setSourceLocation(metadata->getSourceLocation());
    }
    return obj;
  }

}; // class

} } } // namespace

#endif

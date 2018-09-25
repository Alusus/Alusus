/**
 * @file Core/Processing/Handlers/TextParsingHandler.h
 * Contains the header of class Core::Processing::Handlers::TextParsingHandler
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_TEXTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_TEXTPARSINGHANDLER_H

namespace Core { namespace Processing { namespace Handlers
{

// TODO: DOC

template <class TYPE> class TextParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TextParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.net",
                     (TYPE));


  //============================================================================
  // Constructor

  public: TextParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  protected: virtual SharedPtr<TiObject> createTokenNode(ParserState *state, Int levelIndex,
                                                         Word tokenId, Char const *tokenText)
  {
    auto obj = std::make_shared<TYPE>();
    obj->setValue(tokenText);
    return obj;
  }

}; // class

} } } // namespace

#endif

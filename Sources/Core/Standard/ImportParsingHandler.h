/**
 * @file Core/Standard/ImportParsingHandler.h
 * Contains the header of class Core::Standard::ImportParsingHandler.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_IMPORTPARSINGHANDLER_H
#define CORE_STANDARD_IMPORTPARSINGHANDLER_H

namespace Core { namespace Standard
{

// TODO: DOC

class ImportParsingHandler : public Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ImportParsingHandler, Processing::Handlers::GenericParsingHandler, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: RootManager *rootManager;


  //============================================================================
  // Constructor

  public: ImportParsingHandler(RootManager *r) : rootManager(r)
  {
  }


  //============================================================================
  // Member Functions

  /// Load the referenced library.
  public: virtual void onProdEnd(Processing::Parser *parser, Processing::ParserState *state);

  private: Bool import(Char const *filename, Processing::ParserState *state, Str &errorDetails);

}; // class

} } // namespace

#endif

/**
 * @file Core/Processing/Handlers/ImportParsingHandler.h
 * Contains the header of class Core::Processing::Handleres::ImportParsingHandler.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_IMPORTPARSINGHANDLER_H
#define CORE_PROCESSING_HANDLERS_IMPORTPARSINGHANDLER_H

namespace Core::Processing::Handlers
{

// TODO: DOC

class ImportParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ImportParsingHandler, GenericParsingHandler, "Core.Processing.Handlers", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Main::RootManager *rootManager;


  //============================================================================
  // Constructor

  public: ImportParsingHandler(Main::RootManager *r) : rootManager(r)
  {
  }


  //============================================================================
  // Member Functions

  /// Load the referenced library.
  public: virtual void onProdEnd(Parser *parser, ParserState *state);

  private: Int tryImport(TiObject *astNode, Str &filenames, Str &errorDetails, ParserState *state);

}; // class

} // namespace

#endif

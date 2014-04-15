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

#ifndef STANDARD_IMPORT_PARSING_HANDLER_H
#define STANDARD_IMPORT_PARSING_HANDLER_H

namespace Core { namespace Standard
{

// TODO: DOC

class ImportParsingHandler : public GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(ImportParsingHandler, GenericParsingHandler, "Core.Standard", "Core", "alusus.net");


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
  public: virtual void onProdEnd(Parser::StateMachine *machine, Parser::State *state);

  private: Bool import(const Char *filename, Parser::State *state);

}; // class

} } // namespace

#endif

/**
 * @file Spp/Handlers/CodeGenParsingHandler.h
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_CODEGENPARSINGHANDLER_H
#define SPP_HANDLERS_CODEGENPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class CodeGenParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(CodeGenParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  Core::Standard::RootManager *rootManager;
  Spp::LlvmCodeGen::Generator *llvmGenerator;
  Bool execute;


  //============================================================================
  // Constructor

  public: CodeGenParsingHandler(Core::Standard::RootManager *rm, LlvmCodeGen::Generator *generator, Bool exe)
    : rootManager(rm), llvmGenerator(generator), execute(exe)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

}; // class

} } // namespace

#endif

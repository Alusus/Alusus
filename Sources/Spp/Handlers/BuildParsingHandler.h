/**
 * @file Spp/Handlers/BuildParsingHandler.h
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_BUILDPARSINGHANDLER_H
#define SPP_HANDLERS_BUILDPARSINGHANDLER_H

namespace Spp { namespace Handlers
{

class BuildParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  Core::Standard::RootManager *rootManager;
  Spp::LlvmCodeGen::Generator *llvmGenerator;


  //============================================================================
  // Constructor

  public: BuildParsingHandler(Core::Standard::RootManager *rm, LlvmCodeGen::Generator *generator)
    : rootManager(rm), llvmGenerator(generator)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

}; // class

} } // namespace

#endif

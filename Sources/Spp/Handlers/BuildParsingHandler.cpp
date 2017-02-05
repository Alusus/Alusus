/**
 * @file Spp/Handlers/BuildParsingHandler.cpp
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Handlers
{

using namespace Core;
using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void BuildParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  using SeekVerb = Data::Seeker::SeekVerb;

  auto data = state->getData().get();
  ASSERT(data != 0);
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(data);
  ASSERT(metadata != 0);

  try {
    auto root = this->rootManager->getRootScope().get();
    auto ir = this->llvmGenerator->generateIr(root);
    outStream << STR("------------------ Generated LLVM IR ------------------\n");
    outStream << ir;
    outStream << STR("-------------------------------------------------------\n");

    // Bool found = false;
    // this->rootManager->getSeeker()->doForeach(data, state->getDataStack(),
    //   [=, &found](TiObject *obj)->SeekVerb
    //   {
    //     if (obj != 0) {
    //       outStream << STR("------------------ Generated LLVM IR ------------------\n");
    //       // TODO: Generate IR for the requested element.
    //       outStream << STR("\n------------------------------------------------------\n");
    //       found = true;
    //     }
    //     return SeekVerb::MOVE;
    //   }
    // );
    // if (!found) {
    //   state->addBuildMsg(std::make_shared<InvalidDumpArg>(metadata->getSourceLocation()));
    // }
  } catch (InvalidArgumentException &e) {
    outStream << e.getErrorMessage();
    state->addBuildMsg(std::make_shared<InvalidBuildArg>(metadata->getSourceLocation()));
  }

  state->setData(SharedPtr<TiObject>(0));
}

} } // namespace

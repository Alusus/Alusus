/**
 * @file Spp/Handlers/DumpParsingHandler.cpp
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <spp.h>

namespace Spp { namespace Handlers
{

using namespace Core;
using namespace Core::Data;


//==============================================================================
// Overloaded Abstract Functions

void DumpParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  // TODO: Modify this once we have Seeker so it can dump non-root elements.

  auto data = state->getData().get();
  ASSERT(data != 0);
  auto metadata = ti_cast<Core::Data::Ast::MetadataHolder>(data);
  ASSERT(metadata != 0);

  auto identifier = ti_cast<Core::Data::Ast::Identifier>(data);
  if (identifier == 0) {
    state->addBuildMsg(std::make_shared<InvalidDumpArg>(metadata->getSourceLocation()));
    state->setData(SharedPtr<TiObject>(0));
    return;
  }

  auto def = this->rootManager->getDefinitionsRepository()->tryGet(identifier->getValue().c_str());

  if (def != 0) {
    outStream << STR("------------------ Parsed Data Dump ------------------\n");
    dumpData(outStream, def, 0);
    outStream << STR("\n------------------------------------------------------\n");
  } else {
    // Create a build msg.
    Str message = "Couldn't find element: ";
    message += identifier->getValue();
    state->addBuildMsg(std::make_shared<Processing::CustomBuildMsg>(message.c_str(), metadata->getSourceLocation()));
  }

  // Reset parsed data because we are done with the command.
  state->setData(SharedPtr<TiObject>(0));
}

} } // namespace

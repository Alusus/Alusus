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

#include "spp.h"

namespace Spp { namespace Handlers
{

using namespace Core;
using namespace Core::Data;


//==============================================================================
// Overloaded Abstract Functions

void DumpParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  using SeekVerb = Data::Seeker::SeekVerb;

  auto data = state->getData().get();
  ASSERT(data != 0);
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(data);
  ASSERT(metadata != 0);

  try {
    Bool found = false;
    this->rootManager->getSeeker()->foreach(data, state->getDataStack(),
      [=, &found](Seeker *newSeeker, TiObject *obj)->SeekVerb
      {
        if (obj != 0) {
          outStream << STR("------------------ Parsed Data Dump ------------------\n");
          dumpData(outStream, obj, 0);
          outStream << STR("\n------------------------------------------------------\n");
          found = true;
        }
        return SeekVerb::MOVE;
      }
    );
    if (!found) {
      state->addBuildMsg(std::make_shared<InvalidDumpArg>(metadata->getSourceLocation()));
    }
  } catch (InvalidArgumentException) {
    state->addBuildMsg(std::make_shared<InvalidDumpArg>(metadata->getSourceLocation()));
  }

  state->setData(SharedPtr<TiObject>(0));
}

} } // namespace

/**
 * @file Core/Standard/DumpParsingHandler.cpp
 * Contains the implementation of Core::Standard::DumpParsingHandler.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Standard
{

using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void DumpParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  using SeekVerb = Data::Seeker::Verb;

  auto data = state->getData().ti_cast_get<Container>()->get(1);
  ASSERT(data != 0);
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(data);
  ASSERT(metadata != 0);

  try {
    Bool found = false;
    this->rootManager->getSeeker()->foreach(data, state->getDataStack(),
      [=, &found](TiObject *obj, Notice*)->SeekVerb
      {
        if (obj != 0) {
          outStream << STR("------------------ Parsed Data Dump ------------------\n");
          dumpData(outStream, obj, 0);
          outStream << STR("\n------------------------------------------------------\n");
          found = true;
        }
        return SeekVerb::MOVE;
      }, 0
    );
    if (!found) {
      state->addNotice(std::make_shared<InvalidDumpArgNotice>(metadata->findSourceLocation()));
    }
  } catch (InvalidArgumentException) {
    state->addNotice(std::make_shared<InvalidDumpArgNotice>(metadata->findSourceLocation()));
  }

  state->setData(SharedPtr<TiObject>(0));
}

} } // namespace

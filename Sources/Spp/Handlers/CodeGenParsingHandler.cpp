/**
 * @file Spp/Handlers/CodeGenParsingHandler.cpp
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

void CodeGenParsingHandler::onProdEnd(Processing::Parser *parser, Processing::ParserState *state)
{
  using SeekVerb = Data::Seeker::Verb;

  auto data = state->getData().get();
  ASSERT(data != 0);
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(data);
  ASSERT(metadata != 0);

  try {
    auto root = this->rootManager->getRootScope().get();

    if (this->execute) {
      // Build and execute code.
      auto container = ti_cast<Core::Data::Container>(data);
      ASSERT(container != 0);
      auto entryRef = container->get(1);
      this->llvmGenerator->execute(root, state, entryRef);
    } else {
      // Build and dump IR code.
      Core::Basic::StrStream ir;
      Bool result = this->llvmGenerator->build(root, state, ir);
      if (result) {
        outStream << STR("-------------------- Generated LLVM IR ---------------------\n");
        outStream << ir.str();
        outStream << STR("------------------------------------------------------------\n");
      } else {
        parser->flushApprovedNotices();
        outStream << STR("Build Failed...\n");
        outStream << STR("--------------------- Partial LLVM IR ----------------------\n");
        outStream << ir.str();
        outStream << STR("------------------------------------------------------------\n");
      }
    }

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
    //   state->addNotice(std::make_shared<InvalidBuildArgNotice>(metadata->getSourceLocation()));
    // }
  } catch (Exception &e) {
    outStream << e.getVerboseErrorMessage() << NEW_LINE;
    state->addNotice(std::make_shared<InvalidBuildArgNotice>(metadata->getSourceLocation()));
  }

  state->setData(SharedPtr<TiObject>(0));
}

} } // namespace

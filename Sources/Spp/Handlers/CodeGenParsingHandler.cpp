/**
 * @file Spp/Handlers/CodeGenParsingHandler.cpp
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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
    // Build the IR code.
    auto root = this->rootManager->getRootScope().get();
    this->targetGenerator->prepareBuild(state->getNoticeStore());
    auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(this->targetGenerator);
    Bool result = this->generator->generate(root, state, targetGeneration);

    if (this->execute) {
      // Execute the code if build was successful.
      if (result) {
        // Find the entry ref.
        auto container = ti_cast<Core::Data::Container>(data);
        ASSERT(container != 0);
        auto entryRef = ti_cast<Core::Data::Node>(container->get(1));
        ASSERT(entryRef != 0);
        // Find the entry function.
        TiObject *callee = 0;
        Ast::Type *calleeType = 0;
        Ast::Function *entryFunction = 0;
        if (this->astHelper->lookupCallee(
          entryRef, root, true, 0, this->targetGenerator->getExecutionContext(),
          callee, calleeType
        )) {
          entryFunction = ti_cast<Ast::Function>(callee);
        }
        // Execute if an entry function was found.
        if (entryFunction == 0) {
          state->addNotice(std::make_shared<Ast::NoCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(entryRef)));
        } else {
          auto funcName = this->astHelper->getFunctionName(entryFunction).c_str();
          this->targetGenerator->execute(funcName);
        }
      }
    } else {
      // Dump the IR code.
      Core::Basic::StrStream ir;
      this->targetGenerator->dumpIr(ir);
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
    //   state->addNotice(std::make_shared<InvalidBuildArgNotice>(metadata->findSourceLocation()));
    // }
  } catch (Exception &e) {
    outStream << e.getVerboseErrorMessage() << NEW_LINE;
    state->addNotice(std::make_shared<InvalidBuildArgNotice>(metadata->findSourceLocation()));
  }

  state->setData(SharedPtr<TiObject>(0));
}

} } // namespace

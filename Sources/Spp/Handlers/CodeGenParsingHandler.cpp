/**
 * @file Spp/Handlers/CodeGenParsingHandler.cpp
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(data);
  ASSERT(metadata != 0);

  try {
    auto root = this->rootManager->getRootScope().get();

    // Perform macro processing.
    this->macroProcessor->preparePass(state->getNoticeStore());
    Bool result = this->macroProcessor->runMacroPass(root);

    if (result) {
      // Build the IR code.
      this->targetGenerator->prepareBuild(state->getNoticeStore());
      auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(this->targetGenerator);
      result = this->generator->generate(root, state, targetGeneration);

      if (this->execute) {
        // Execute the code if build was successful.
        auto minSeverity = this->rootManager->getMinNoticeSeverityEncountered();
        auto thisMinSeverity = state->getNoticeStore()->getMinEncounteredSeverity();
        if (result && (minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
          // Find the entry ref.
          auto container = ti_cast<Containing<TiObject>>(data);
          ASSERT(container != 0);
          auto entryRef = ti_cast<Core::Data::Node>(container->getElement(1));
          ASSERT(entryRef != 0);
          // Generate args.
          PlainList<TiObject> paramAstTypes;
          this->getEntryArgTypes(&paramAstTypes);
          Bool sendArgs;
          // Find the entry function.
          TiObject *callee = 0;
          Ast::Type *calleeType = 0;
          Ast::Function *entryFunction = 0;
          SharedPtr<Core::Notices::Notice> notice;
          if (this->astHelper->lookupCallee(
            entryRef, root, true, &paramAstTypes, this->targetGenerator->getExecutionContext(),
            callee, calleeType, notice
          )) {
            entryFunction = ti_cast<Ast::Function>(callee);
            sendArgs = true;
          } else if (this->astHelper->lookupCallee(
            entryRef, root, true, 0, this->targetGenerator->getExecutionContext(),
            callee, calleeType, notice
          )) {
            sendArgs = false;
            entryFunction = ti_cast<Ast::Function>(callee);
          }
          // Execute if an entry function was found.
          if (entryFunction == 0) {
            if (notice != 0) {
              state->addNotice(notice);
            } else {
              state->addNotice(
                std::make_shared<Spp::Notices::NoCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(entryRef))
              );
            }
          } else {
            auto funcName = this->astHelper->getFunctionName(entryFunction).c_str();
            auto executionResult = this->targetGenerator->execute(
              funcName, sendArgs, this->rootManager->getProcessArgCount(), this->rootManager->getProcessArgs()
            );
            // If the execution wasn't successful, throw the error and quit.
            auto retType = entryFunction->getType()->traceRetType(this->astHelper);
            if (!this->astHelper->isVoid(retType) && executionResult != 0) throw executionResult;
          }
        } else {
          state->addNotice(
            std::make_shared<Spp::Notices::ExecutionAbortedNotice>(metadata->findSourceLocation())
          );
        }
      } else {
        // Dump the IR code.
        StrStream ir;
        this->targetGenerator->dumpIr(ir);
        if (result) {
          outStream << S("-------------------- Generated LLVM IR ---------------------\n");
          outStream << ir.str();
          outStream << S("------------------------------------------------------------\n");
        } else {
          parser->flushApprovedNotices();
          outStream << S("Build Failed...\n");
          outStream << S("--------------------- Partial LLVM IR ----------------------\n");
          outStream << ir.str();
          outStream << S("------------------------------------------------------------\n");
        }
      }
    }

    // Bool found = false;
    // this->rootManager->getSeeker()->foreach(data, state->getDataStack(),
    //   [=, &found](TiObject *obj)->SeekVerb
    //   {
    //     if (obj != 0) {
    //       outStream << S("------------------ Generated LLVM IR ------------------\n");
    //       // TODO: Generate IR for the requested element.
    //       outStream << S("\n------------------------------------------------------\n");
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
    state->addNotice(std::make_shared<Spp::Notices::InvalidBuildArgNotice>(metadata->findSourceLocation()));
  }

  state->setData(SharedPtr<TiObject>(0));
}


void CodeGenParsingHandler::getEntryArgTypes(DynamicContaining<TiObject> *resultTypes)
{
  VALIDATE_NOT_NULL(resultTypes);

  // Prepare argCount type.
  auto integerType = this->astHelper->getIntType(32);
  TioSharedPtr argCount;
  resultTypes->addElement(integerType);

  // Prepare args type.
  auto charType = this->astHelper->getCharType();
  auto charPtrType = this->astHelper->getPointerTypeFor(charType);
  auto charPtrArrayType = this->astHelper->getArrayTypeFor(charPtrType);
  auto charPtrArrayPtrType = this->astHelper->getPointerTypeFor(charPtrArrayType);
  resultTypes->addElement(charPtrArrayPtrType);
}

} } // namespace

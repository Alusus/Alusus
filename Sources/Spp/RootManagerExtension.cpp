/**
 * @file Spp/RootManagerExtension.cpp
 * Contains the implementation of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

//==============================================================================
// Initialization Functions

RootManagerExtension::Overrides* RootManagerExtension::extend(
  Core::Main::RootManager *rootManager,
  SharedPtr<Ast::Helper> const &astHelper, SharedPtr<CodeGen::MacroProcessor> const &macroProcessor,
  SharedPtr<CodeGen::Generator> const &generator, SharedPtr<LlvmCodeGen::TargetGenerator> const &targetGenerator
) {
  auto extension = std::make_shared<RootManagerExtension>(rootManager);
  rootManager->addDynamicInterface(extension);

  auto overrides = new Overrides();
  extension->astHelper = astHelper;
  extension->macroProcessor = macroProcessor;
  extension->generator = generator;
  extension->targetGenerator = targetGenerator;
  extension->rootStmtTgFunc = TioSharedPtr::null;
  overrides->executeRootElementRef = extension->executeRootElement.set(
    &RootManagerExtension::_executeRootElement
  ).get();
  overrides->dumpLlvmIrForElementRef = extension->dumpLlvmIrForElement.set(
    &RootManagerExtension::_dumpLlvmIrForElement
  ).get();
  overrides->resetBuildDataRef = extension->resetBuildData.set(&RootManagerExtension::_resetBuildData).get();

  return overrides;
}


void RootManagerExtension::unextend(Core::Main::RootManager *rootManager, Overrides *overrides)
{
  auto extension = ti_cast<RootManagerExtension>(rootManager);
  extension->executeRootElement.reset(overrides->executeRootElementRef);
  extension->dumpLlvmIrForElement.reset(overrides->dumpLlvmIrForElementRef);
  extension->resetBuildData.reset(overrides->resetBuildDataRef);
  extension->astHelper.remove();
  extension->macroProcessor.remove();
  extension->generator.remove();
  extension->targetGenerator.remove();
  extension->rootStmtTgFunc.remove();
  rootManager->removeDynamicInterface<RootManagerExtension>();
  delete overrides;
}


//==============================================================================
// Main Functions

void RootManagerExtension::_executeRootElement(
  TiObject *self, TioSharedPtr const &element, Core::Notices::Store *noticeStore
) {
  PREPARE_SELF(rootManager, Core::Main::RootManager);
  PREPARE_SELF(rootManagerExt, RootManagerExtension);

  auto root = rootManager->getRootScope().get();

  if (rootManager->isInteractive()) {
    // If we are running in an interactive mode and we faced previous errors, we'll try to clear the errors and start
    // fresh to give the user a chance to correct the errors if possible.
    auto minSeverity = rootManager->getMinNoticeSeverityEncountered();
    auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
    if ((minSeverity >= 0 && minSeverity <= 1) || (thisMinSeverity >= 0 && thisMinSeverity <= 1)) {
      rootManager->resetMinNoticeSeverityEncountered();
      noticeStore->resetMinEncounteredSeverity();
      rootManagerExt->resetBuildData(root);
      rootManagerExt->targetGenerator->resetBuild();
    }
  }

  // Perform macro processing.
  // TODO: Perform macro processing only on the new element.
  rootManagerExt->macroProcessor->preparePass(noticeStore);
  Bool result = rootManagerExt->macroProcessor->runMacroPass(root);

  if (result) {
    // Build the IR code.
    rootManagerExt->targetGenerator->setGlobalItemRepo(rootManagerExt->generator->getGlobalItemRepo());
    rootManagerExt->targetGenerator->setNoticeStore(noticeStore);
    auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(rootManagerExt->targetGenerator.get().get());
    rootManagerExt->generator->prepareBuild(noticeStore, false);
    auto generation = ti_cast<CodeGen::Generation>(rootManagerExt->generator.get().get());

    // Generate function type.
    TioSharedPtr tgVoidType;
    if (!rootManagerExt->targetGenerator->generateVoidType(tgVoidType)) {
      throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
    }
    SharedMap<TiObject> argTypes;
    TioSharedPtr tgFuncType;
    if (!rootManagerExt->targetGenerator->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
      throw EXCEPTION(GenericException, S("Failed to generate function type for root scope execution."));
    }

    // Prepare a function name.
    auto funcName = S("__rootstatement__");

    // Generate the function.
    TioSharedPtr tgFunc;
    if (!rootManagerExt->targetGenerator->generateFunctionDecl(funcName, tgFuncType.get(), tgFunc)) {
      throw EXCEPTION(GenericException, S("Failed to generate function declaration for root scope statement."));
    }
    rootManagerExt->rootStmtTgFunc = tgFunc;
    SharedList<TiObject> args;
    TioSharedPtr context;
    if (!rootManagerExt->targetGenerator->prepareFunctionBody(
      rootManagerExt->rootStmtTgFunc.get().get(), tgFuncType.get(), &args, context)
    ) {
      throw EXCEPTION(GenericException, S("Failed to generate function body for root scope statement."));
    }
    CodeGen::TerminalStatement terminal;
    result = generation->generateStatement(element.get(), targetGeneration, context.get(), terminal);
    if (!rootManagerExt->targetGenerator->finishFunctionBody(
      rootManagerExt->rootStmtTgFunc.get().get(), tgFuncType.get(), &args, context.get()
    )) {
      throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
    }

    // Execute
    auto minSeverity = rootManager->getMinNoticeSeverityEncountered();
    auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
    if (result && (minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
      rootManagerExt->targetGenerator->execute(funcName);
    }

    // Delete the function if it wasn't already deleted automatically.
    if (rootManagerExt->rootStmtTgFunc != 0) {
      if (!rootManagerExt->targetGenerator->deleteFunction(rootManagerExt->rootStmtTgFunc.get().get())) {
        throw EXCEPTION(GenericException, S("Failed to delete root statement function."));
      }
      rootManagerExt->rootStmtTgFunc = TioSharedPtr::null;
    }
  }
}


void RootManagerExtension::_dumpLlvmIrForElement(
  TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(rootManager, Core::Main::RootManager);
  PREPARE_SELF(rootManagerExt, RootManagerExtension);

  auto root = rootManager->getRootScope().get();
  rootManagerExt->resetBuildData(root);
  rootManagerExt->targetGenerator->resetBuild();
  rootManagerExt->rootStmtTgFunc = TioSharedPtr::null;

  // Preprocessing.
  rootManagerExt->macroProcessor->preparePass(noticeStore);
  if (!rootManagerExt->macroProcessor->runMacroPass(root)) return;

  // Prepare for the build.
  rootManagerExt->targetGenerator->setGlobalItemRepo(rootManagerExt->generator->getGlobalItemRepo());
  rootManagerExt->targetGenerator->setNoticeStore(noticeStore);
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(rootManagerExt->targetGenerator.get().get());
  rootManagerExt->generator->prepareBuild(noticeStore, true);
  auto generation = ti_cast<CodeGen::Generation>(rootManagerExt->generator.get().get());

  // Generate the element.
  Bool result;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), targetGeneration);
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), targetGeneration);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("element"), S("Invalid argument type."));
  }

  // Dump the IR code.
  StrStream ir;
  rootManagerExt->targetGenerator->dumpIr(ir);
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


void RootManagerExtension::_resetBuildData(TiObject *self, TiObject *obj)
{
  if (obj == 0) return;
  if (obj->isDerivedFrom<Core::Data::Grammar::Module>()) return;

  CodeGen::removeCodeGenData(obj);
  CodeGen::resetCodeGenFailed(obj);

  auto container = ti_cast<Core::Basic::Containing<TiObject>>(obj);
  if (container != 0) {
    PREPARE_SELF(rootManagerExt, RootManagerExtension);
    for (Int i = 0; i < container->getElementCount(); ++i) {
      rootManagerExt->resetBuildData(container->getElement(i));
    }
  }
}

} // namespace

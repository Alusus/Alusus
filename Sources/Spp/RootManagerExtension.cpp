/**
 * @file Spp/RootManagerExtension.cpp
 * Contains the implementation of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  extension->rootCtorTgFunc = TioSharedPtr::null;
  extension->rootStmtTgFunc = TioSharedPtr::null;
  overrides->executeRootElementRef = extension->executeRootElement.set(
    &RootManagerExtension::_executeRootElement
  ).get();
  overrides->dumpLlvmIrForElementRef = extension->dumpLlvmIrForElement.set(
    &RootManagerExtension::_dumpLlvmIrForElement
  ).get();
  overrides->buildObjectFileForElementRef = extension->buildObjectFileForElement.set(
    &RootManagerExtension::_buildObjectFileForElement
  ).get();
  overrides->resetBuildDataRef = extension->resetBuildData.set(&RootManagerExtension::_resetBuildData).get();
  overrides->importFileRef = extension->importFile.set(&RootManagerExtension::_importFile).get();
  overrides->getModifierStringsRef = extension->getModifierStrings.set(&RootManagerExtension::_getModifierStrings).get();

  return overrides;
}


void RootManagerExtension::unextend(Core::Main::RootManager *rootManager, Overrides *overrides)
{
  auto extension = ti_cast<RootManagerExtension>(rootManager);
  extension->executeRootElement.reset(overrides->executeRootElementRef);
  extension->dumpLlvmIrForElement.reset(overrides->dumpLlvmIrForElementRef);
  extension->buildObjectFileForElement.reset(overrides->buildObjectFileForElementRef);
  extension->resetBuildData.reset(overrides->resetBuildDataRef);
  extension->importFile.reset(overrides->importFileRef);
  extension->getModifierStrings.reset(overrides->getModifierStringsRef);
  extension->astHelper.remove();
  extension->macroProcessor.remove();
  extension->generator.remove();
  extension->targetGenerator.remove();
  extension->rootCtorTgFunc.remove();
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

  // Build the IR code.
  rootManagerExt->targetGenerator->setGlobalItemRepo(rootManagerExt->generator->getGlobalItemRepo());
  rootManagerExt->targetGenerator->setNoticeStore(noticeStore);
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(rootManagerExt->targetGenerator.get().get());
  rootManagerExt->generator->prepareBuild(noticeStore, false);
  auto generation = ti_cast<CodeGen::Generation>(rootManagerExt->generator.get().get());

  // If there was a previous root functions, delete them now.
  if (rootManagerExt->rootCtorTgFunc != 0) {
    if (!rootManagerExt->targetGenerator->deleteFunction(rootManagerExt->rootCtorTgFunc.get().get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root constructor function."));
    }
    rootManagerExt->rootCtorTgFunc = TioSharedPtr::null;
  }
  if (rootManagerExt->rootStmtTgFunc != 0) {
    if (!rootManagerExt->targetGenerator->deleteFunction(rootManagerExt->rootStmtTgFunc.get().get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root statement function."));
    }
    rootManagerExt->rootStmtTgFunc = TioSharedPtr::null;
  }

  // Generate function type.
  TioSharedPtr tgFuncType = rootManagerExt->getVoidNoArgsFuncTgType();

  // Prepare the constructor function.
  auto ctorFuncName = S("__constructor__");
  TioSharedPtr ctorContext;
  TioSharedPtr ctorTgFunc;
  rootManagerExt->prepareFunction(ctorFuncName, tgFuncType.get(), ctorContext, ctorTgFunc);
  rootManagerExt->rootCtorTgFunc = ctorTgFunc;

  // Prepare the execution function.
  auto funcName = S("__rootstatement__");
  TioSharedPtr context;
  TioSharedPtr tgFunc;
  rootManagerExt->prepareFunction(funcName, tgFuncType.get(), context, tgFunc);
  rootManagerExt->rootStmtTgFunc = tgFunc;
  CodeGen::setCodeGenData(root, context);

  // Generate the function.
  CodeGen::TerminalStatement terminal;
  CodeGen::DestructionStack destructionStack;
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::GenDeps deps(targetGeneration, context.get(), &destructionStack, ctorContext.get(), &globalDestructionStack);
  Bool result = generation->generateStatement(element.get(), deps, terminal);
  if (!generation->generateVarGroupDestruction(deps, 0)) result = false;

  // Finalize the two functions.
  SharedList<TiObject> args;
  if (!rootManagerExt->targetGenerator->finishFunctionBody(
    rootManagerExt->rootCtorTgFunc.get().get(), tgFuncType.get(), &args, ctorContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }
  if (!rootManagerExt->targetGenerator->finishFunctionBody(
    rootManagerExt->rootStmtTgFunc.get().get(), tgFuncType.get(), &args, context.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
  }

  // Execute
  auto minSeverity = rootManager->getMinNoticeSeverityEncountered();
  auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
  if (result && (minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
    rootManagerExt->targetGenerator->execute(ctorFuncName);
    rootManagerExt->targetGenerator->execute(funcName);
  }
  CodeGen::removeCodeGenData(root);
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
  rootManagerExt->rootCtorTgFunc = TioSharedPtr::null;
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

  // Generate function type.
  TioSharedPtr tgFuncType = rootManagerExt->getVoidNoArgsFuncTgType();

  // Prepare the constructor function.
  auto ctorFuncName = S("__constructor__");
  TioSharedPtr ctorContext;
  TioSharedPtr ctorTgFunc;
  rootManagerExt->prepareFunction(ctorFuncName, tgFuncType.get(), ctorContext, ctorTgFunc);

  // Generate the element.
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::GenDeps deps(targetGeneration, 0, 0, ctorContext.get(), &globalDestructionStack);
  Bool result;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), deps);
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), deps);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("element"), S("Invalid argument type."));
  }

  // Finalize the root constructor.
  SharedList<TiObject> args;
  if (!rootManagerExt->targetGenerator->finishFunctionBody(
    ctorTgFunc.get(), tgFuncType.get(), &args, ctorContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
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


Bool RootManagerExtension::_buildObjectFileForElement(
  TiObject *self, TiObject *element, Char const *objectFilename, Core::Notices::Store *noticeStore,
  Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(rootManager, Core::Main::RootManager);
  PREPARE_SELF(rootManagerExt, RootManagerExtension);

  auto root = rootManager->getRootScope().get();
  rootManagerExt->resetBuildData(root);
  rootManagerExt->targetGenerator->resetBuild();
  rootManagerExt->rootCtorTgFunc = TioSharedPtr::null;
  rootManagerExt->rootStmtTgFunc = TioSharedPtr::null;

  // Preprocessing.
  rootManagerExt->macroProcessor->preparePass(noticeStore);
  if (!rootManagerExt->macroProcessor->runMacroPass(root)) return false;

  // Prepare for the build.
  rootManagerExt->targetGenerator->setGlobalItemRepo(rootManagerExt->generator->getGlobalItemRepo());
  rootManagerExt->targetGenerator->setNoticeStore(noticeStore);
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(rootManagerExt->targetGenerator.get().get());
  rootManagerExt->generator->prepareBuild(noticeStore, true);
  auto generation = ti_cast<CodeGen::Generation>(rootManagerExt->generator.get().get());

  // Generate function type.
  TioSharedPtr tgFuncType = rootManagerExt->getVoidNoArgsFuncTgType();

  // Prepare the constructor function.
  auto ctorFuncName = S("__constructor__");
  TioSharedPtr ctorContext;
  TioSharedPtr ctorTgFunc;
  rootManagerExt->prepareFunction(ctorFuncName, tgFuncType.get(), ctorContext, ctorTgFunc);

  // Generate the element.
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::GenDeps deps(targetGeneration, 0, 0, ctorContext.get(), &globalDestructionStack);
  Bool result;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), deps);
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), deps);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("element"), S("Invalid argument type."));
  }

  // Finalize the root constructor.
  SharedList<TiObject> args;
  if (!rootManagerExt->targetGenerator->finishFunctionBody(
    ctorTgFunc.get(), tgFuncType.get(), &args, ctorContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }

  if (result) {
    rootManagerExt->targetGenerator->buildObjectFile(objectFilename);
  }

  return result;
}


void RootManagerExtension::_resetBuildData(TiObject *self, TiObject *obj)
{
  if (obj == 0) return;
  if (obj->isDerivedFrom<Core::Data::Grammar::Module>()) return;

  PREPARE_SELF(rootManagerExt, RootManagerExtension);

  auto metahaving = ti_cast<Core::Data::Ast::MetaHaving>(obj);
  if (metahaving != 0) {
    CodeGen::removeCodeGenData(metahaving);
    CodeGen::removeAutoCtor(metahaving);
    CodeGen::removeAutoDtor(metahaving);
    CodeGen::resetCodeGenFailed(metahaving);
  }

  auto container = ti_cast<Core::Basic::Containing<TiObject>>(obj);
  if (container != 0) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      rootManagerExt->resetBuildData(container->getElement(i));
    }
  }

  auto binding = ti_cast<Core::Basic::Binding>(obj);
  if (binding != 0) {
    for (Int i = 0; i < binding->getMemberCount(); ++i) {
      rootManagerExt->resetBuildData(binding->getMember(i));
    }
  }

  auto tpl = ti_cast<Ast::Template>(obj);
  if (tpl != 0) {
    for (Int i = 0; i < tpl->getInstanceCount(); ++i) {
      rootManagerExt->resetBuildData(tpl->getInstance(i).get());
    }
  }
}


void RootManagerExtension::_importFile(TiObject *self, Char const *filename)
{
  PREPARE_SELF(rootManager, Core::Main::RootManager);
  Str error;
  if (!rootManager->tryImportFile(filename, error)) {
    throw EXCEPTION(FileException, filename, C('r'), error.c_str());
  }
}


Bool RootManagerExtension::_getModifierStrings(
  TiObject *self, TiObject *element, Char const *modifierKwd, Char const **resultStrs[], Word *resultCount,
  Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  auto node = ti_cast<Core::Data::Node>(element);
  auto def = Core::Data::findOwner<Core::Data::Ast::Definition>(node);
  if (def == 0 || def->getModifiers() == 0 || def->getModifiers()->getCount() == 0) {
    *resultCount = 0;
    *resultStrs = 0;
    return true;
  }
  // Look for the modifier.
  for (Int i = 0; i < def->getModifiers()->getElementCount(); ++i) {
    auto modifier = ti_cast<Core::Data::Ast::ParamPass>(def->getModifiers()->getElement(i));
    if (modifier == 0) continue;
    auto identifier = modifier->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
    if (identifier != 0 && identifier->getValue() == modifierKwd && modifier->getParam() != 0) {
      // Found the requested modifier.
      Core::Basic::PlainList<TiObject> strList;
      auto strs = modifier->getParam().ti_cast_get<Core::Basic::Containing<TiObject>>();
      if (strs == 0) {
        strList.add(modifier->getParam().get());
        strs = &strList;
      }
      *resultCount = strs->getElementCount();
      *resultStrs = reinterpret_cast<Char const**>(malloc(sizeof(Char*) * (*resultCount)));
      for (Int j = 0; j < *resultCount; ++j) {
        auto str = ti_cast<Core::Data::Ast::StringLiteral>(strs->getElement(j));
        if (str == 0) {
          noticeStore->add(std::make_shared<Spp::Notices::InvalidModifierDataNotice>(
            Core::Data::Ast::findSourceLocation(strs->getElement(j))
          ));
          parser->flushApprovedNotices();
          free(*resultStrs);
          *resultStrs = 0;
          *resultCount = 0;
          return false;
        }
        (*resultStrs)[j] = str->getValue().get();
      }
      return true;
    }
  }
  *resultCount = 0;
  *resultStrs = 0;
  return true;
}


//==============================================================================
// Helper Functions

TioSharedPtr RootManagerExtension::getVoidNoArgsFuncTgType()
{
  // Generate function type.
  TioSharedPtr tgVoidType;
  if (!this->targetGenerator->generateVoidType(tgVoidType)) {
    throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
  }
  SharedMap<TiObject> argTypes;
  TioSharedPtr tgFuncType;
  if (!this->targetGenerator->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
    throw EXCEPTION(GenericException, S("Failed to generate function type for root scope execution."));
  }
  return tgFuncType;
}


void RootManagerExtension::prepareFunction(
  Char const *funcName, TiObject *tgFuncType, TioSharedPtr &context, TioSharedPtr &tgFunc
) {
  if (!this->targetGenerator->generateFunctionDecl(funcName, tgFuncType, tgFunc)) {
    throw EXCEPTION(GenericException, S("Failed to generate function declaration for root scope execution."));
  }
  SharedList<TiObject> args;
  if (!this->targetGenerator->prepareFunctionBody(
    tgFunc.get(), tgFuncType, &args, context)
  ) {
    throw EXCEPTION(GenericException, S("Failed to prepare function body for root scope execution."));
  }
}

} // namespace

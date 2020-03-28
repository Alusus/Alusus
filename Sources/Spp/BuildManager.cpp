/**
 * @file Spp/BuildManager.cpp
 * Contains the implementation of class Spp::BuildManager.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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

void BuildManager::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->prepareRootScopeExecution,
    &this->addRootScopeExecutionElement,
    &this->finalizeRootScopeExecution,
    &this->dumpLlvmIrForElement,
    &this->buildObjectFileForElement,
    &this->resetBuildData
  });
}


void BuildManager::initBindings()
{
  this->prepareRootScopeExecution = &BuildManager::_prepareRootScopeExecution;
  this->addRootScopeExecutionElement = &BuildManager::_addRootScopeExecutionElement;
  this->finalizeRootScopeExecution = &BuildManager::_finalizeRootScopeExecution;
  this->dumpLlvmIrForElement = &BuildManager::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildManager::_buildObjectFileForElement;
  this->resetBuildData = &BuildManager::_resetBuildData;
}


//==============================================================================
// Build Functions

void BuildManager::_prepareRootScopeExecution(TiObject *self, Core::Notices::Store *noticeStore)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto root = buildMgr->rootManager->getRootScope().get();

  if (buildMgr->rootManager->isInteractive()) {
    // If we are running in an interactive mode and we faced previous errors, we'll try to clear the errors and start
    // fresh to give the user a chance to correct the errors if possible.
    auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
    auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
    if ((minSeverity >= 0 && minSeverity <= 1) || (thisMinSeverity >= 0 && thisMinSeverity <= 1)) {
      buildMgr->rootManager->resetMinNoticeSeverityEncountered();
      noticeStore->resetMinEncounteredSeverity();
      buildMgr->resetBuildData(root);
      buildMgr->targetGenerator->resetBuild();
      buildMgr->rootExecTgFuncType = TioSharedPtr::null;
      buildMgr->rootCtorTgFunc = TioSharedPtr::null;
      buildMgr->rootCtorTgContext = TioSharedPtr::null;
      buildMgr->rootStmtTgFunc = TioSharedPtr::null;
      buildMgr->rootStmtTgContext = TioSharedPtr::null;
    }
  }

  // Build the IR code.
  buildMgr->targetGenerator->setGlobalItemRepo(buildMgr->generator->getGlobalItemRepo());
  buildMgr->targetGenerator->setNoticeStore(noticeStore);
  buildMgr->generator->prepareBuild(noticeStore, false);

  // If there was previous root functions, delete them now.
  if (buildMgr->rootCtorTgFunc != 0) {
    if (!buildMgr->targetGenerator->deleteFunction(buildMgr->rootCtorTgFunc.get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root constructor function."));
    }
    buildMgr->rootCtorTgFunc = TioSharedPtr::null;
  }
  if (buildMgr->rootStmtTgFunc != 0) {
    if (!buildMgr->targetGenerator->deleteFunction(buildMgr->rootStmtTgFunc.get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root statement function."));
    }
    buildMgr->rootStmtTgFunc = TioSharedPtr::null;
  }

  // Generate function type.
  if (buildMgr->rootExecTgFuncType == 0) {
    buildMgr->rootExecTgFuncType = buildMgr->getVoidNoArgsFuncTgType();
  }

  // Prepare the constructor function.
  auto ctorFuncName = S("__constructor__");
  TioSharedPtr ctorTgContext;
  TioSharedPtr ctorTgFunc;
  buildMgr->prepareFunction(
    ctorFuncName, buildMgr->rootExecTgFuncType.get(), ctorTgContext, ctorTgFunc
  );
  buildMgr->rootCtorTgFunc = ctorTgFunc;
  buildMgr->rootCtorTgContext = ctorTgContext;

  // Prepare the execution function.
  auto funcName = S("__rootstatement__");
  TioSharedPtr tgContext;
  TioSharedPtr tgFunc;
  buildMgr->prepareFunction(funcName, buildMgr->rootExecTgFuncType.get(), tgContext, tgFunc);
  buildMgr->rootStmtTgFunc = tgFunc;
  buildMgr->rootStmtTgContext = tgContext;
  CodeGen::setCodeGenData(root, tgContext);
}


Bool BuildManager::_addRootScopeExecutionElement(TiObject *self, TioSharedPtr const &element)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(buildMgr->targetGenerator);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  // Generate the statement.
  CodeGen::TerminalStatement terminal;
  CodeGen::DestructionStack destructionStack;
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::GenDeps deps(
    targetGeneration, buildMgr->rootStmtTgContext.get(), &destructionStack,
    buildMgr->rootCtorTgContext.get(), &globalDestructionStack
  );
  Bool result = generation->generateStatement(element.get(), deps, terminal);
  if (!generation->generateVarGroupDestruction(deps, 0)) result = false;
  return result;
}


void BuildManager::_finalizeRootScopeExecution(
  TiObject *self, Core::Notices::Store *noticeStore, Bool execute
) {
  PREPARE_SELF(buildMgr, BuildManager);

  auto root = buildMgr->rootManager->getRootScope().get();

  // Finalize the two functions.
  SharedList<TiObject> args;
  if (!buildMgr->targetGenerator->finishFunctionBody(
    buildMgr->rootCtorTgFunc.get(), buildMgr->rootExecTgFuncType.get(), &args,
    buildMgr->rootCtorTgContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }
  if (!buildMgr->targetGenerator->finishFunctionBody(
    buildMgr->rootStmtTgFunc.get(), buildMgr->rootExecTgFuncType.get(), &args,
    buildMgr->rootStmtTgContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
  }

  // Execute
  auto ctorFuncName = S("__constructor__");
  auto funcName = S("__rootstatement__");
  auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
  auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
  if (execute && (minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
    buildMgr->targetGenerator->execute(ctorFuncName);
    buildMgr->targetGenerator->execute(funcName);
  }
  CodeGen::removeCodeGenData(root);
}


void BuildManager::_dumpLlvmIrForElement(
  TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(buildMgr, BuildManager);

  auto root = buildMgr->rootManager->getRootScope().get();
  buildMgr->resetBuildData(root);
  buildMgr->targetGenerator->resetBuild();
  buildMgr->rootExecTgFuncType = TioSharedPtr::null;
  buildMgr->rootCtorTgFunc = TioSharedPtr::null;
  buildMgr->rootCtorTgContext = TioSharedPtr::null;
  buildMgr->rootStmtTgFunc = TioSharedPtr::null;
  buildMgr->rootStmtTgContext = TioSharedPtr::null;

  // Preprocessing.
  buildMgr->astProcessor->preparePass(noticeStore);
  if (!buildMgr->astProcessor->runPass(root)) return;

  // Prepare for the build.
  buildMgr->targetGenerator->setGlobalItemRepo(buildMgr->generator->getGlobalItemRepo());
  buildMgr->targetGenerator->setNoticeStore(noticeStore);
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(buildMgr->targetGenerator);
  buildMgr->generator->prepareBuild(noticeStore, true);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  // Generate function type.
  TioSharedPtr tgFuncType = buildMgr->getVoidNoArgsFuncTgType();

  // Prepare the constructor function.
  auto ctorFuncName = S("__constructor__");
  TioSharedPtr ctorContext;
  TioSharedPtr ctorTgFunc;
  buildMgr->prepareFunction(ctorFuncName, tgFuncType.get(), ctorContext, ctorTgFunc);

  // Generate the element.
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::GenDeps deps(targetGeneration, 0, 0, ctorContext.get(), &globalDestructionStack);
  Bool result;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), deps);
    CodeGen::GenDeps initDeps(deps, ctorContext.get(), &globalDestructionStack);
    if (!generation->generateModuleInit(static_cast<Ast::Module*>(element), initDeps)) result = false;
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), deps);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("element"), S("Invalid argument type."));
  }

  // Finalize the root constructor.
  SharedList<TiObject> args;
  if (!buildMgr->targetGenerator->finishFunctionBody(
    ctorTgFunc.get(), tgFuncType.get(), &args, ctorContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }

  // Dump the IR code.
  StrStream ir;
  buildMgr->targetGenerator->dumpIr(ir);
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


Bool BuildManager::_buildObjectFileForElement(
  TiObject *self, TiObject *element, Char const *objectFilename, Core::Notices::Store *noticeStore,
  Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(buildMgr, BuildManager);

  auto root = buildMgr->rootManager->getRootScope().get();
  buildMgr->resetBuildData(root);
  buildMgr->targetGenerator->resetBuild();
  buildMgr->rootExecTgFuncType = TioSharedPtr::null;
  buildMgr->rootCtorTgFunc = TioSharedPtr::null;
  buildMgr->rootCtorTgContext = TioSharedPtr::null;
  buildMgr->rootStmtTgFunc = TioSharedPtr::null;
  buildMgr->rootStmtTgContext = TioSharedPtr::null;

  // Preprocessing.
  buildMgr->astProcessor->preparePass(noticeStore);
  if (!buildMgr->astProcessor->runPass(root)) return false;

  // Prepare for the build.
  buildMgr->targetGenerator->setGlobalItemRepo(buildMgr->generator->getGlobalItemRepo());
  buildMgr->targetGenerator->setNoticeStore(noticeStore);
  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(buildMgr->targetGenerator);
  buildMgr->generator->prepareBuild(noticeStore, true);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  // Generate function type.
  TioSharedPtr tgFuncType = buildMgr->getVoidNoArgsFuncTgType();

  // Prepare the constructor function.
  auto ctorFuncName = S("__constructor__");
  TioSharedPtr ctorContext;
  TioSharedPtr ctorTgFunc;
  buildMgr->prepareFunction(ctorFuncName, tgFuncType.get(), ctorContext, ctorTgFunc);

  // Generate the element.
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::GenDeps deps(targetGeneration, 0, 0, ctorContext.get(), &globalDestructionStack);
  Bool result;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), deps);
    CodeGen::GenDeps initDeps(deps, ctorContext.get(), &globalDestructionStack);
    if (!generation->generateModuleInit(static_cast<Ast::Module*>(element), initDeps)) result = false;
    // TODO: Set attribute for the global constructor in the generated object file.
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), deps);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("element"), S("Invalid argument type."));
  }

  // Finalize the root constructor.
  SharedList<TiObject> args;
  if (!buildMgr->targetGenerator->finishFunctionBody(
    ctorTgFunc.get(), tgFuncType.get(), &args, ctorContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }

  if (result) {
    buildMgr->targetGenerator->buildObjectFile(objectFilename);
  }

  return result;
}


void BuildManager::_resetBuildData(TiObject *self, TiObject *obj)
{
  if (obj == 0) return;
  if (obj->isDerivedFrom<Core::Data::Grammar::Module>()) return;

  PREPARE_SELF(buildMgr, BuildManager);

  auto metahaving = ti_cast<Core::Data::Ast::MetaHaving>(obj);
  if (metahaving != 0) {
    CodeGen::removeCodeGenData(metahaving);
    CodeGen::removeAutoCtor(metahaving);
    CodeGen::removeAutoDtor(metahaving);
    CodeGen::resetCodeGenFailed(metahaving);
    CodeGen::resetInitStatementsGenIndex(metahaving);
  }

  auto container = ti_cast<Core::Basic::Containing<TiObject>>(obj);
  if (container != 0) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      buildMgr->resetBuildData(container->getElement(i));
    }
  }

  auto binding = ti_cast<Core::Basic::Binding>(obj);
  if (binding != 0) {
    for (Int i = 0; i < binding->getMemberCount(); ++i) {
      buildMgr->resetBuildData(binding->getMember(i));
    }
  }

  auto tpl = ti_cast<Ast::Template>(obj);
  if (tpl != 0) {
    for (Int i = 0; i < tpl->getInstanceCount(); ++i) {
      buildMgr->resetBuildData(tpl->getInstance(i).get());
    }
  }
}


//==============================================================================
// Helper Functions

TioSharedPtr BuildManager::getVoidNoArgsFuncTgType()
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


void BuildManager::prepareFunction(
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

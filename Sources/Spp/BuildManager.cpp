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
    &this->prepareExecution,
    &this->prepareBuild,
    &this->addElementToBuild,
    &this->finalizeBuild,
    &this->execute,
    &this->dumpLlvmIrForElement,
    &this->buildObjectFileForElement,
    &this->resetBuild,
    &this->resetBuildData
  });
}


void BuildManager::initBindings()
{
  this->prepareExecution = &BuildManager::_prepareExecution;
  this->prepareBuild = &BuildManager::_prepareBuild;
  this->addElementToBuild = &BuildManager::_addElementToBuild;
  this->finalizeBuild = &BuildManager::_finalizeBuild;
  this->execute = &BuildManager::_execute;
  this->dumpLlvmIrForElement = &BuildManager::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildManager::_buildObjectFileForElement;
  this->resetBuild = &BuildManager::_resetBuild;
  this->resetBuildData = &BuildManager::_resetBuildData;
}


//==============================================================================
// Build Functions

void BuildManager::_prepareExecution(
  TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement, Char const *globalFuncName
) {
  PREPARE_SELF(buildMgr, BuildManager);

  if (buildMgr->rootManager->isInteractive()) {
    // If we are running in an interactive mode and we faced previous errors, we'll try to clear the errors and start
    // fresh to give the user a chance to correct the errors if possible.
    auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
    auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
    if ((minSeverity >= 0 && minSeverity <= 1) || (thisMinSeverity >= 0 && thisMinSeverity <= 1)) {
      buildMgr->rootManager->resetMinNoticeSeverityEncountered();
      noticeStore->resetMinEncounteredSeverity();
      buildMgr->resetBuild();
    }
  }

  buildMgr->prepareBuild(noticeStore, globalFuncElement, globalFuncName, false);
}


void BuildManager::_prepareBuild(
  TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement, Char const *globalFuncName,
  Bool offlineExecution
) {
  PREPARE_SELF(buildMgr, BuildManager);

  // Build the IR code.
  buildMgr->targetGenerator->setGlobalItemRepo(buildMgr->generator->getGlobalItemRepo());
  buildMgr->targetGenerator->setNoticeStore(noticeStore);
  buildMgr->generator->prepareBuild(noticeStore, offlineExecution);

  // If there was previous root functions, delete them now.
  if (buildMgr->globalCtorTgFunc != 0) {
    if (!buildMgr->targetGenerator->deleteFunction(buildMgr->globalCtorTgFunc.get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root constructor function."));
    }
    buildMgr->globalCtorTgFunc = TioSharedPtr::null;
  }
  if (buildMgr->globalProcTgFunc != 0) {
    if (!buildMgr->targetGenerator->deleteFunction(buildMgr->globalProcTgFunc.get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root statement function."));
    }
    buildMgr->globalProcTgFunc = TioSharedPtr::null;
  }

  // Generate function type.
  if (buildMgr->globalTgFuncType == 0) {
    buildMgr->globalTgFuncType = buildMgr->getVoidNoArgsFuncTgType();
  }

  // Prepare the constructor function.
  auto ctorFuncName = S("__constructor__");
  TioSharedPtr ctorTgContext;
  TioSharedPtr ctorTgFunc;
  buildMgr->prepareFunction(
    ctorFuncName, buildMgr->globalTgFuncType.get(), ctorTgContext, ctorTgFunc
  );
  buildMgr->globalCtorTgFunc = ctorTgFunc;
  buildMgr->globalCtorTgContext = ctorTgContext;

  if (globalFuncElement != 0) {
    // Prepare the execution function.
    TioSharedPtr tgContext;
    TioSharedPtr tgFunc;
    buildMgr->prepareFunction(globalFuncName, buildMgr->globalTgFuncType.get(), tgContext, tgFunc);
    buildMgr->globalProcTgFunc = tgFunc;
    buildMgr->globalProcTgContext = tgContext;
    buildMgr->extraDataAccessor.setCodeGenData(globalFuncElement, tgContext);
  }
}


Bool BuildManager::_addElementToBuild(TiObject *self, TiObject *element)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(buildMgr->targetGenerator);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  // Generate the statement.
  CodeGen::TerminalStatement terminal;
  CodeGen::DestructionStack destructionStack;
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::Session session(
    &buildMgr->extraDataAccessor, targetGeneration, buildMgr->globalProcTgContext.get(), &destructionStack,
    buildMgr->globalCtorTgContext.get(), &globalDestructionStack
  );
  Bool result;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), &session);
    CodeGen::Session initSession(&session, buildMgr->globalCtorTgContext.get(), &globalDestructionStack);
    if (!generation->generateModuleInit(static_cast<Ast::Module*>(element), &initSession)) result = false;
    // TODO: Set attribute for the global constructor in the generated object file.
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), &session);
  } else {
    result = generation->generateStatement(element, &session, terminal);
    if (!generation->generateVarGroupDestruction(&session, 0)) result = false;
  }
  return result;
}


void BuildManager::_finalizeBuild(
  TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement
) {
  PREPARE_SELF(buildMgr, BuildManager);

  SharedList<TiObject> args;

  if (!buildMgr->targetGenerator->finishFunctionBody(
    buildMgr->globalCtorTgFunc.get(), buildMgr->globalTgFuncType.get(), &args,
    buildMgr->globalCtorTgContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }

  if (globalFuncElement != 0) {
    if (!buildMgr->targetGenerator->finishFunctionBody(
      buildMgr->globalProcTgFunc.get(), buildMgr->globalTgFuncType.get(), &args,
      buildMgr->globalProcTgContext.get()
    )) {
      throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
    }
    buildMgr->extraDataAccessor.removeCodeGenData(globalFuncElement);
  }
}


Bool BuildManager::_execute(
  TiObject *self, Core::Notices::Store *noticeStore, Char const *funcName
) {
  PREPARE_SELF(buildMgr, BuildManager);

  auto ctorFuncName = S("__constructor__");
  auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
  auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
  if ((minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
    buildMgr->targetGenerator->execute(ctorFuncName);
    buildMgr->targetGenerator->execute(funcName);
    return true;
  } else {
    return false;
  }
}


void BuildManager::_dumpLlvmIrForElement(
  TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(buildMgr, BuildManager);

  buildMgr->resetBuild();
  buildMgr->prepareBuild(noticeStore, 0, 0, true);
  auto result = buildMgr->addElementToBuild(element);
  buildMgr->finalizeBuild(noticeStore, 0);

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

  buildMgr->resetBuild();
  buildMgr->prepareBuild(noticeStore, 0, 0, true);
  auto result = buildMgr->addElementToBuild(element);
  buildMgr->finalizeBuild(noticeStore, 0);

  if (result) {
    buildMgr->targetGenerator->buildObjectFile(objectFilename);
  }

  return result;
}


void BuildManager::_resetBuild(TiObject *self)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto root = buildMgr->rootManager->getRootScope().get();
  buildMgr->resetBuildData(root);
  buildMgr->targetGenerator->resetBuild();
  buildMgr->globalTgFuncType = TioSharedPtr::null;
  buildMgr->globalCtorTgFunc = TioSharedPtr::null;
  buildMgr->globalCtorTgContext = TioSharedPtr::null;
  buildMgr->globalProcTgFunc = TioSharedPtr::null;
  buildMgr->globalProcTgContext = TioSharedPtr::null;
}


void BuildManager::_resetBuildData(TiObject *self, TiObject *obj)
{
  if (obj == 0) return;
  if (obj->isDerivedFrom<Core::Data::Grammar::Module>()) return;

  PREPARE_SELF(buildMgr, BuildManager);

  auto metahaving = ti_cast<Core::Data::Ast::MetaHaving>(obj);
  if (metahaving != 0) {
    buildMgr->extraDataAccessor.removeCodeGenData(metahaving);
    buildMgr->extraDataAccessor.removeAutoCtor(metahaving);
    buildMgr->extraDataAccessor.removeAutoDtor(metahaving);
    buildMgr->extraDataAccessor.resetCodeGenFailed(metahaving);
    buildMgr->extraDataAccessor.resetInitStatementsGenIndex(metahaving);
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

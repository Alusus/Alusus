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
    &this->dumpLlvmIrForElement,
    &this->buildObjectFileForElement,
    &this->resetBuild,
    &this->resetBuildData
  });
}


void BuildManager::initBindings()
{
  auto building = ti_cast<Building>(this);

  building->prepareExecution = &BuildManager::_prepareExecution;
  building->prepareBuild = &BuildManager::_prepareBuild;
  building->addElementToBuild = &BuildManager::_addElementToBuild;
  building->finalizeBuild = &BuildManager::_finalizeBuild;
  building->execute = &BuildManager::_execute;
  building->deleteTempFunctions = &BuildManager::_deleteTempFunctions;

  this->dumpLlvmIrForElement = &BuildManager::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildManager::_buildObjectFileForElement;
  this->resetBuild = &BuildManager::_resetBuild;
  this->resetBuildData = &BuildManager::_resetBuildData;
}


//==============================================================================
// Build Functions

void BuildManager::_prepareExecution(
  TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement, BuildSession &buildSession
) {
  PREPARE_SELF(buildMgr, BuildManager);
  PREPARE_SELF(building, Building);

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

  building->prepareBuild(noticeStore, globalFuncElement, false, buildSession);
}


void BuildManager::_prepareBuild(
  TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement,
  Bool offlineExecution, BuildSession &buildSession
) {
  PREPARE_SELF(buildMgr, BuildManager);

  buildMgr->funcNameIndex++;
  StrStream globalCtorName;
  globalCtorName << S("__constructor__");
  StrStream globalEntryName;
  globalEntryName << S("__entry__");

  if (!offlineExecution) {
    globalCtorName << buildMgr->funcNameIndex;
    globalEntryName << buildMgr->funcNameIndex;
  }

  buildSession.globalCtorName = globalCtorName.str();
  buildSession.globalEntryName = globalEntryName.str();

  // Build the IR code.
  buildMgr->targetGenerator->setGlobalItemRepo(buildMgr->generator->getGlobalItemRepo());
  buildMgr->targetGenerator->setNoticeStore(noticeStore);
  buildMgr->generator->prepareBuild(noticeStore);
  buildSession.offlineExecution = offlineExecution;

  // Generate function type.
  if (buildMgr->globalTgFuncType == 0) {
    buildMgr->globalTgFuncType = buildMgr->getVoidNoArgsFuncTgType();
  }

  // Prepare the constructor function.
  TioSharedPtr ctorTgContext;
  TioSharedPtr ctorTgFunc;
  buildMgr->prepareFunction(
    buildSession.globalCtorName.c_str(), buildMgr->globalTgFuncType.get(), ctorTgContext, ctorTgFunc
  );
  buildSession.globalCtorTgFunc = ctorTgFunc;
  buildSession.globalCtorTgContext = ctorTgContext;

  if (globalFuncElement != 0) {
    // Prepare the execution function.
    TioSharedPtr tgContext;
    TioSharedPtr tgFunc;
    buildMgr->prepareFunction(
      buildSession.globalEntryName.c_str(), buildMgr->globalTgFuncType.get(), tgContext, tgFunc
    );
    buildSession.globalEntryTgFunc = tgFunc;
    buildSession.globalEntryTgContext = tgContext;
    buildMgr->extraDataAccessor.setCodeGenData(globalFuncElement, tgContext);
  }
}


Bool BuildManager::_addElementToBuild(TiObject *self, TiObject *element, BuildSession &buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(buildMgr->targetGenerator);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  // Generate the statement.
  CodeGen::TerminalStatement terminal;
  CodeGen::DestructionStack destructionStack;
  CodeGen::DestructionStack globalDestructionStack;
  CodeGen::Session session(
    &buildMgr->extraDataAccessor, targetGeneration, buildSession.globalEntryTgContext.get(), &destructionStack,
    buildSession.globalCtorTgContext.get(), &globalDestructionStack, buildSession.offlineExecution
  );
  Bool result;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), &session);
    CodeGen::Session initSession(&session, buildSession.globalCtorTgContext.get(), &globalDestructionStack);
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
  TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement, BuildSession &buildSession
) {
  PREPARE_SELF(buildMgr, BuildManager);

  SharedList<TiObject> args;

  if (!buildMgr->targetGenerator->finishFunctionBody(
    buildSession.globalCtorTgFunc.get(), buildMgr->globalTgFuncType.get(), &args,
    buildSession.globalCtorTgContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }

  if (globalFuncElement != 0) {
    if (!buildMgr->targetGenerator->finishFunctionBody(
      buildSession.globalEntryTgFunc.get(), buildMgr->globalTgFuncType.get(), &args,
      buildSession.globalEntryTgContext.get()
    )) {
      throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
    }
    buildMgr->extraDataAccessor.removeCodeGenData(globalFuncElement);
  }
}


Bool BuildManager::_execute(
  TiObject *self, Core::Notices::Store *noticeStore, BuildSession &buildSession
) {
  PREPARE_SELF(buildMgr, BuildManager);

  auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
  auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
  if ((minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
    buildMgr->targetGenerator->execute(buildSession.globalCtorName.c_str());
    buildMgr->targetGenerator->execute(buildSession.globalEntryName.c_str());
    return true;
  } else {
    return false;
  }
}


void BuildManager::_deleteTempFunctions(TiObject *self, BuildSession &buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  if (buildSession.globalCtorTgFunc != 0) {
    if (!buildMgr->targetGenerator->deleteFunction(buildSession.globalCtorTgFunc.get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root constructor function."));
    }
  }
  if (buildSession.globalEntryTgFunc != 0) {
    if (!buildMgr->targetGenerator->deleteFunction(buildSession.globalEntryTgFunc.get())) {
      throw EXCEPTION(GenericException, S("Failed to delete root statement function."));
    }
  }
}


void BuildManager::_dumpLlvmIrForElement(
  TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(buildMgr, BuildManager);
  PREPARE_SELF(building, Building);

  buildMgr->resetBuild();

  BuildSession buildSession;
  building->prepareBuild(noticeStore, 0, true, buildSession);
  auto result = building->addElementToBuild(element, buildSession);
  building->finalizeBuild(noticeStore, 0, buildSession);

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
  PREPARE_SELF(building, Building);

  buildMgr->resetBuild();

  BuildSession buildSession;
  building->prepareBuild(noticeStore, 0, true, buildSession);
  auto result = building->addElementToBuild(element, buildSession);
  building->finalizeBuild(noticeStore, 0, buildSession);

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

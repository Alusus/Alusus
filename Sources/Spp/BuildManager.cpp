/**
 * @file Spp/BuildManager.cpp
 * Contains the implementation of class Spp::BuildManager.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

BuildManager::~BuildManager()
{
  // Reset all build data to avoid possible segmentation faults caused by destructing LLVM objects in the wrong order.
  this->resetBuild(BuildManager::BuildType::JIT);
  this->resetBuild(BuildManager::BuildType::EVAL);
  this->resetBuild(BuildManager::BuildType::OFFLINE);
}


//==============================================================================
// Initialization Functions

void BuildManager::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->prepareBuild,
    &this->addElementToBuild,
    &this->finalizeBuild,
    &this->buildDependencies,
    &this->execute,
    &this->dumpLlvmIrForElement,
    &this->buildObjectFileForElement,
    &this->resetBuild,
    &this->resetBuildData
  });
}


void BuildManager::initBindings()
{
  auto executing = ti_cast<Executing>(this);

  executing->prepareBuild = &BuildManager::_prepareBuild;
  executing->addElementToBuild = &BuildManager::_addElementToBuild;
  executing->finalizeBuild = &BuildManager::_finalizeBuild;
  executing->execute = &BuildManager::_execute;

  this->prepareBuild = &BuildManager::_prepareBuild;
  this->addElementToBuild = &BuildManager::_addElementToBuild;
  this->finalizeBuild = &BuildManager::_finalizeBuild;
  this->buildDependencies = &BuildManager::_buildDependencies;
  this->execute = &BuildManager::_execute;
  this->dumpLlvmIrForElement = &BuildManager::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildManager::_buildObjectFileForElement;
  this->resetBuild = &BuildManager::_resetBuild;
  this->resetBuildData = &BuildManager::_resetBuildData;
}


void BuildManager::initTargets()
{
  this->jitEda.setIdPrefix("jit");
  this->jitBuildTarget = std::make_shared<LlvmCodeGen::JitBuildTarget>(this->globalItemRepo);
  this->jitTargetGenerator = std::make_shared<LlvmCodeGen::TargetGenerator>(this->jitBuildTarget.get(), false);
  this->jitTargetGenerator->setupBuild();

  this->evalEda.setIdPrefix("eval");
  this->evalBuildTarget = std::make_shared<LlvmCodeGen::LazyJitBuildTarget>(this->globalItemRepo);
  this->evalTargetGenerator = std::make_shared<LlvmCodeGen::TargetGenerator>(
    this->jitTargetGenerator.get(), this->evalBuildTarget.get(), true
  );
  this->evalTargetGenerator->setupBuild();

  this->offlineEda.setIdPrefix("ofln");
  this->offlineBuildTarget = std::make_shared<LlvmCodeGen::OfflineBuildTarget>();
  this->offlineTargetGenerator = std::make_shared<LlvmCodeGen::TargetGenerator>(
    this->jitTargetGenerator.get(), this->offlineBuildTarget.get(), false
  );
  this->offlineTargetGenerator->setupBuild();
}


//==============================================================================
// Build Functions

SharedPtr<BuildSession> BuildManager::_prepareBuild(
  TiObject *self, Core::Notices::Store *noticeStore, Int buildType, TiObject *globalFuncElement
) {
  PREPARE_SELF(buildMgr, BuildManager);

  if (buildType == BuildManager::BuildType::OFFLINE) {
      buildMgr->resetBuild(buildType);
  } else if (buildMgr->rootManager->isInteractive()) {
    // If we are running in an interactive mode and we faced previous errors, we'll try to clear the errors and start
    // fresh to give the user a chance to correct the errors if possible.
    auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
    auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
    if ((minSeverity >= 0 && minSeverity <= 1) || (thisMinSeverity >= 0 && thisMinSeverity <= 1)) {
      buildMgr->rootManager->resetMinNoticeSeverityEncountered();
      noticeStore->resetMinEncounteredSeverity();
      buildMgr->resetBuild(buildType);
    }
  }

  CodeGen::ExtraDataAccessor *eda;
  LlvmCodeGen::TargetGenerator *targetGen;
  Word pointerBc;
  Bool offlineExec;
  if (buildType == BuildType::JIT) {
    targetGen = buildMgr->jitTargetGenerator.get();
    eda = &buildMgr->jitEda;
    offlineExec = false;
    pointerBc = buildMgr->jitBuildTarget->getPointerBitCount();
  } else if (buildType == BuildType::EVAL) {
    targetGen = buildMgr->evalTargetGenerator.get();
    eda = &buildMgr->evalEda;
    offlineExec = false;
    pointerBc = buildMgr->evalBuildTarget->getPointerBitCount();
  } else if (buildType == BuildType::OFFLINE) {
    targetGen = buildMgr->offlineTargetGenerator.get();
    eda = &buildMgr->offlineEda;
    offlineExec = true;
    pointerBc = 64; // buildMgr->offlineBuildTarget->getPointerBitCount();
  } else {
    throw EXCEPTION(InvalidArgumentException, S("buildType"), S("Unexpected build type."), buildType);
  }

  buildMgr->funcNameIndex++;
  StrStream globalEntryName;
  globalEntryName << S("__entry__");
  if (!offlineExec) globalEntryName << buildMgr->funcNameIndex;

  // Build the IR code.
  targetGen->setNoticeStore(noticeStore);
  buildMgr->generator->prepareBuild(noticeStore);

  auto targetGeneration = ti_cast<CodeGen::TargetGeneration>(targetGen);

  TioSharedPtr tgContext;
  TioSharedPtr tgFunc;
  if (globalFuncElement != 0) {
    // Prepare the execution function.
    buildMgr->prepareFunction(
      targetGeneration, globalEntryName.str().c_str(), buildMgr->getVoidNoArgsFuncTgType(buildType), tgContext, tgFunc
    );
    eda->setCodeGenData(globalFuncElement, tgContext);
  }

  return std::make_shared<BuildSession>(
    eda, targetGeneration, offlineExec, pointerBc, buildType, tgFunc, tgContext, globalEntryName.str().c_str()
  );
}


Bool BuildManager::_addElementToBuild(TiObject *self, TiObject *element, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  // Generate the statement.
  CodeGen::TerminalStatement terminal;
  Bool result = true;
  if (element->isDerivedFrom<Ast::Module>()) {
    if (!generation->generateModuleInit(static_cast<Ast::Module*>(element), buildSession->getCodeGenSession())) {
      result = false;
    }
    if (buildSession->getCodeGenSession()->isOfflineExecution()) {
      if (!generation->generateModule(static_cast<Ast::Module*>(element), buildSession->getCodeGenSession())) {
        result = false;
      }
    }
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), buildSession->getCodeGenSession());
  } else if (!element->isDerivedFrom<Core::Data::Ast::Bridge>()) {
    result = generation->generateStatement(element, buildSession->getCodeGenSession(), terminal);
    if (!generation->generateVarGroupDestruction(buildSession->getCodeGenSession(), 0)) result = false;
  }
  return result;
}


Bool BuildManager::_finalizeBuild(
  TiObject *self, Core::Notices::Store *noticeStore, TiObject *globalFuncElement, BuildSession *buildSession
) {
  PREPARE_SELF(buildMgr, BuildManager);

  if (globalFuncElement != 0) {
    SharedList<TiObject> args;
    if (!buildSession->getCodeGenSession()->getTg()->finishFunctionBody(
      buildSession->getGlobalEntryTgFunc().get(), buildMgr->getVoidNoArgsFuncTgType(buildSession->getBuildType()),
      &args, buildSession->getGlobalEntryTgContext().get()
    )) {
      throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
    }
    buildSession->getCodeGenSession()->getEda()->removeCodeGenData(globalFuncElement);
  }

  return buildMgr->buildDependencies(noticeStore, buildSession);
}


Bool BuildManager::_execute(TiObject *self, Core::Notices::Store *noticeStore, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
  auto thisMinSeverity = noticeStore->getMinEncounteredSeverity();
  if ((minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
    if (buildSession->getBuildType() == BuildType::JIT) {
      // First run all the constructors. Constructors need to be run in reverse order since the deeper dependencies
      // are generated after the immediate dependencies.
      for (Int i = buildSession->getGlobalCtorNames()->size() - 1; i >= 0; --i) {
        buildMgr->jitBuildTarget->execute(buildSession->getGlobalCtorNames()->at(i).c_str());
      }
      buildMgr->jitBuildTarget->execute(buildSession->getGlobalEntryName().c_str());
    } else if (buildSession->getBuildType() == BuildType::EVAL) {
      // First run all the constructors. Constructors need to be run in reverse order since the deeper dependencies
      // are generated after the immediate dependencies.
      for (Int i = buildSession->getGlobalCtorNames()->size() - 1; i >= 0; --i) {
        buildMgr->evalBuildTarget->execute(buildSession->getGlobalCtorNames()->at(i).c_str());
      }
      buildMgr->evalBuildTarget->execute(buildSession->getGlobalEntryName().c_str());
    } else {
      throw EXCEPTION(InvalidArgumentException, S("buildSession"), S("Unexpected build type."));
    }
    return true;
  } else {
    return false;
  }
}


Bool BuildManager::_buildDependencies(TiObject *self, Core::Notices::Store *noticeStore, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  Bool result = true;

  // Dependencies can themselves have other dependencies, which in turn can also have their own dependencies, so we'll
  // need to loop until there are no more dependencies.
  Int funcDepsIndex = 0;
  Int globalVarInitializationIndex = 0;
  Int globalVarDestructionIndex = 0;
  while (
    funcDepsIndex < buildSession->getFuncDeps()->getCount() ||
    globalVarInitializationIndex < buildSession->getGlobalVarInitializationDeps()->getCount() ||
    globalVarDestructionIndex < buildSession->getGlobalVarDestructionDeps()->getCount()
  ) {
    // Build function dependencies.
    for (; funcDepsIndex < buildSession->getFuncDeps()->getCount(); ++funcDepsIndex) {
      auto astFunc = static_cast<Ast::Function*>(buildSession->getFuncDeps()->get(funcDepsIndex));
      if (!generation->generateFunction(astFunc, buildSession->getCodeGenSession())) result = false;
    }

    // Build global var dependencies.
    if (globalVarInitializationIndex < buildSession->getGlobalVarInitializationDeps()->getCount()) {
      buildMgr->funcNameIndex++;

      // Build the constructor function.
      StrStream ctorNameStream;
      ctorNameStream << S("__constructor__") << buildMgr->funcNameIndex;
      Str ctorName = ctorNameStream.str();
      if (buildMgr->buildGlobalCtorOrDtor(
        buildSession, buildSession->getGlobalVarInitializationDeps(), globalVarInitializationIndex, ctorName.c_str(),
        [=](
          Spp::Ast::Type *varAstType, TiObject *varTgRef, Core::Data::Node *varAstNode, TiObject *astParams,
          CodeGen::Session *childSession
        )->Bool {
          SharedList<TiObject> initTgVals;
          PlainList<TiObject> initAstTypes;
          PlainList<TiObject> initAstNodes;
          if (astParams != 0) {
            if (!buildMgr->generator->getExpressionGenerator()->generateParams(
              astParams, generation, childSession, &initAstNodes, &initAstTypes, &initTgVals
            )) return false;
          }
          return generation->generateVarInitialization(
            varAstType, varTgRef, varAstNode, &initAstNodes, &initAstTypes, &initTgVals, childSession
          );
        }
      )) {
        buildSession->getGlobalCtorNames()->push_back(ctorName);
      } else {
        result = false;
      }

      globalVarInitializationIndex = buildSession->getGlobalVarInitializationDeps()->getCount();
    }

    if (globalVarDestructionIndex < buildSession->getGlobalVarDestructionDeps()->getCount()) {
      buildMgr->funcNameIndex++;

      // Build the destructor function.
      StrStream dtorNameStream;
      dtorNameStream << S("__destructor__") << buildMgr->funcNameIndex;
      Str dtorName = dtorNameStream.str();
      if (buildMgr->buildGlobalCtorOrDtor(
        buildSession, buildSession->getGlobalVarDestructionDeps(), globalVarDestructionIndex, dtorName.c_str(),
        [=](
          Spp::Ast::Type *varAstType, TiObject *varTgRef, Core::Data::Node *varAstNode, TiObject *astParams,
          CodeGen::Session *childSession
        )->Bool {
          return generation->generateVarDestruction(varAstType, varTgRef, varAstNode, childSession);
        }
      )) {
        buildSession->getGlobalDtorNames()->push_back(dtorName);
      } else {
        result = false;
      }

      globalVarDestructionIndex = buildSession->getGlobalVarDestructionDeps()->getCount();
    }
  }

  return result;
}


Bool BuildManager::buildGlobalCtorOrDtor(
  BuildSession *buildSession, DependencyList<Core::Data::Node> *deps, Int depsIndex, Char const *funcName,
  std::function<Bool(
    Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode, TiObject *astParams,
    CodeGen::Session *session
  )> varOpCallback
) {
  auto generation = ti_cast<CodeGen::Generation>(this->generator);

  // Prepare the function.
  TioSharedPtr tgContext;
  TioSharedPtr tgFunc;
  this->prepareFunction(
    buildSession->getCodeGenSession()->getTg(), funcName, this->getVoidNoArgsFuncTgType(buildSession->getBuildType()),
    tgContext, tgFunc
  );
  CodeGen::DestructionStack destructionStack;
  CodeGen::Session session(buildSession->getCodeGenSession(), tgContext.get(), &destructionStack);

  Bool result = true;

  for (; depsIndex < deps->getCount(); ++depsIndex) {
    auto astVar = deps->get(depsIndex);
    TiObject *tgVar = session.getEda()->getCodeGenData<TiObject>(astVar);

    // Get initialization params, if any.
    TiObject *astTypeRef = astVar;
    TiObject *astParams = 0;
    auto astParamPass = ti_cast<Core::Data::Ast::ParamPass>(astVar);
    if (astParamPass != 0 && astParamPass->getType() == Core::Data::Ast::BracketType::ROUND) {
      astTypeRef = astParamPass->getOperand().get();
      astParams = astParamPass->getParam().get();
    }

    // Get the type of the variable.
    TiObject *tgType;
    Ast::Type *astType;
    if (!generation->getGeneratedType(astTypeRef, &session, tgType, &astType)) {
      result = false;
      continue;
    }

    // Init the var
    TioSharedPtr tgVarRef;
    if (!session.getTg()->generateVarReference(tgContext.get(), tgType, tgVar, tgVarRef)) {
      result = false;
      continue;
    }
    if (!varOpCallback(astType, tgVarRef.get(), astVar, astParams, &session)) {
      result = false;
      continue;
    }
  }

  // Finalize function.
  SharedList<TiObject> args;
  if (!buildSession->getCodeGenSession()->getTg()->finishFunctionBody(
    tgFunc.get(), this->getVoidNoArgsFuncTgType(buildSession->getBuildType()), &args, tgContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }

  return result;
}


void BuildManager::_dumpLlvmIrForElement(
  TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(buildMgr, BuildManager);

  TiObject *globalFuncElement = 0;
  if (element->isDerivedFrom<Ast::Module>()) globalFuncElement = element;

  SharedPtr<BuildSession> buildSession = buildMgr->prepareBuild(
    noticeStore, BuildManager::BuildType::OFFLINE, globalFuncElement
  );
  auto result = buildMgr->addElementToBuild(element, buildSession.get());
  if (!buildMgr->finalizeBuild(noticeStore, globalFuncElement, buildSession.get())) result = false;

  if (element->isDerivedFrom<Ast::Module>()) {
    buildSession->getGlobalCtorNames()->insert(
      buildSession->getGlobalCtorNames()->begin(), buildSession->getGlobalEntryName()
    );
  }

  // Dump the IR code.
  Str ir = buildMgr->offlineBuildTarget->generateLlvmIr(
    buildSession->getGlobalCtorNames(), buildSession->getGlobalDtorNames()
  );
  if (result) {
    outStream << S("-------------------- Generated LLVM IR ---------------------\n");
    outStream << ir;
    outStream << S("------------------------------------------------------------\n");
  } else {
    parser->flushApprovedNotices();
    outStream << S("Build Failed...\n");
    outStream << S("--------------------- Partial LLVM IR ----------------------\n");
    outStream << ir;
    outStream << S("------------------------------------------------------------\n");
  }
}


Bool BuildManager::_buildObjectFileForElement(
  TiObject *self, TiObject *element, Char const *objectFilename, Core::Notices::Store *noticeStore,
  Core::Processing::Parser *parser
) {
  VALIDATE_NOT_NULL(element, noticeStore);
  PREPARE_SELF(buildMgr, BuildManager);

  TiObject *globalFuncElement = 0;
  if (element->isDerivedFrom<Ast::Module>()) globalFuncElement = element;

  SharedPtr<BuildSession> buildSession = buildMgr->prepareBuild(
    noticeStore, BuildManager::BuildType::OFFLINE, globalFuncElement
  );
  auto result = buildMgr->addElementToBuild(element, buildSession.get());
  if (!buildMgr->finalizeBuild(noticeStore, globalFuncElement, buildSession.get())) result = false;

  if (element->isDerivedFrom<Ast::Module>()) {
    buildSession->getGlobalCtorNames()->insert(
      buildSession->getGlobalCtorNames()->begin(), buildSession->getGlobalEntryName()
    );
  }

  if (result) {
    buildMgr->offlineBuildTarget->generateObjectFile(
      objectFilename, buildSession->getGlobalCtorNames(), buildSession->getGlobalDtorNames()
    );
  }

  return result;
}


void BuildManager::_resetBuild(TiObject *self, Int buildType)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto root = buildMgr->rootManager->getRootScope().get();
  if (buildType == BuildType::JIT) {
    buildMgr->resetBuildData(root, &buildMgr->jitEda);
    buildMgr->jitTargetGenerator->setupBuild();
    buildMgr->jitGlobalTgFuncType = TioSharedPtr::null;
  } else if (buildType == BuildType::EVAL) {
    buildMgr->resetBuildData(root, &buildMgr->evalEda);
    buildMgr->evalTargetGenerator->setupBuild();
    buildMgr->evalGlobalTgFuncType = TioSharedPtr::null;
  } else if (buildType == BuildType::OFFLINE) {
    buildMgr->resetBuildData(root, &buildMgr->offlineEda);
    buildMgr->offlineTargetGenerator->setupBuild();
    buildMgr->offlineGlobalTgFuncType = TioSharedPtr::null;
  } else {
    throw EXCEPTION(InvalidArgumentException, S("buildType"), S("Unexpected build type."), buildType);
  }
}


void BuildManager::_resetBuildData(TiObject *self, TiObject *obj, CodeGen::ExtraDataAccessor *eda)
{
  if (obj == 0) return;
  if (obj->isDerivedFrom<Core::Data::Grammar::Module>()) return;

  PREPARE_SELF(buildMgr, BuildManager);

  auto metahaving = ti_cast<Core::Data::Ast::MetaHaving>(obj);
  if (metahaving != 0) {
    eda->removeCodeGenData(metahaving);
    eda->removeAutoCtor(metahaving);
    eda->removeAutoDtor(metahaving);
    eda->resetCodeGenFailed(metahaving);
    eda->resetInitStatementsGenIndex(metahaving);
  }

  auto container = ti_cast<Core::Basic::Containing<TiObject>>(obj);
  if (container != 0) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      buildMgr->resetBuildData(container->getElement(i), eda);
    }
  }

  auto binding = ti_cast<Core::Basic::Binding>(obj);
  if (binding != 0) {
    for (Int i = 0; i < binding->getMemberCount(); ++i) {
      buildMgr->resetBuildData(binding->getMember(i), eda);
    }
  }

  auto tpl = ti_cast<Ast::Template>(obj);
  if (tpl != 0) {
    for (Int i = 0; i < tpl->getInstanceCount(); ++i) {
      buildMgr->resetBuildData(tpl->getInstance(i).get(), eda);
    }
  }
}


//==============================================================================
// Helper Functions


TiObject* BuildManager::getVoidNoArgsFuncTgType(Int buildType)
{
  if (buildType == BuildType::JIT) {
    if (this->jitGlobalTgFuncType == 0) {
      this->jitGlobalTgFuncType = this->createVoidNoArgsFuncTgType(this->jitTargetGenerator.get());
    }
    return this->jitGlobalTgFuncType.get();
  } else if (buildType == BuildType::EVAL) {
    if (this->evalGlobalTgFuncType == 0) {
      this->evalGlobalTgFuncType = this->createVoidNoArgsFuncTgType(this->evalTargetGenerator.get());
    }
    return this->evalGlobalTgFuncType.get();
  } else if (buildType == BuildType::OFFLINE) {
    if (this->offlineGlobalTgFuncType == 0) {
      this->offlineGlobalTgFuncType = this->createVoidNoArgsFuncTgType(this->offlineTargetGenerator.get());
    }
    return this->offlineGlobalTgFuncType.get();
  } else {
    throw EXCEPTION(InvalidArgumentException, S("buildType"), S("Unexpected build type."), buildType);
  }
}


TioSharedPtr BuildManager::createVoidNoArgsFuncTgType(LlvmCodeGen::TargetGenerator *targetGen)
{
  // Generate function type.
  TioSharedPtr tgVoidType;
  if (!targetGen->generateVoidType(tgVoidType)) {
    throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
  }
  SharedMap<TiObject> argTypes;
  TioSharedPtr tgFuncType;
  if (!targetGen->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
    throw EXCEPTION(GenericException, S("Failed to generate function type for root scope execution."));
  }
  return tgFuncType;
}


void BuildManager::prepareFunction(
  CodeGen::TargetGeneration *targetGen, Char const *funcName, TiObject *tgFuncType,
  TioSharedPtr &context, TioSharedPtr &tgFunc
) {
  if (!targetGen->generateFunctionDecl(funcName, tgFuncType, tgFunc)) {
    throw EXCEPTION(GenericException, S("Failed to generate function declaration for root scope execution."));
  }
  SharedList<TiObject> args;
  if (!targetGen->prepareFunctionBody(
    tgFunc.get(), tgFuncType, &args, context)
  ) {
    throw EXCEPTION(GenericException, S("Failed to prepare function body for root scope execution."));
  }
}

} // namespace

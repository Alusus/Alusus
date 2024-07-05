/**
 * @file Spp/BuildManager.cpp
 * Contains the implementation of class Spp::BuildManager.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
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
  this->resetBuild(this->jitBuildSession.get());
  this->resetBuild(this->preprocessBuildSession.get());
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
    &this->resetBuildData,
    &this->computeResultType
  });
}


void BuildManager::initBindings()
{
  auto executing = ti_cast<Executing>(this);
  executing->prepareBuild = &BuildManager::_prepareBuild;
  executing->addElementToBuild = &BuildManager::_addElementToBuild;
  executing->finalizeBuild = &BuildManager::_finalizeBuild;
  executing->execute = &BuildManager::_execute;

  auto expressionComputation = ti_cast<ExpressionComputation>(this);
  expressionComputation->computeResultType = &BuildManager::_computeResultType;

  this->prepareBuild = &BuildManager::_prepareBuild;
  this->addElementToBuild = &BuildManager::_addElementToBuild;
  this->finalizeBuild = &BuildManager::_finalizeBuild;
  this->buildDependencies = &BuildManager::_buildDependencies;
  this->execute = &BuildManager::_execute;
  this->dumpLlvmIrForElement = &BuildManager::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildManager::_buildObjectFileForElement;
  this->resetBuild = &BuildManager::_resetBuild;
  this->resetBuildData = &BuildManager::_resetBuildData;
  this->computeResultType = &BuildManager::_computeResultType;
}


void BuildManager::initNonOfflineBuildSessions()
{
  auto jitBuildTarget = newSrdObj<LlvmCodeGen::JitBuildTarget>(this->globalItemRepo);
  auto jitTargetGenerator = newSrdObj<LlvmCodeGen::TargetGenerator>(
    this->rootManager, jitBuildTarget.get(), false
  );
  jitTargetGenerator->setupBuild();
  this->jitBuildSession = newSrdObj<BuildSession>(
    jitTargetGenerator, jitBuildTarget, false, BuildManager::BuildType::JIT
  );
  this->jitBuildSession->getExtraDataAccessor()->setIdPrefix("jit");

  auto preprocessBuildTarget = newSrdObj<LlvmCodeGen::LazyJitBuildTarget>(this->globalItemRepo);
  auto preprocessTargetGenerator = newSrdObj<LlvmCodeGen::TargetGenerator>(
    jitTargetGenerator.get(), preprocessBuildTarget.get(), true
  );
  preprocessTargetGenerator->setupBuild();
  this->preprocessBuildSession = newSrdObj<BuildSession>(
    preprocessTargetGenerator, preprocessBuildTarget, false, BuildManager::BuildType::PREPROCESS
  );
  this->preprocessBuildSession->getExtraDataAccessor()->setIdPrefix("preprc");
}


SharedPtr<BuildSession> BuildManager::createOfflineBuildSession(Char const *targetTriple)
{
  static LongInt idPrefixCounter(0);
  ++idPrefixCounter;
  auto buildTarget = newSrdObj<LlvmCodeGen::OfflineBuildTarget>();
  buildTarget->setTargetTriple(targetTriple);
  auto targetGenerator = newSrdObj<LlvmCodeGen::TargetGenerator>(
    this->jitBuildSession->getTargetGenerator().get(), buildTarget.get(), false
  );
  targetGenerator->setupBuild();
  auto buildSession = newSrdObj<BuildSession>(targetGenerator, buildTarget, true, BuildManager::BuildType::OFFLINE);
  buildSession->getExtraDataAccessor()->setIdPrefix(Str("ofln") + idPrefixCounter);
  return buildSession;
}


//==============================================================================
// Build Functions

SharedPtr<BuildSession> BuildManager::_prepareBuild(
  TiObject *self, Int buildType, Char const *targetTriple, TiObject *globalFuncElement
) {
  PREPARE_SELF(buildMgr, BuildManager);

  // Create the session.
  SharedPtr<BuildSession> buildSession;
  if (buildType == BuildManager::BuildType::JIT) {
    buildSession = newSrdObj<BuildSession>(buildMgr->jitBuildSession.get());
  } else if (buildType == BuildManager::BuildType::PREPROCESS) {
    buildSession = newSrdObj<BuildSession>(buildMgr->preprocessBuildSession.get());
  } else if (buildType == BuildManager::BuildType::OFFLINE) {
    buildSession = buildMgr->createOfflineBuildSession(targetTriple);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("buildType"), S("Invalid build type"), buildType);
  }

  if (buildSession->getBuildType() != BuildManager::BuildType::OFFLINE && buildMgr->rootManager->isInteractive()) {
    // If we are running in an interactive mode and we faced previous errors, we'll try to clear the errors and start
    // fresh to give the user a chance to correct the errors if possible.
    auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
    auto thisMinSeverity = buildMgr->rootManager->getNoticeStore()->getMinEncounteredSeverity();
    if ((minSeverity >= 0 && minSeverity <= 1) || (thisMinSeverity >= 0 && thisMinSeverity <= 1)) {
      buildMgr->rootManager->resetMinNoticeSeverityEncountered();
      buildMgr->rootManager->getNoticeStore()->resetMinEncounteredSeverity();
      buildMgr->resetBuild(buildSession.get());
    }
  }

  buildMgr->funcNameIndex++;
  StrStream globalEntryName;
  globalEntryName << S("__entry__");
  if (!buildSession->getCodeGenSession()->isOfflineExecution()) globalEntryName << buildMgr->funcNameIndex;

  buildMgr->rootManager->getNoticeStore()->clearPrefixSourceLocationStack();
  buildMgr->astHelper->prepare();

  auto targetGeneration = buildSession->getTargetGenerator().ti_cast_get<CodeGen::TargetGeneration>();

  TioSharedPtr tgContext;
  TioSharedPtr tgFunc;
  if (globalFuncElement != 0) {
    // Prepare the execution function.
    buildMgr->prepareFunction(
      targetGeneration, globalEntryName.str().c_str(), buildMgr->getVoidNoArgsFuncTgType(buildSession.get()),
      tgContext, tgFunc
    );
  }

  buildSession->setGlobalEntryName(globalEntryName.str().c_str());
  buildSession->setGlobalEntryTgFunc(tgFunc);
  buildSession->setGlobalEntryTgContext(tgContext);
  return buildSession;
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
  }
  return result;
}


Bool BuildManager::_finalizeBuild(TiObject *self, TiObject *globalFuncElement, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  Bool result = true;
  if (!generation->generateVarGroupDestruction(buildSession->getCodeGenSession(), 0)) result = false;

  if (globalFuncElement != 0) {
    SharedList<TiObject> args;
    if (!buildSession->getCodeGenSession()->getTg()->finishFunctionBody(
      buildSession->getGlobalEntryTgFunc().get(), buildMgr->getVoidNoArgsFuncTgType(buildSession),
      &args, buildSession->getGlobalEntryTgContext().get()
    )) {
      throw EXCEPTION(GenericException, S("Failed to finalize function body for root scope statement."));
    }
  }

  if (!buildMgr->buildDependencies(buildSession)) result = false;

  buildMgr->rootManager->flushNotices();

  return result;
}


Bool BuildManager::_execute(TiObject *self, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
  auto thisMinSeverity = buildMgr->rootManager->getNoticeStore()->getMinEncounteredSeverity();
  if ((minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
    if (buildSession->getBuildType() == BuildType::JIT) {
      // First run all the constructors. Constructors need to be run in reverse order since the deeper dependencies
      // are generated after the immediate dependencies.
      while (buildSession->getDepsInfo()->globalCtors.getLength() > 0) {
        auto index = buildSession->getDepsInfo()->globalCtors.getLength() - 1;
        buildSession->getBuildTarget().s_cast<LlvmCodeGen::JitBuildTarget>()->execute(
          buildSession->getDepsInfo()->globalCtors(index).name
        );
        buildMgr->markGlobalItemsAsInitialized(buildSession->getDepsInfo()->globalCtors(index).initializedVarNames);
        buildSession->getDepsInfo()->globalCtors.remove(index);
      }
      buildSession->getBuildTarget().s_cast<LlvmCodeGen::JitBuildTarget>()->execute(buildSession->getGlobalEntryName());
    } else if (buildSession->getBuildType() == BuildType::PREPROCESS) {
      // First run all the constructors. Constructors need to be run in reverse order since the deeper dependencies
      // are generated after the immediate dependencies.
      while (buildSession->getDepsInfo()->globalCtors.getLength() > 0) {
        auto index = buildSession->getDepsInfo()->globalCtors.getLength() - 1;
        buildSession->getBuildTarget().s_cast<LlvmCodeGen::LazyJitBuildTarget>()->execute(
          buildSession->getDepsInfo()->globalCtors(index).name
        );
        buildMgr->markGlobalItemsAsInitialized(buildSession->getDepsInfo()->globalCtors(index).initializedVarNames);
        buildSession->getDepsInfo()->globalCtors.remove(index);
      }
      buildSession->getBuildTarget().s_cast<LlvmCodeGen::LazyJitBuildTarget>()->execute(
        buildSession->getGlobalEntryName()
      );
    } else {
      throw EXCEPTION(InvalidArgumentException, S("buildSession"), S("Unexpected build type."));
    }
    return true;
  } else {
    return false;
  }
}


Bool BuildManager::_buildDependencies(TiObject *self, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  Bool result = true;

  // Dependencies can themselves have other dependencies, which in turn can also have their own dependencies, so we'll
  // need to loop until there are no more dependencies.
  while (
    buildSession->getDepsInfo()->funcDeps.getCount() > 0 ||
    buildSession->getDepsInfo()->globalVarInitializationDeps.getCount() > 0 ||
    buildSession->getDepsInfo()->globalVarDestructionDeps.getCount() > 0
  ) {
    // Build function dependencies.
    while (buildSession->getDepsInfo()->funcDeps.getCount() > 0) {
      auto astFunc = static_cast<Ast::Function*>(buildSession->getDepsInfo()->funcDeps.get(0));
      buildSession->getDepsInfo()->funcDeps.remove(0);
      if (!generation->generateFunction(astFunc, buildSession->getCodeGenSession())) result = false;
    }

    // Build global var dependencies.
    if (buildSession->getDepsInfo()->globalVarInitializationDeps.getCount() > 0) {
      buildMgr->funcNameIndex++;

      // Build the constructor function.
      StrStream ctorNameStream;
      ctorNameStream << S("__constructor__") << buildMgr->funcNameIndex;
      GlobalCtorDtorInfo ctorInfo;
      ctorInfo.name = ctorNameStream.str().c_str();
      if (buildMgr->buildGlobalCtorOrDtor(
        buildSession, &buildSession->getDepsInfo()->globalVarInitializationDeps, ctorInfo.name,
        [=,&ctorInfo](
          Spp::Ast::Type *varAstType, TiObject *varTgRef, Core::Data::Node *varAstNode, TiObject *astParams,
          CodeGen::Session *childSession
        )->Bool {
          childSession->getDestructionStack()->pushScope();

          SharedList<TiObject> initTgVals;
          PlainList<TiObject> initAstTypes;
          PlainList<TiObject> initAstNodes;
          if (astParams != 0) {
            if (!buildMgr->generator->getExpressionGenerator()->generateParams(
              astParams, generation, childSession, &initAstNodes, &initAstTypes, &initTgVals
            )) {
              childSession->getDestructionStack()->popScope();
              return false;
            }
          }
          if (!generation->generateVarInitialization(
            varAstType, varTgRef, varAstNode, &initAstNodes, &initAstTypes, &initTgVals, childSession
          )) {
            childSession->getDestructionStack()->popScope();
            return false;
          };

          if (!generation->generateVarGroupDestruction(
            childSession, childSession->getDestructionStack()->getScopeStartIndex(-1)
          )) {
            childSession->getDestructionStack()->popScope();
            return false;
          }
          childSession->getDestructionStack()->popScope();

          // Add the name of this var to the ctor info.
          Str name = S("!");
          name += buildMgr->getAstHelper()->resolveNodePath(varAstNode->getOwner());
          ctorInfo.initializedVarNames.add(name);

          return true;
        }
      )) {
        buildSession->getDepsInfo()->globalCtors.add(ctorInfo);
      } else {
        result = false;
      }
    }

    if (buildSession->getDepsInfo()->globalVarDestructionDeps.getCount() > 0) {
      buildMgr->funcNameIndex++;

      // Build the destructor function.
      StrStream dtorNameStream;
      dtorNameStream << S("__destructor__") << buildMgr->funcNameIndex;
      GlobalCtorDtorInfo dtorInfo;
      dtorInfo.name = dtorNameStream.str().c_str();
      if (buildMgr->buildGlobalCtorOrDtor(
        buildSession, &buildSession->getDepsInfo()->globalVarDestructionDeps, dtorInfo.name,
        [=](
          Spp::Ast::Type *varAstType, TiObject *varTgRef, Core::Data::Node *varAstNode, TiObject *astParams,
          CodeGen::Session *childSession
        )->Bool {
          return generation->generateVarDestruction(varAstType, varTgRef, varAstNode, childSession);
        }
      )) {
        buildSession->getDepsInfo()->globalDtors.add(dtorInfo);
      } else {
        result = false;
      }
    }
  }

  return result;
}


Bool BuildManager::buildGlobalCtorOrDtor(
  BuildSession *buildSession, DependencyList<Core::Data::Node> *deps, Char const *funcName,
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
    buildSession->getCodeGenSession()->getTg(), funcName, this->getVoidNoArgsFuncTgType(buildSession),
    tgContext, tgFunc
  );
  CodeGen::DestructionStack destructionStack;
  CodeGen::Session session(buildSession->getCodeGenSession(), tgContext.get(), tgContext.get(), &destructionStack);

  Bool result = true;

  while (deps->getCount() > 0) {
    auto astVar = deps->get(0);
    deps->remove(0);
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
    tgFunc.get(), this->getVoidNoArgsFuncTgType(buildSession), &args, tgContext.get()
  )) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for root constructor."));
  }

  return result;
}


void BuildManager::_dumpLlvmIrForElement(TiObject *self, TiObject *element)
{
  VALIDATE_NOT_NULL(element);
  PREPARE_SELF(buildMgr, BuildManager);

  TiObject *globalFuncElement = 0;
  if (element->isDerivedFrom<Ast::Module>()) globalFuncElement = element;

  SharedPtr<BuildSession> buildSession = buildMgr->prepareBuild(BuildManager::BuildType::OFFLINE, 0, globalFuncElement);
  auto result = buildMgr->addElementToBuild(element, buildSession.get());
  if (!buildMgr->finalizeBuild(globalFuncElement, buildSession.get())) result = false;

  if (element->isDerivedFrom<Ast::Module>()) {
    buildSession->getDepsInfo()->globalCtors.insert(0, GlobalCtorDtorInfo(buildSession->getGlobalEntryName()));
  }

  Array<Str> globalCtorNames = BuildManager::getGlobalCtorNames(buildSession->getDepsInfo().get());
  Array<Str> globalDtorNames = BuildManager::getGlobalDtorNames(buildSession->getDepsInfo().get());

  // Dump the IR code.
  Str ir = buildSession->getBuildTarget().s_cast<LlvmCodeGen::OfflineBuildTarget>()->generateLlvmIr(
    &globalCtorNames, &globalDtorNames
  );
  if (result) {
    outStream << S("-------------------- Generated LLVM IR ---------------------\n");
    outStream << ir;
    outStream << S("------------------------------------------------------------\n");
  } else {
    outStream << S("Build Failed...\n");
    outStream << S("--------------------- Partial LLVM IR ----------------------\n");
    outStream << ir;
    outStream << S("------------------------------------------------------------\n");
  }

  buildMgr->resetBuild(buildSession.get());
}


Bool BuildManager::_buildObjectFileForElement(
  TiObject *self, TiObject *element, Char const *objectFilename, Char const *targetTriple
) {
  VALIDATE_NOT_NULL(element);
  PREPARE_SELF(buildMgr, BuildManager);

  TiObject *globalFuncElement = 0;
  if (element->isDerivedFrom<Ast::Module>()) globalFuncElement = element;

  SharedPtr<BuildSession> buildSession = buildMgr->prepareBuild(
    BuildManager::BuildType::OFFLINE, targetTriple, globalFuncElement
  );
  auto result = buildMgr->addElementToBuild(element, buildSession.get());
  if (!buildMgr->finalizeBuild(globalFuncElement, buildSession.get())) result = false;

  if (element->isDerivedFrom<Ast::Module>()) {
    buildSession->getDepsInfo()->globalCtors.insert(0, GlobalCtorDtorInfo(buildSession->getGlobalEntryName()));
  }

  if (result) {
    Array<Str> globalCtorNames = BuildManager::getGlobalCtorNames(buildSession->getDepsInfo().get());
    Array<Str> globalDtorNames = BuildManager::getGlobalDtorNames(buildSession->getDepsInfo().get());
    buildSession->getBuildTarget().s_cast<LlvmCodeGen::OfflineBuildTarget>()->generateObjectFile(
      objectFilename, &globalCtorNames, &globalDtorNames
    );
  }

  buildMgr->resetBuild(buildSession.get());

  return result;
}


void BuildManager::_resetBuild(TiObject *self, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto root = buildMgr->rootManager->getRootScope().get();
  buildMgr->resetBuildData(root, buildSession->getExtraDataAccessor().get());
  buildSession->getTargetGenerator()->setupBuild();
  buildSession->setGlobalEntryName(0);
  buildSession->setGlobalEntryTgFunc(TioSharedPtr::null);
  buildSession->setGlobalEntryTgContext(TioSharedPtr::null);
  buildSession->setVoidNoArgsFuncTgType(TioSharedPtr::null);
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
    eda->removeAutoCtorType(metahaving);
    eda->removeAutoDtor(metahaving);
    eda->removeAutoDtorType(metahaving);
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


Bool BuildManager::_computeResultType(TiObject *self, TiObject *astNode, TiObject *&result, Bool &resultIsValue)
{
  PREPARE_SELF(buildMgr, BuildManager);

  BuildSession buildSession(buildMgr->preprocessBuildSession.get(), newSrdObj<DependencyInfo>());

  buildMgr->rootManager->getNoticeStore()->clearPrefixSourceLocationStack();
  buildMgr->astHelper->prepare();

  CodeGen::GenResult genResult;

  CodeGen::TerminalStatement terminal;
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);
  if (!generation->generateExpression(astNode, buildSession.getCodeGenSession(), genResult, terminal)) {
    buildMgr->rootManager->flushNotices();
    return false;
  }

  if (genResult.astType != 0) {
    result = genResult.astType;
    resultIsValue = true;
  } else {
    result = genResult.astNode;
    resultIsValue = false;
  }

  return true;
}


//==============================================================================
// Helper Functions


TiObject* BuildManager::getVoidNoArgsFuncTgType(BuildSession *buildSession)
{
  if (buildSession->getVoidNoArgsFuncTgType() == 0) {
    buildSession->setVoidNoArgsFuncTgType(this->createVoidNoArgsFuncTgType(buildSession->getTargetGenerator().get()));
  }
  return buildSession->getVoidNoArgsFuncTgType().get();
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


void BuildManager::markGlobalItemsAsInitialized(Array<Str> const &initializedVarNames)
{
  for (Int i = 0; i < initializedVarNames.getLength(); ++i) {
    Int itemIndex = this->globalItemRepo->findItem(initializedVarNames(i));
    this->globalItemRepo->setItemInitialized(itemIndex);
  }
}


Array<Str> BuildManager::getGlobalCtorNames(DependencyInfo *info)
{
  Array<Str> globalCtorNames;
  for (Int i = 0; i < info->globalCtors.getLength(); ++i) {
    globalCtorNames.add(info->globalCtors(i).name);
  }
  return globalCtorNames;
}


Array<Str> BuildManager::getGlobalDtorNames(DependencyInfo *info)
{
  Array<Str> globalDtorNames;
  for (Int i = 0; i < info->globalDtors.getLength(); ++i) {
    globalDtorNames.add(info->globalDtors(i).name);
  }
  return globalDtorNames;
}

} // namespace

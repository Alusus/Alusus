/**
 * @file Spp/BuildManager.cpp
 * Contains the implementation of class Spp::BuildManager.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
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
    &this->prepareExecutionEntry,
    &this->finalizeExecutionEntry,
    &this->addElementToBuild,
    &this->addElementToExecutionEntry,
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
  executing->prepareExecutionEntry = &BuildManager::_prepareExecutionEntry;
  executing->finalizeExecutionEntry = &BuildManager::_finalizeExecutionEntry;
  executing->addElementToExecutionEntry = &BuildManager::_addElementToExecutionEntry;
  executing->execute = &BuildManager::_execute;

  auto expressionComputation = ti_cast<ExpressionComputation>(this);
  expressionComputation->computeResultType = &BuildManager::_computeResultType;

  this->prepareBuild = &BuildManager::_prepareBuild;
  this->prepareExecutionEntry = &BuildManager::_prepareExecutionEntry;
  this->finalizeExecutionEntry = &BuildManager::_finalizeExecutionEntry;
  this->addElementToBuild = &BuildManager::_addElementToBuild;
  this->addElementToExecutionEntry = &BuildManager::_addElementToExecutionEntry;
  this->execute = &BuildManager::_execute;
  this->dumpLlvmIrForElement = &BuildManager::_dumpLlvmIrForElement;
  this->buildObjectFileForElement = &BuildManager::_buildObjectFileForElement;
  this->resetBuild = &BuildManager::_resetBuild;
  this->resetBuildData = &BuildManager::_resetBuildData;
  this->computeResultType = &BuildManager::_computeResultType;
}


void BuildManager::initNonOfflineBuildSessions()
{
  // Prepare build targets and target generators.

  auto jitBuildTarget = newSrdObj<LlvmCodeGen::JitBuildTarget>(this->globalItemRepo);
  auto jitTargetGenerator = newSrdObj<LlvmCodeGen::TargetGenerator>(
    this->rootManager, jitBuildTarget.get(), false
  );
  jitTargetGenerator->setupBuild();

  auto preprocessBuildTarget = newSrdObj<LlvmCodeGen::LazyJitBuildTarget>(this->globalItemRepo);
  auto preprocessTargetGenerator = newSrdObj<LlvmCodeGen::TargetGenerator>(
    jitTargetGenerator.get(), preprocessBuildTarget.get(), true
  );
  preprocessTargetGenerator->setupBuild();

  // Prepare sessions.

  this->preprocessBuildSession = newSrdObj<BuildSession>(
    ++this->buildIdCounter, BuildManager::BuildType::PREPROCESS, false,
    preprocessTargetGenerator, preprocessBuildTarget,
    newSrdObj<Array<CodeGen::GlobalCtorDtorInfo>>(), newSrdObj<Array<CodeGen::GlobalCtorDtorInfo>>(),
    (BuildSession*)0
  );
  this->preprocessBuildSession->getExtraDataAccessor()->setIdPrefix("preprc");

  this->jitBuildSession = newSrdObj<BuildSession>(
    ++this->buildIdCounter, BuildManager::BuildType::JIT, false, jitTargetGenerator, jitBuildTarget,
    this->preprocessBuildSession->getGlobalCtors(),
    this->preprocessBuildSession->getGlobalDtors(),
    this->preprocessBuildSession.get()
  );
  this->jitBuildSession->getExtraDataAccessor()->setIdPrefix("jit");
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
  auto buildSession = newSrdObj<BuildSession>(
    ++this->buildIdCounter, BuildManager::BuildType::OFFLINE, true, targetGenerator, buildTarget,
    newSrdObj<Array<CodeGen::GlobalCtorDtorInfo>>(), newSrdObj<Array<CodeGen::GlobalCtorDtorInfo>>(),
    (BuildSession*)0
  );
  buildSession->getExtraDataAccessor()->setIdPrefix(Str("ofln") + idPrefixCounter);
  return buildSession;
}


//==============================================================================
// Build Functions

SharedPtr<BuildSession> BuildManager::_prepareBuild(TiObject *self, Int buildType, Char const *targetTriple)
{
  PREPARE_SELF(buildMgr, BuildManager);

  // Create the session.
  SharedPtr<BuildSession> buildSession;
  if (buildType == BuildManager::BuildType::JIT) {
    buildSession = newSrdObj<BuildSession>(++buildMgr->buildIdCounter, buildMgr->jitBuildSession.get());
  } else if (buildType == BuildManager::BuildType::PREPROCESS) {
    buildSession = newSrdObj<BuildSession>(++buildMgr->buildIdCounter, buildMgr->preprocessBuildSession.get());
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

  buildMgr->rootManager->getNoticeStore()->clearPrefixSourceLocationStack();
  buildMgr->astHelper->prepare();

  return buildSession;
}


void BuildManager::_prepareExecutionEntry(TiObject *self, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  buildMgr->funcNameIndex++;
  StrStream executionEntryName;
  executionEntryName << S("__entry__") << buildMgr->funcNameIndex;

  TioSharedPtr tgContext;
  TioSharedPtr tgFunc;
  generation->prepareNullaryProcedure(
    executionEntryName.str().c_str(), buildSession->getCodeGenSession(), tgFunc, tgContext
  );

  buildSession->setExecutionEntryName(executionEntryName.str().c_str());
  buildSession->setExecutionEntryTgFunc(tgFunc);
  buildSession->setExecutionEntryTgContext(tgContext);
}


Bool BuildManager::_finalizeExecutionEntry(TiObject *self, BuildSession *buildSession)
{
  if (buildSession->getExecutionEntryTgFunc() == 0 || buildSession->getExecutionEntryTgContext() == 0) {
    throw EXCEPTION(
      GenericException,
      S("BuildManager::finalizeExecutionEntry called without BuildManager::prepareExecutionEntry.")
    );
  }

  PREPARE_SELF(buildMgr, BuildManager);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  Bool result = true;
  if (!generation->generateVarGroupDestruction(buildSession->getCodeGenSession(), 0)) result = false;

  generation->finalizeNullaryProcedure(
    buildSession->getCodeGenSession(),
    buildSession->getExecutionEntryTgFunc().get(),
    buildSession->getExecutionEntryTgContext().get()
  );

  if (!generation->buildDependencies(buildSession->getCodeGenSession())) result = false;
  
  buildMgr->rootManager->flushNotices();

  return result;
}


Bool BuildManager::_addElementToBuild(TiObject *self, TiObject *element, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  // Generate the statement.
  CodeGen::TerminalStatement terminal;
  Bool result = true;
  if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModule(static_cast<Ast::Module*>(element), buildSession->getCodeGenSession());
  } else if (element->isDerivedFrom<Ast::Function>()) {
    result = generation->generateFunction(static_cast<Ast::Function*>(element), buildSession->getCodeGenSession());
  } else {
    throw EXCEPTION(InvalidArgumentException, S("element"), S("Invalid element type."));
  }

  buildMgr->rootManager->flushNotices();
  return result;
}


Bool BuildManager::_addElementToExecutionEntry(TiObject *self, TiObject *element, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);
  auto generation = ti_cast<CodeGen::Generation>(buildMgr->generator);

  CodeGen::TerminalStatement terminal;
  Bool result = true;
  if (element->isDerivedFrom<Ast::Function>()) {
    throw EXCEPTION(InvalidArgumentException, S("element"), S("Invalid element type."));
  } else if (element->isDerivedFrom<Ast::Module>()) {
    result = generation->generateModuleInit(static_cast<Ast::Module*>(element), buildSession->getCodeGenSession());
  } else {
    result = generation->generateStatement(element, buildSession->getCodeGenSession(), terminal);
  }
  
  buildMgr->rootManager->flushNotices();
  return result;
}


Bool BuildManager::_execute(TiObject *self, BuildSession *buildSession)
{
  PREPARE_SELF(buildMgr, BuildManager);

  auto minSeverity = buildMgr->rootManager->getMinNoticeSeverityEncountered();
  auto thisMinSeverity = buildMgr->rootManager->getNoticeStore()->getMinEncounteredSeverity();

  // Get the session in which global constructors were generated, if different from the given session.
  // Refer to BuildSession::globalCtorSession for more info about this.
  auto ctorBuildSession = buildSession->getGlobalCtorSession();
  if (ctorBuildSession == 0) ctorBuildSession = buildSession;

  if ((minSeverity == -1 || minSeverity > 1) && (thisMinSeverity == -1 || thisMinSeverity > 1)) {
    // Execute constructors.
    if (ctorBuildSession->getBuildType() == BuildType::JIT) {
      for (Int index = 0; index < buildSession->getGlobalCtors()->getLength(); ++index) {
        ctorBuildSession->getBuildTarget().s_cast<LlvmCodeGen::JitBuildTarget>()->execute(
          buildSession->getGlobalCtors()->at(index).name
        );
      }
      buildSession->getGlobalCtors()->clear();
    } else if (ctorBuildSession->getBuildType() == BuildType::PREPROCESS) {
      for (Int index = 0; index < buildSession->getGlobalCtors()->getLength(); ++index) {
        ctorBuildSession->getBuildTarget().s_cast<LlvmCodeGen::LazyJitBuildTarget>()->execute(
          buildSession->getGlobalCtors()->at(index).name
        );
      }
      buildSession->getGlobalCtors()->clear();
    } else {
      throw EXCEPTION(InvalidArgumentException, S("buildSession"), S("Unexpected build type."));
    }
    // Execute the main function.
    if (buildSession->getBuildType() == BuildType::JIT) {
      buildSession->getBuildTarget().s_cast<LlvmCodeGen::JitBuildTarget>()->execute(
        buildSession->getExecutionEntryName()
      );
    } else if (buildSession->getBuildType() == BuildType::PREPROCESS) {
      buildSession->getBuildTarget().s_cast<LlvmCodeGen::LazyJitBuildTarget>()->execute(
        buildSession->getExecutionEntryName()
      );
    } else {
      throw EXCEPTION(InvalidArgumentException, S("buildSession"), S("Unexpected build type."));
    }
    return true;
  } else {
    return false;
  }
}


void BuildManager::_dumpLlvmIrForElement(TiObject *self, TiObject *element)
{
  VALIDATE_NOT_NULL(element);
  PREPARE_SELF(buildMgr, BuildManager);

  SharedPtr<BuildSession> buildSession = buildMgr->prepareBuild(BuildManager::BuildType::OFFLINE, 0);
  Bool result = true;
  if (element->isDerivedFrom<Ast::Module>()) {
    buildMgr->prepareExecutionEntry(buildSession.get());
    if (!buildMgr->addElementToExecutionEntry(element, buildSession.get())) result = false;
    if (!buildMgr->finalizeExecutionEntry(buildSession.get())) result = false;
    buildSession->getGlobalCtors()->add(CodeGen::GlobalCtorDtorInfo(buildSession->getExecutionEntryName()));
  }
  if (!buildMgr->addElementToBuild(element, buildSession.get())) result = false;

  Array<Str> globalCtorNames = BuildManager::getGlobalCtorNames(buildSession.get());
  Array<Str> globalDtorNames = BuildManager::getGlobalDtorNames(buildSession.get());

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

  SharedPtr<BuildSession> buildSession = buildMgr->prepareBuild(BuildManager::BuildType::OFFLINE, targetTriple);
  Bool result = true;
  if (element->isDerivedFrom<Ast::Module>()) {
    buildMgr->prepareExecutionEntry(buildSession.get());
    if (!buildMgr->addElementToExecutionEntry(element, buildSession.get())) result = false;
    if (!buildMgr->finalizeExecutionEntry(buildSession.get())) result = false;
    buildSession->getGlobalCtors()->add(CodeGen::GlobalCtorDtorInfo(buildSession->getExecutionEntryName()));
  }
  if (!buildMgr->addElementToBuild(element, buildSession.get())) result = false;

  if (result) {
    Array<Str> globalCtorNames = BuildManager::getGlobalCtorNames(buildSession.get());
    Array<Str> globalDtorNames = BuildManager::getGlobalDtorNames(buildSession.get());
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
  buildSession->setExecutionEntryName(0);
  buildSession->setExecutionEntryTgFunc(TioSharedPtr::null);
  buildSession->setExecutionEntryTgContext(TioSharedPtr::null);
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

  if (obj->isDerivedFrom<Core::Data::Ast::Passage>()) return;

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

  BuildSession buildSession(buildMgr->preprocessBuildSession->getBuildId(), buildMgr->preprocessBuildSession.get());

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


Array<Str> BuildManager::getGlobalCtorNames(BuildSession *buildSession)
{
  Array<Str> globalCtorNames;
  for (Int i = 0; i < buildSession->getGlobalCtors()->getLength(); ++i) {
    globalCtorNames.add(buildSession->getGlobalCtors()->at(i).name);
  }
  return globalCtorNames;
}


Array<Str> BuildManager::getGlobalDtorNames(BuildSession *buildSession)
{
  Array<Str> globalDtorNames;
  for (Int i = 0; i < buildSession->getGlobalDtors()->getLength(); ++i) {
    globalDtorNames.add(buildSession->getGlobalDtors()->at(i).name);
  }
  return globalDtorNames;
}

} // namespace

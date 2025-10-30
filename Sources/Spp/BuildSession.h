/**
 * @file Spp/BuildSession.h
 * Contains the header of class Spp::BuildSession.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_BUILDSESSION_H
#define SPP_BUILDSESSION_H

namespace Spp
{

class BuildSession : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildSession, TiObject, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: Int buildId;
  private: Word buildType;
  private: SharedPtr<CodeGen::ExtraDataAccessor> extraDataAccessor;
  private: SharedPtr<LlvmCodeGen::TargetGenerator> targetGenerator;
  private: SharedPtr<LlvmCodeGen::BuildTarget> buildTarget;

  private: SharedPtr<Array<CodeGen::GlobalCtorDtorInfo>> globalCtors;
  private: SharedPtr<Array<CodeGen::GlobalCtorDtorInfo>> globalDtors;
  private: TioSharedPtr executionEntryTgFunc;
  private: TioSharedPtr executionEntryTgContext;
  private: Str executionEntryName;

  private: CodeGen::Session codeGenSession;

  // This variable is used to enable generating global constructors in a different session.
  // It's needed in case of running in JIT session. When running in JIT session we still
  // want to run global constructors in the pre-process session to avoid cases where a
  // preprocess statement tries to access a global variable scheduled for initialization
  // in the JIT context, but hasn't yet been initialized, since preprocess statements
  // share the same global variables as JIT statements.
  // It's possible to avoid this complexity by dropping the JIT session in favour of
  // the preprocess session, but the lazy JIT build target used by the preprocess
  // session is significantly slower than the non-lazy JIT target (can be up to 35%
  // slower).
  private: BuildSession *globalCtorSession;


  //============================================================================
  // Constructor

  public: BuildSession(
    Int bId, Word bType, Bool offlineExec,
    SharedPtr<LlvmCodeGen::TargetGenerator> tg, SharedPtr<LlvmCodeGen::BuildTarget> bt,
    SharedPtr<Array<CodeGen::GlobalCtorDtorInfo>> gCtors,
    SharedPtr<Array<CodeGen::GlobalCtorDtorInfo>> gDtors,
    BuildSession *globalCtorSess
  ) : buildId(bId)
    , buildType(bType)
    , extraDataAccessor(newSrdObj<CodeGen::ExtraDataAccessor>())
    , targetGenerator(tg)
    , buildTarget(bt)
    , globalCtors(gCtors)
    , globalDtors(gDtors)
    , globalCtorSession(globalCtorSess)
    , codeGenSession(
      bId,
      extraDataAccessor.get(),
      tg->getInterface<CodeGen::TargetGeneration>(),
      offlineExec,
      globalCtors.get(),
      globalDtors.get(),
      0,
      0,
      globalCtorSess == 0 ? 0 : globalCtorSess->getCodeGenSession()
    )
  {
  }

  public: BuildSession(Int bId, BuildSession *bs)
    : buildId(bId)
    , buildType(bs->getBuildType())
    , extraDataAccessor(bs->getExtraDataAccessor())
    , targetGenerator(bs->getTargetGenerator())
    , buildTarget(bs->getBuildTarget())
    , globalCtors(bs->getGlobalCtors())
    , globalDtors(bs->getGlobalDtors())
    , globalCtorSession(bs->getGlobalCtorSession())
    , codeGenSession(
      bId,
      extraDataAccessor.get(),
      targetGenerator->getInterface<CodeGen::TargetGeneration>(),
      bs->getCodeGenSession()->isOfflineExecution(),
      globalCtors.get(),
      globalDtors.get(),
      0,
      0,
      bs->getGlobalCtorSession() == 0 ? 0 : bs->getGlobalCtorSession()->getCodeGenSession()
    )
  {
  }


  //============================================================================
  // Member Variables

  public: Int getBuildId() const
  {
    return this->buildId;
  }

  public: Word getBuildType() const
  {
    return this->buildType;
  }

  public: SharedPtr<CodeGen::ExtraDataAccessor> const& getExtraDataAccessor()
  {
    return this->extraDataAccessor;
  }

  public: SharedPtr<LlvmCodeGen::TargetGenerator> const& getTargetGenerator() const
  {
    return this->targetGenerator;
  }

  public: SharedPtr<LlvmCodeGen::BuildTarget> const& getBuildTarget() const
  {
    return this->buildTarget;
  }

  public: SharedPtr<Array<CodeGen::GlobalCtorDtorInfo>> const& getGlobalCtors()
  {
    return this->globalCtors;
  }

  public: SharedPtr<Array<CodeGen::GlobalCtorDtorInfo>> const& getGlobalDtors()
  {
    return this->globalDtors;
  }

  public: void setExecutionEntryTgFunc(TioSharedPtr const &tgFunc)
  {
    this->executionEntryTgFunc = tgFunc;
  }

  public: TioSharedPtr const& getExecutionEntryTgFunc() const
  {
    return this->executionEntryTgFunc;
  }

  public: void setExecutionEntryTgContext(TioSharedPtr const &tgContext)
  {
    this->executionEntryTgContext = tgContext;
    this->codeGenSession.setTgContext(this->executionEntryTgContext.get());
    this->codeGenSession.setTgAllocContext(this->executionEntryTgContext.get());
  }

  public: TioSharedPtr const& getExecutionEntryTgContext() const
  {
    return this->executionEntryTgContext;
  }

  public: void setExecutionEntryName(Char const *een)
  {
    this->executionEntryName = een;
  }

  public: Str const& getExecutionEntryName() const
  {
    return this->executionEntryName;
  }

  public: CodeGen::Session* getCodeGenSession()
  {
    return &this->codeGenSession;
  }

  public: BuildSession* getGlobalCtorSession() const
  {
    return this->globalCtorSession;
  }

}; // class

} // namespace

#endif

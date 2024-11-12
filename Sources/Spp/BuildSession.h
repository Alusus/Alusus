/**
 * @file Spp/BuildSession.h
 * Contains the header of class Spp::BuildSession.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
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

  private: SharedPtr<DependencyInfo> depsInfo;
  private: SharedPtr<CodeGen::ExtraDataAccessor> extraDataAccessor;
  private: SharedPtr<LlvmCodeGen::TargetGenerator> targetGenerator;
  private: SharedPtr<LlvmCodeGen::BuildTarget> buildTarget;
  private: Word buildType;

  private: TioSharedPtr voidNoArgsFuncTgType;
  private: TioSharedPtr globalEntryTgFunc;
  private: TioSharedPtr globalEntryTgContext;
  private: Str globalEntryName;

  private: CodeGen::DestructionStack destructionStack;
  private: CodeGen::Session codeGenSession;


  //============================================================================
  // Constructor

  public: BuildSession(
    SharedPtr<LlvmCodeGen::TargetGenerator> tg, SharedPtr<LlvmCodeGen::BuildTarget> bt, Bool offlineExec, Word bType
  ) : depsInfo(newSrdObj<DependencyInfo>())
    , extraDataAccessor(newSrdObj<CodeGen::ExtraDataAccessor>())
    , targetGenerator(tg)
    , buildTarget(bt)
    , buildType(bType)
    , codeGenSession(
      extraDataAccessor.get(),
      tg->getInterface<CodeGen::TargetGeneration>(),
      0,
      0,
      &destructionStack,
      &depsInfo->globalVarInitializationDeps,
      &depsInfo->globalVarDestructionDeps,
      &depsInfo->funcDeps,
      offlineExec
    )
  {
  }

  public: BuildSession(BuildSession *bs)
    : depsInfo(bs->getDepsInfo())
    , extraDataAccessor(bs->getExtraDataAccessor())
    , targetGenerator(bs->getTargetGenerator())
    , buildTarget(bs->getBuildTarget())
    , buildType(bs->getBuildType())
    , voidNoArgsFuncTgType(bs->voidNoArgsFuncTgType)
    , codeGenSession(
      extraDataAccessor.get(),
      targetGenerator->getInterface<CodeGen::TargetGeneration>(),
      0,
      0,
      &destructionStack,
      &depsInfo->globalVarInitializationDeps,
      &depsInfo->globalVarDestructionDeps,
      &depsInfo->funcDeps,
      bs->getCodeGenSession()->isOfflineExecution()
    )
  {
  }

  public: BuildSession(BuildSession *bs, SharedPtr<DependencyInfo> const &di)
    : depsInfo(di)
    , extraDataAccessor(bs->getExtraDataAccessor())
    , targetGenerator(bs->getTargetGenerator())
    , buildTarget(bs->getBuildTarget())
    , buildType(bs->getBuildType())
    , voidNoArgsFuncTgType(bs->voidNoArgsFuncTgType)
    , codeGenSession(
      extraDataAccessor.get(),
      targetGenerator->getInterface<CodeGen::TargetGeneration>(),
      0,
      0,
      &destructionStack,
      &depsInfo->globalVarInitializationDeps,
      &depsInfo->globalVarDestructionDeps,
      &depsInfo->funcDeps,
      bs->getCodeGenSession()->isOfflineExecution()
    )
  {
  }


  //============================================================================
  // Member Variables

  public: SharedPtr<DependencyInfo> const& getDepsInfo()
  {
    return this->depsInfo;
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

  public: Word getBuildType() const
  {
    return this->buildType;
  }

  public: void setVoidNoArgsFuncTgType(TioSharedPtr const &tgType)
  {
    this->voidNoArgsFuncTgType = tgType;
  }

  public: TioSharedPtr const& getVoidNoArgsFuncTgType() const
  {
    return this->voidNoArgsFuncTgType;
  }

  public: void setGlobalEntryTgFunc(TioSharedPtr const &tgFunc)
  {
    this->globalEntryTgFunc = tgFunc;
  }

  public: TioSharedPtr const& getGlobalEntryTgFunc() const
  {
    return this->globalEntryTgFunc;
  }

  public: void setGlobalEntryTgContext(TioSharedPtr const &tgContext)
  {
    this->globalEntryTgContext = tgContext;
    this->codeGenSession.setTgContext(this->globalEntryTgContext.get());
    this->codeGenSession.setTgAllocContext(this->globalEntryTgContext.get());
  }

  public: TioSharedPtr const& getGlobalEntryTgContext() const
  {
    return this->globalEntryTgContext;
  }

  public: void setGlobalEntryName(Char const *gen)
  {
    this->globalEntryName = gen;
  }

  public: Str const& getGlobalEntryName() const
  {
    return this->globalEntryName;
  }

  public: CodeGen::DestructionStack* getDestructionStack()
  {
    return &this->destructionStack;
  }

  public: CodeGen::Session* getCodeGenSession()
  {
    return &this->codeGenSession;
  }

}; // class

} // namespace

#endif

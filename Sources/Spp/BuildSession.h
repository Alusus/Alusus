/**
 * @file Spp/BuildSession.h
 * Contains the header of class Spp::BuildSession.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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

  private: DependencyList<Core::Data::Node> globalVarInitializationDeps;
  private: DependencyList<Core::Data::Node> globalVarDestructionDeps;
  private: DependencyList<Ast::Function> funcDeps;

  private: Word buildType;
  private: TioSharedPtr globalEntryTgFunc;
  private: TioSharedPtr globalEntryTgContext;
  private: Str globalEntryName;

  private: std::vector<Str> globalCtorNames;
  private: std::vector<Str> globalDtorNames;

  private: ExecutionContext execContext;
  private: CodeGen::DestructionStack destructionStack;
  private: CodeGen::Session codeGenSession;


  //============================================================================
  // Constructor

  public: BuildSession(
    CodeGen::ExtraDataAccessor *eda, CodeGen::TargetGeneration *tg, Bool offlineExec, Word pointerBc,
    Word bType, TioSharedPtr const &geTgFunc, TioSharedPtr const &geTgContext, Char const *geName
  ) : buildType(bType)
    , globalEntryTgFunc(geTgFunc)
    , globalEntryTgContext(geTgContext)
    , globalEntryName(geName)
    , execContext(pointerBc)
    , codeGenSession(
      eda, tg, &execContext, geTgContext.get(), &destructionStack, &globalVarInitializationDeps, &globalVarDestructionDeps,
      &funcDeps, offlineExec
    )
  {}


  //============================================================================
  // Member Variables

  public: DependencyList<Core::Data::Node>* getGlobalVarInitializationDeps()
  {
    return &this->globalVarInitializationDeps;
  }

  public: DependencyList<Core::Data::Node>* getGlobalVarDestructionDeps()
  {
    return &this->globalVarDestructionDeps;
  }

  public: DependencyList<Ast::Function>* getFuncDeps()
  {
    return &this->funcDeps;
  }

  public: Word getBuildType() const
  {
    return this->buildType;
  }

  public: TioSharedPtr const& getGlobalEntryTgFunc() const
  {
    return this->globalEntryTgFunc;
  }

  public: TioSharedPtr const& getGlobalEntryTgContext() const
  {
    return this->globalEntryTgContext;
  }

  public: Str const& getGlobalEntryName() const
  {
    return this->globalEntryName;
  }

  public: std::vector<Str>* getGlobalCtorNames()
  {
    return &this->globalCtorNames;
  }

  public: std::vector<Str>* getGlobalDtorNames()
  {
    return &this->globalDtorNames;
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

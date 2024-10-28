/**
 * @file Spp/CodeGen/Session.h
 * Contains the header of class Spp::CodeGen::Session.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_SESSION_H
#define SPP_CODEGEN_SESSION_H

namespace Spp::CodeGen
{

class Session : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Session, TiObject, "Spp.CodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: ExtraDataAccessor *eda;
  private: TargetGeneration *tg;
  private: TiObject *tgContext;
  private: TiObject *tgAllocContext;
  private: DestructionStack *destructionStack;
  private: DependencyList<Core::Data::Node> *globalVarInitializationDeps;
  private: DependencyList<Core::Data::Node> *globalVarDestructionDeps;
  private: DependencyList<Ast::Function> *funcDeps;
  private: Bool offlineExecution;
  private: TioSharedPtr tgSelf;
  private: Ast::Type *astSelfType;


  //============================================================================
  // Constructor & Destructor

  public: Session(
    ExtraDataAccessor *eda, TargetGeneration *tg, TiObject *tgc, TiObject *tgac,
    DestructionStack *ds, DependencyList<Core::Data::Node> *gvInitDeps, DependencyList<Core::Data::Node> *gvDestDeps,
    DependencyList<Ast::Function> *fDeps, Bool offlineExec
  ) : eda(eda)
    , tg(tg)
    , tgContext(tgc)
    , tgAllocContext(tgac)
    , destructionStack(ds)
    , globalVarInitializationDeps(gvInitDeps)
    , globalVarDestructionDeps(gvDestDeps)
    , funcDeps(fDeps)
    , offlineExecution(offlineExec)
    , tgSelf(0)
    , astSelfType(0)
  {}

  public: Session(Session *session, TiObject *tgc, TiObject *tgac)
    : eda(session->getEda())
    , tg(session->getTg())
    , tgContext(tgc)
    , tgAllocContext(tgac)
    , destructionStack(session->getDestructionStack())
    , globalVarInitializationDeps(session->getGlobalVarInitializationDeps())
    , globalVarDestructionDeps(session->getGlobalVarDestructionDeps())
    , funcDeps(session->getFuncDeps())
    , offlineExecution(session->isOfflineExecution())
    , tgSelf(session->getTgSelf())
    , astSelfType(session->getAstSelfType())
  {}

  public: Session(Session *session, TiObject *tgc, TiObject *tgac, DestructionStack *ds)
    : eda(session->getEda())
    , tg(session->getTg())
    , tgContext(tgc)
    , tgAllocContext(tgac)
    , destructionStack(ds)
    , globalVarInitializationDeps(session->getGlobalVarInitializationDeps())
    , globalVarDestructionDeps(session->getGlobalVarDestructionDeps())
    , funcDeps(session->getFuncDeps())
    , offlineExecution(session->isOfflineExecution())
    , tgSelf(session->getTgSelf())
    , astSelfType(session->getAstSelfType())
  {}

  public: Session(
    Session *session, TiObject *tgc, TiObject *tgac, DestructionStack *ds, TioSharedPtr const &tgs, Ast::Type *astst
  ) : eda(session->getEda())
    , tg(session->getTg())
    , tgContext(tgc)
    , tgAllocContext(tgac)
    , destructionStack(ds)
    , globalVarInitializationDeps(session->getGlobalVarInitializationDeps())
    , globalVarDestructionDeps(session->getGlobalVarDestructionDeps())
    , funcDeps(session->getFuncDeps())
    , offlineExecution(session->isOfflineExecution())
    , tgSelf(tgs)
    , astSelfType(astst)
  {}


  //============================================================================
  // Member Functions

  public: ExtraDataAccessor* getEda() {
    return this->eda;
  }

  public: TargetGeneration* getTg() {
    return this->tg;
  }

  public: void setTgContext(TiObject *tgc)
  {
    this->tgContext = tgc;
  }

  public: TiObject* getTgContext() {
    return this->tgContext;
  }

  public: void setTgAllocContext(TiObject *tgac)
  {
    this->tgAllocContext = tgac;
  }

  public: TiObject* getTgAllocContext() {
    return this->tgAllocContext;
  }

  public: DestructionStack* getDestructionStack() {
    return this->destructionStack;
  }

  public: DependencyList<Core::Data::Node>* getGlobalVarInitializationDeps() {
    return this->globalVarInitializationDeps;
  }

  public: DependencyList<Core::Data::Node>* getGlobalVarDestructionDeps() {
    return this->globalVarDestructionDeps;
  }

  public: DependencyList<Ast::Function>* getFuncDeps() {
    return this->funcDeps;
  }

  public: Bool isOfflineExecution() {
    return this->offlineExecution;
  }

  public: void setTgSelf(TioSharedPtr const &tgs) {
    this->tgSelf = tgs;
  }

  public: TioSharedPtr const& getTgSelf() {
    return this->tgSelf;
  }

  public: void setAstSelfType(Ast::Type *type) {
    this->astSelfType = type;
  }

  public: Ast::Type* getAstSelfType() {
    return this->astSelfType;
  }

}; // class

} // namespace

#endif

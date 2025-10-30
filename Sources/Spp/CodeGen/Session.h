/**
 * @file Spp/CodeGen/Session.h
 * Contains the header of class Spp::CodeGen::Session.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
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

  private: Int buildId;
  private: ExtraDataAccessor *eda;
  private: TargetGeneration *tg;
  private: Bool offlineExecution;
  private: Array<GlobalCtorDtorInfo> *globalCtors;
  private: Array<GlobalCtorDtorInfo> *globalDtors;
  private: TiObject *tgContext;
  private: TiObject *tgAllocContext;
  private: SharedPtr<DestructionStack> destructionStack;
  private: SharedPtr<DependencyInfo> depsInfo;
  private: TioSharedPtr tgSelf; // TODO: Rename to tgSelfVar
  private: Ast::Type *astSelfType;

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
  private: Session *globalCtorSession;


  //============================================================================
  // Constructor & Destructor

  public: Session(
    Int bId, ExtraDataAccessor *eda, TargetGeneration *tg, Bool offlineExec,
    Array<GlobalCtorDtorInfo> *globalCtors, Array<GlobalCtorDtorInfo> *globalDtors,
    TiObject *tgc, TiObject *tgac, Session *globalCtorSess
  ) : buildId(bId)
    , eda(eda)
    , tg(tg)
    , offlineExecution(offlineExec)
    , globalCtors(globalCtors)
    , globalDtors(globalDtors)
    , tgContext(tgc)
    , tgAllocContext(tgac)
    , destructionStack(newSrdObj<DestructionStack>())
    , depsInfo(newSrdObj<DependencyInfo>())
    , tgSelf(0)
    , astSelfType(0)
    , globalCtorSession(globalCtorSess)
  {}

  public: Session(Session *session, TiObject *tgc, TiObject *tgac)
    : tgContext(tgc)
    , tgAllocContext(tgac)
    , buildId(session->getBuildId())
    , eda(session->getEda())
    , tg(session->getTg())
    , offlineExecution(session->isOfflineExecution())
    , globalCtors(session->getGlobalCtors())
    , globalDtors(session->getGlobalDtors())
    , destructionStack(session->getDestructionStack())
    , depsInfo(session->depsInfo)
    , tgSelf(session->getTgSelf())
    , astSelfType(session->getAstSelfType())
    , globalCtorSession(session->getGlobalCtorSession())
  {}

  public: Session(
    Session *session, TiObject *tgc, TiObject *tgac, SharedPtr<DestructionStack> ds, SharedPtr<DependencyInfo> di
  ) : tgContext(tgc)
    , tgAllocContext(tgac)
    , destructionStack(ds)
    , depsInfo(di)
    , buildId(session->getBuildId())
    , eda(session->getEda())
    , tg(session->getTg())
    , offlineExecution(session->isOfflineExecution())
    , globalCtors(session->getGlobalCtors())
    , globalDtors(session->getGlobalDtors())
    , tgSelf(session->getTgSelf())
    , astSelfType(session->getAstSelfType())
    , globalCtorSession(session->getGlobalCtorSession())
  {}

  public: Session(
    Session *session, TiObject *tgc, TiObject *tgac, SharedPtr<DestructionStack> ds, SharedPtr<DependencyInfo> di,
    TioSharedPtr const &tgs, Ast::Type *astst
  ) : tgContext(tgc)
    , tgAllocContext(tgac)
    , destructionStack(ds)
    , depsInfo(di)
    , tgSelf(tgs)
    , astSelfType(astst)
    , buildId(session->getBuildId())
    , eda(session->getEda())
    , tg(session->getTg())
    , offlineExecution(session->isOfflineExecution())
    , globalCtors(session->getGlobalCtors())
    , globalDtors(session->getGlobalDtors())
    , globalCtorSession(session->getGlobalCtorSession())
  {}


  //============================================================================
  // Member Functions

  public: Int getBuildId() const {
    return this->buildId;
  }

  public: ExtraDataAccessor* getEda() {
    return this->eda;
  }

  public: TargetGeneration* getTg() {
    return this->tg;
  }

  public: Bool isOfflineExecution() {
    return this->offlineExecution;
  }

  public: Array<GlobalCtorDtorInfo>* getGlobalCtors() {
    return this->globalCtors;
  }

  public: Array<GlobalCtorDtorInfo>* getGlobalDtors() {
    return this->globalDtors;
  }

  public: void setTgContext(TiObject *tgc) {
    this->tgContext = tgc;
  }

  public: TiObject* getTgContext() {
    return this->tgContext;
  }

  public: void setTgAllocContext(TiObject *tgac) {
    this->tgAllocContext = tgac;
  }

  public: TiObject* getTgAllocContext() {
    return this->tgAllocContext;
  }

  public: SharedPtr<DestructionStack> const& getDestructionStack() {
    return this->destructionStack;
  }

  public: SharedPtr<DependencyInfo> const& getDependencyInfo() {
    return this->depsInfo;
  }

  public: DependencyList<Core::Data::Node>* getGlobalVarInitializationDeps() {
    return &this->depsInfo->globalVarInitializationDeps;
  }

  public: DependencyList<Core::Data::Node>* getGlobalVarDestructionDeps() {
    return &this->depsInfo->globalVarDestructionDeps;
  }

  public: DependencyList<Ast::Function>* getFuncDeps() {
    return &this->depsInfo->funcDeps;
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

  public: Session* getGlobalCtorSession() const {
    return this->globalCtorSession;
  }

}; // class

} // namespace

#endif

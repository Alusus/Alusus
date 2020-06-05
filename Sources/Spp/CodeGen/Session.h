/**
 * @file Spp/CodeGen/Session.h
 * Contains the header of class Spp::CodeGen::Session.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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
  private: DestructionStack *destructionStack;
  private: TiObject *tgGlobalConstructionContext;
  private: DestructionStack *globalDestructionStack;
  private: Bool offlineExecution;
  private: TioSharedPtr tgSelf;
  private: Ast::Type *astSelfType;


  //============================================================================
  // Constructor & Destructor

  public: Session(
    ExtraDataAccessor *eda, TargetGeneration *tg, TiObject *tgc, DestructionStack *ds,
    TiObject *tgGcc, DestructionStack *gds, Bool offlineExec
  ) : eda(eda), tg(tg), tgContext(tgc), destructionStack(ds)
    , tgGlobalConstructionContext(tgGcc), globalDestructionStack(gds)
    , offlineExecution(offlineExec), tgSelf(0), astSelfType(0)
  {}

  public: Session(Session *session, TiObject *tgc)
    : eda(session->getEda()), tg(session->getTg()), tgContext(tgc), destructionStack(session->getDestructionStack())
    , tgGlobalConstructionContext(session->getTgGlobalConstructionContext())
    , globalDestructionStack(session->getGlobalDestructionStack())
    , offlineExecution(session->isOfflineExecution())
    , tgSelf(session->getTgSelf()), astSelfType(session->getAstSelfType())
  {}

  public: Session(Session *session, TiObject *tgc, DestructionStack *ds)
    : eda(session->getEda()), tg(session->getTg()), tgContext(tgc), destructionStack(ds)
    , tgGlobalConstructionContext(session->getTgGlobalConstructionContext())
    , globalDestructionStack(session->getGlobalDestructionStack())
    , offlineExecution(session->isOfflineExecution())
    , tgSelf(session->getTgSelf()), astSelfType(session->getAstSelfType())
  {}

  public: Session(
    Session *session, TiObject *tgc, DestructionStack *ds, TioSharedPtr const &tgs, Ast::Type *astst
  ) : eda(session->getEda()), tg(session->getTg()), tgContext(tgc), destructionStack(ds)
    , tgGlobalConstructionContext(session->getTgGlobalConstructionContext())
    , globalDestructionStack(session->getGlobalDestructionStack())
    , offlineExecution(session->isOfflineExecution())
    , tgSelf(tgs), astSelfType(astst)
  {}


  //============================================================================
  // Member Functions

  public: ExtraDataAccessor* getEda() {
    return this->eda;
  }

  public: TargetGeneration* getTg() {
    return this->tg;
  }

  public: TiObject* getTgContext() {
    return this->tgContext;
  }

  public: DestructionStack* getDestructionStack() {
    return this->destructionStack;
  }

  public: TiObject* getTgGlobalConstructionContext() {
    return this->tgGlobalConstructionContext;
  }

  public: DestructionStack* getGlobalDestructionStack() {
    return this->globalDestructionStack;
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

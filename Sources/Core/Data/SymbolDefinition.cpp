/**
 * @file Core/Data/SymbolDefinition.cpp
 * Contains the implementation of class Core::Data::SymbolDefinition.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//==============================================================================
// Constructor

SymbolDefinition::SymbolDefinition(const SharedPtr<Reference> &pnt,
                                   SharedPtr<Node> const &t,
                                   SharedPtr<Node> const &vd,
                                   SharedPtr<Node> const &v,
                                   SharedPtr<BuildHandler> const &h,
                                   SharedPtr<Node> const &p,
                                   SharedPtr<Node> const &f,
                                   SharedPtr<Node> const &a) :
  parentReference(pnt), term(t), varDefs(vd), vars(v), handler(h), priority(p),
  flags(f), attributes(a), ownership(SymbolDefElement::ALL)
{
  if (t == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("t"), STR("Should not be null."));
  }
  if (!t->isDerivedFrom<Term>() && !t->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("t"), STR("Must be of type Term or Reference."));
  }
  if (vd != 0 && !vd->isA<SharedMap>() && !vd->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("vd"), STR("Must be of type SharedMap or Reference."));
  }
  if (v != 0 && !v->isA<SharedMap>() && !v->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("v"), STR("Must be of type SharedMap or Reference."));
  }
  if (p != 0 && !p->isA<Integer>() && !p->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("p"), STR("Must be of type Integer or Reference."));
  }
  if (f != 0 && !f->isA<Integer>() && !f->isDerivedFrom<Reference>()) {
    throw EXCEPTION(InvalidArgumentException, STR("f"), STR("Must be of type Integer or Reference."));
  }
  if (this->parentReference != 0) this->parentReference->setOwner(this);
  if (this->term != 0) this->term->setOwner(this);
  if (this->varDefs != 0) this->varDefs->setOwner(this);
  if (this->vars != 0) this->vars->setOwner(this);
  if (this->attributes != 0) this->attributes->setOwner(this);
  if (this->handler != 0) this->handler->setOwner(this);
}


SymbolDefinition::SymbolDefinition(const std::initializer_list<Argument<SymbolDefElement>> &args) :
  priority(0), flags(0), ownership(0), parent(0)
{
  for (auto arg : args) {
    switch (arg.id.val) {
      case SymbolDefElement::PARENT_REF:
        this->parentReference = arg.tiShared.ti_cast<Reference>();
        if (this->parentReference == 0 && arg.tiShared != 0) {
          throw EXCEPTION(InvalidArgumentException, STR("parent_ref"),
                          STR("Must be of type Reference."), arg.tiShared->getMyTypeInfo()->getUniqueName());
        }
        this->ownership |= SymbolDefElement::PARENT_REF;
        break;
      case SymbolDefElement::TERM:
        UPDATE_OWNED_SHAREDPTR(this->term, arg.tiShared.ti_cast<Node>());
        this->ownership |= SymbolDefElement::TERM;
        break;
      case SymbolDefElement::VAR_DEFS:
        UPDATE_OWNED_SHAREDPTR(this->varDefs, arg.tiShared.ti_cast<Node>());
        if (this->varDefs != 0 && !this->varDefs->isA<SharedMap>() && !this->varDefs->isDerivedFrom<Reference>()) {
          throw EXCEPTION(InvalidArgumentException, STR("varDefs"),
                          STR("Must be of type SharedMap or Reference."));
        }
        this->ownership |= SymbolDefElement::VAR_DEFS;
        break;
      case SymbolDefElement::VARS:
        UPDATE_OWNED_SHAREDPTR(this->vars, arg.tiShared.ti_cast<Node>());
        if (this->vars != 0 && !this->vars->isA<SharedMap>() && !this->vars->isDerivedFrom<Reference>()) {
          throw EXCEPTION(InvalidArgumentException, STR("vars"),
                          STR("Must be of type SharedMap or Reference."));
        }
        this->ownership |= SymbolDefElement::VARS;
        break;
      case SymbolDefElement::HANDLER:
        UPDATE_OWNED_SHAREDPTR(this->handler, arg.tiShared.ti_cast<BuildHandler>());
        if (this->handler == 0 && arg.tiShared != 0) {
          throw EXCEPTION(InvalidArgumentException, STR("handler"), STR("Must be of type BuildHandler."),
                          arg.tiShared->getMyTypeInfo()->getUniqueName());
        }
        this->ownership |= SymbolDefElement::HANDLER;
        break;
      case SymbolDefElement::PRIORITY:
        UPDATE_OWNED_SHAREDPTR(this->priority, arg.tiShared.ti_cast<Node>());
        if (this->priority != 0 && !this->priority->isA<Integer>() && !this->priority->isDerivedFrom<Reference>()) {
          throw EXCEPTION(InvalidArgumentException, STR("priority"),
                          STR("Must be of type Integer or Reference."));
        }
        this->ownership |= SymbolDefElement::PRIORITY;
        break;
      case SymbolDefElement::FLAGS:
        UPDATE_OWNED_SHAREDPTR(this->flags, arg.tiShared.ti_cast<Node>());
        if (this->flags != 0 && !this->flags->isA<Integer>() && !this->flags->isDerivedFrom<Reference>()) {
          throw EXCEPTION(InvalidArgumentException, STR("flags"),
                          STR("Must be of type Integer or Reference."));
        }
        this->ownership |= SymbolDefElement::FLAGS;
        break;
      case SymbolDefElement::ATTRIBUTES:
        UPDATE_OWNED_SHAREDPTR(this->attributes, arg.tiShared.ti_cast<Node>());
        this->ownership |= SymbolDefElement::ATTRIBUTES;
        break;
    }
  }
  if (this->term == 0 && this->parentReference == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("term"),
                    STR("Should not be null."));
  } else if (this->term != 0) {
    if (!this->term->isDerivedFrom<Term>() && !this->term->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("term"),
                      STR("Must be of type Term or Reference."));
    }
  }
}


SymbolDefinition::~SymbolDefinition()
{
  if (this->parent != 0) this->detachFromParent();
  this->changeNotifier.emit(this, SymbolDefChangeOp::DESTROY, 0);
  RESET_OWNED_SHAREDPTR(this->parentReference);
  RESET_OWNED_SHAREDPTR(this->term);
  RESET_OWNED_SHAREDPTR(this->varDefs);
  RESET_OWNED_SHAREDPTR(this->vars);
  RESET_OWNED_SHAREDPTR(this->attributes);
  RESET_OWNED_SHAREDPTR(this->handler);
}


//==============================================================================
// Member Functions

void SymbolDefinition::attachToParent(SymbolDefinition *p)
{
  ASSERT(this->parent == 0);
  ASSERT(p != 0);
  this->parent = p;
  this->parent->changeNotifier.connect(this->parentContentChangeSlot);
  this->inheritFromParent();
}


void SymbolDefinition::detachFromParent()
{
  ASSERT(this->parent != 0);
  this->removeInheritted();
  this->parent->changeNotifier.disconnect(this->parentContentChangeSlot);
  this->parent = 0;
}


void SymbolDefinition::inheritFromParent()
{
  ASSERT(this->parent != 0);
  if ((this->ownership & SymbolDefElement::TERM) == 0) this->term = this->parent->getTerm();
  if ((this->ownership & SymbolDefElement::VAR_DEFS) == 0) this->varDefs = this->parent->getVarDefs();
  if ((this->ownership & SymbolDefElement::VARS) == 0) this->vars = this->parent->getVars();
  if ((this->ownership & SymbolDefElement::HANDLER) == 0) this->handler = this->parent->getBuildHandler();
  if ((this->ownership & SymbolDefElement::PRIORITY) == 0) this->priority = this->parent->getPriority();
  if ((this->ownership & SymbolDefElement::FLAGS) == 0) this->flags = this->parent->getFlags();
  if ((this->ownership & SymbolDefElement::ATTRIBUTES) == 0) this->attributes = this->parent->getAttributes();
}


void SymbolDefinition::removeInheritted()
{
  if ((this->ownership & SymbolDefElement::TERM) == 0) this->term.reset();
  if ((this->ownership & SymbolDefElement::VAR_DEFS) == 0) this->varDefs.reset();
  if ((this->ownership & SymbolDefElement::VARS) == 0) this->vars.reset();
  if ((this->ownership & SymbolDefElement::HANDLER) == 0) this->handler.reset();
  if ((this->ownership & SymbolDefElement::PRIORITY) == 0) this->priority.reset();
  if ((this->ownership & SymbolDefElement::FLAGS) == 0) this->flags.reset();
  if ((this->ownership & SymbolDefElement::ATTRIBUTES) == 0) this->attributes.reset();
}


void SymbolDefinition::onParentElementChanged(SymbolDefinition *obj, SymbolDefChangeOp op, Word elmt)
{
  if (op == SymbolDefChangeOp::DESTROY) {
    this->detachFromParent();
  } else if ((elmt & this->ownership) == 0) {
    if ((elmt & SymbolDefElement::TERM) == 0) this->term = this->parent->getTerm();
    if ((elmt & SymbolDefElement::VAR_DEFS) == 0) this->varDefs = this->parent->getVarDefs();
    if ((elmt & SymbolDefElement::VARS) == 0) this->vars = this->parent->getVars();
    if ((elmt & SymbolDefElement::HANDLER) == 0) this->handler = this->parent->getBuildHandler();
    if ((elmt & SymbolDefElement::PRIORITY) == 0) this->priority = this->parent->getPriority();
    if ((elmt & SymbolDefElement::FLAGS) == 0) this->flags = this->parent->getFlags();
    if ((elmt & SymbolDefElement::ATTRIBUTES) == 0) this->attributes = this->parent->getAttributes();
  }
}


//==============================================================================
// Initializable Implementation

void SymbolDefinition::initialize(TiObject *owner)
{
  if (this->parentReference != 0) {
    Tracer *tracer = owner->getInterface<Tracer>();
    if (tracer != 0) {
      TiObject *p = tracer->traceValue(this->parentReference.get());
      if (p == 0) {
        throw EXCEPTION(GenericException, STR("Parent reference points to missing definition."));
      }
      SymbolDefinition *psd = tio_cast<SymbolDefinition>(p);
      if (psd == 0) {
        throw EXCEPTION(GenericException, STR("Parent reference points to an object of an invalid type."));
      }
      this->setParent(psd);
    }
  }
}


//==============================================================================
// DataOwner Implementation

void SymbolDefinition::unsetIndexes(Int from, Int to)
{
  if (this->parentReference != 0) {
    Data::unsetIndexes(this->parentReference.get(), from, to);
  }
  if (this->term != 0 && (this->ownership & SymbolDefElement::TERM) != 0) {
    Data::unsetIndexes(this->term.get(), from, to);
  }
  if (this->varDefs != 0 && (this->ownership & SymbolDefElement::VAR_DEFS) != 0) {
    Data::unsetIndexes(this->varDefs.get(), from, to);
  }
  if (this->vars != 0 && (this->ownership & SymbolDefElement::VARS) != 0) {
    Data::unsetIndexes(this->vars.get(), from, to);
  }
  if (this->handler != 0 && (this->ownership & SymbolDefElement::HANDLER) != 0) {
    Data::unsetIndexes(this->handler.get(), from, to);
  }
  if (this->attributes != 0 && (this->ownership & SymbolDefElement::ATTRIBUTES) != 0) {
    Data::unsetIndexes(this->attributes.get(), from, to);
  }
}

} } // namespace

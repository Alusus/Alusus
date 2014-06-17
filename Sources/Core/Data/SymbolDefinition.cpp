/**
 * @file Core/Data/SymbolDefinition.cpp
 * Contains the implementation of class Core::Data::SymbolDefinition.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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

SymbolDefinition::SymbolDefinition(const std::initializer_list<Argument<SymbolDefElement>> &args) :
  priority(0), flags(0), ownership(0), plainParent(0)
{
  for (auto arg : args) {
    switch (arg.id.val) {
      case SymbolDefElement::PARENT_REF:
        this->parentReference = arg.ioVal.io_cast<Reference>();
        if (this->parentReference == 0 && arg.ioVal != 0) {
          throw InvalidArgumentException(STR("parent_ref"),
                                         STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                         STR("Must be of type Reference."),
                                         arg.ioVal->getMyTypeInfo()->getUniqueName());
        }
        this->ownership |= SymbolDefElement::PARENT_REF;
        break;
      case SymbolDefElement::TERM:
        this->term = arg.ioVal;
        this->ownership |= SymbolDefElement::TERM;
        break;
      case SymbolDefElement::VAR_DEFS:
        this->varDefs = arg.ioVal;
        if (this->varDefs != 0 && !this->varDefs->isA<SharedMap>() && !this->varDefs->isDerivedFrom<Reference>()) {
          throw InvalidArgumentException(STR("varDefs"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                         STR("Must be of type SharedMap or Reference."));
        }
        this->ownership |= SymbolDefElement::VAR_DEFS;
        break;
      case SymbolDefElement::VARS:
        this->vars = arg.ioVal;
        if (this->vars != 0 && !this->vars->isA<SharedMap>() && !this->vars->isDerivedFrom<Reference>()) {
          throw InvalidArgumentException(STR("vars"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                         STR("Must be of type SharedMap or Reference."));
        }
        this->ownership |= SymbolDefElement::VARS;
        break;
      case SymbolDefElement::HANDLER:
        this->handler = arg.ioVal.io_cast<OperationHandler>();
        if (this->handler == 0 && arg.ioVal != 0) {
          throw InvalidArgumentException(STR("handler"),
                                         STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                         STR("Must be of type OperationHandler."),
                                         arg.ioVal->getMyTypeInfo()->getUniqueName());
        }
        this->ownership |= SymbolDefElement::HANDLER;
        break;
      case SymbolDefElement::PRIORITY:
        this->priority = arg.intVal;
        this->ownership |= SymbolDefElement::PRIORITY;
        break;
      case SymbolDefElement::FLAGS:
        this->flags = arg.intVal;
        this->ownership |= SymbolDefElement::FLAGS;
        break;
      case SymbolDefElement::ATTRIBUTES:
        this->attributes = arg.ioVal;
        this->ownership |= SymbolDefElement::ATTRIBUTES;
        break;
    }
  }
  if (this->term == 0 && this->parentReference == 0) {
    throw InvalidArgumentException(STR("term"), STR("Core::Data::SymbolDefinition::setTerm"),
                                   STR("Should not be null."));
  } else if (this->term != 0) {
    if (!this->term->isDerivedFrom<Term>() && !this->term->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("term"), STR("Core::Data::SymbolDefinition::Symbol_Definision"),
                                     STR("Must be of type Term or Reference."));
    }
  }
}


//==============================================================================
// Member Functions

void SymbolDefinition::attachToParent(SymbolDefinition *p)
{
  ASSERT(this->plainParent == 0);
  this->plainParent = p;
  this->plainParent->changeNotifier.connect(this, &SymbolDefinition::onParentElementChanged);
  this->inheritFromParent();
}


void SymbolDefinition::detachFromParent()
{
  ASSERT(this->plainParent != 0);
  this->removeInheritted();
  this->plainParent->changeNotifier.unconnect(this, &SymbolDefinition::onParentElementChanged);
  this->plainParent = 0;
  this->parent.reset();
}


void SymbolDefinition::inheritFromParent()
{
  ASSERT(this->plainParent != 0);
  if ((this->ownership & SymbolDefElement::TERM) == 0) this->term = this->plainParent->getTerm();
  if ((this->ownership & SymbolDefElement::VAR_DEFS) == 0) this->varDefs = this->plainParent->getVarDefs();
  if ((this->ownership & SymbolDefElement::VARS) == 0) this->vars = this->plainParent->getVars();
  if ((this->ownership & SymbolDefElement::HANDLER) == 0) this->handler = this->plainParent->getOperationHandler();
  if ((this->ownership & SymbolDefElement::PRIORITY) == 0) this->priority = this->plainParent->getPriority();
  if ((this->ownership & SymbolDefElement::FLAGS) == 0) this->flags = this->plainParent->getFlags();
  if ((this->ownership & SymbolDefElement::ATTRIBUTES) == 0) this->attributes = this->plainParent->getAttributes();
}


void SymbolDefinition::removeInheritted()
{
  if ((this->ownership & SymbolDefElement::TERM) == 0) this->term.reset();
  if ((this->ownership & SymbolDefElement::VAR_DEFS) == 0) this->varDefs.reset();
  if ((this->ownership & SymbolDefElement::VARS) == 0) this->vars.reset();
  if ((this->ownership & SymbolDefElement::HANDLER) == 0) this->handler.reset();
  if ((this->ownership & SymbolDefElement::PRIORITY) == 0) this->priority = 0;
  if ((this->ownership & SymbolDefElement::FLAGS) == 0) this->flags = 0;
  if ((this->ownership & SymbolDefElement::ATTRIBUTES) == 0) this->attributes.reset();
}


void SymbolDefinition::onParentElementChanged(SymbolDefinition *obj, SymbolDefChangeOp op, Word elmt)
{
  if (op == SymbolDefChangeOp::DESTROY) {
    this->detachFromParent();
  } else if ((elmt & this->ownership) == 0) {
    if ((elmt & SymbolDefElement::TERM) == 0) this->term = this->plainParent->getTerm();
    if ((elmt & SymbolDefElement::VAR_DEFS) == 0) this->varDefs = this->plainParent->getVarDefs();
    if ((elmt & SymbolDefElement::VARS) == 0) this->vars = this->plainParent->getVars();
    if ((elmt & SymbolDefElement::HANDLER) == 0) this->handler = this->plainParent->getOperationHandler();
    if ((elmt & SymbolDefElement::PRIORITY) == 0) this->priority = this->plainParent->getPriority();
    if ((elmt & SymbolDefElement::FLAGS) == 0) this->flags = this->plainParent->getFlags();
    if ((elmt & SymbolDefElement::ATTRIBUTES) == 0) this->attributes = this->plainParent->getAttributes();
  }
}


//==============================================================================
// Initializable Implementation

void SymbolDefinition::initialize(IdentifiableObject *owner)
{
  if (this->parentReference != 0) {
    SharedTracer *tracer = owner->getInterface<SharedTracer>();
    if (tracer != 0) {
      SharedPtr<IdentifiableObject> p = tracer->traceSharedValue(this->parentReference);
      if (p == 0) {
        throw GeneralException(STR("Parent reference points to missing definition."),
                               STR("Data::SymbolDefinition::initialize"));
      }
      SharedPtr<SymbolDefinition> psd = p.io_cast<SymbolDefinition>();
      if (psd == 0) {
        throw GeneralException(STR("Parent reference points to an object of an invalid type."),
                               STR("Data::SymbolDefinition::initialize"));
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

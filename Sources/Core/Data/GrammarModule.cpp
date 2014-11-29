/**
 * @file Core/Data/GrammarModule.cpp
 * Contains the implementation of class Core::Data::GrammarModule.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdarg.h>

namespace Core { namespace Data
{

//==============================================================================
// Constructors

GrammarModule::GrammarModule(const std::initializer_list<Argument<Char const*>> &args) : ownership(0), parent(0)
{
  for (auto arg : args) {
    if (sbstr_cast(arg.id) == STR("@parent")) {
      this->parentReference = arg.ioVal.io_cast<Reference>();
      if (this->parentReference == 0 && arg.ioVal != 0) {
        throw GeneralException(STR("Provided parent reference is not of type Reference."),
                               STR("Core::Data::GrammarModule::GrammarModule"));
      }
    } else if (sbstr_cast(arg.id) == STR("@start")) {
      this->startRef = arg.ioVal.io_cast<Reference>();
      this->ownership |= GrammarModuleMetaElement::START_REF;
      if (this->startRef == 0 && arg.ioVal != 0) {
        throw GeneralException(STR("Provided start reference is not of type Reference."),
                               STR("Core::Data::GrammarModule::GrammarModule"));
      }
    } else if (sbstr_cast(arg.id) == STR("@lexer_module")) {
      this->lexerModuleRef = arg.ioVal.io_cast<Reference>();
      this->ownership |= GrammarModuleMetaElement::LEXER_MODULE_REF;
      if (this->lexerModuleRef == 0 && arg.ioVal != 0) {
        throw GeneralException(STR("Provided lexer module reference is not of type Reference."),
                               STR("Core::Data::GrammarModule::GrammarModule"));
      }
    } else {
      this->add(arg.id, arg.ioVal);
    }
  }
}


//==============================================================================
// Inheritance Functions

void GrammarModule::attachToParent(GrammarModule *p)
{
  ASSERT(this->parent == 0);

  this->parent = p;
  this->parent->destroyNotifier.connect(this, &GrammarModule::onParentDestroyed);
  this->definitions.setParent(this->parent->getDefinitions());

  this->parent->metaChangeNotifier.connect(this, &GrammarModule::onParentMetaChanged);
  if ((this->ownership & GrammarModuleMetaElement::START_REF) == 0) {
    this->startRef = this->parent->getStartRef();
  }
  if ((this->ownership & GrammarModuleMetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = this->parent->getLexerModuleRef();
  }
}


void GrammarModule::detachFromParent()
{
  ASSERT(this->parent != 0);

  this->parent->metaChangeNotifier.unconnect(this, &GrammarModule::onParentMetaChanged);
  if ((this->ownership & GrammarModuleMetaElement::START_REF) == 0) {
    this->startRef.reset();
  }
  if ((this->ownership & GrammarModuleMetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef.reset();
  }

  this->parent->destroyNotifier.unconnect(this, &GrammarModule::onParentDestroyed);
  this->parent = 0;
  this->definitions.setParent(0);
}


void GrammarModule::onParentMetaChanged(GrammarModule *obj, Word element)
{
  if ((element & GrammarModuleMetaElement::START_REF) == 0) {
    this->startRef = obj->getStartRef();
  }
  if ((element & GrammarModuleMetaElement::LEXER_MODULE_REF) == 0) {
    this->lexerModuleRef = obj->getLexerModuleRef();
  }
}


//==============================================================================
// Initializable Implementation

void GrammarModule::initialize(IdentifiableObject *owner)
{
  if (this->parentReference != 0) {
    Tracer *tracer = owner->getInterface<Tracer>();
    IdentifiableObject *p = tracer->traceValue(this->parentReference.get());
    if (p == 0) {
      throw GeneralException(STR("Parent reference points to missing definition."),
                             STR("Data::GrammarModule::initialize"));
    }
    GrammarModule *pm = io_cast<GrammarModule>(p);
    if (pm == 0) {
      throw GeneralException(STR("Parent reference points to an object of an invalid type."),
                             STR("Data::GrammarModule::initialize"));
    }
    this->setParent(pm);
  }
}


//==============================================================================
// DataOwner Implementation

void GrammarModule::unsetIndexes(Int from, Int to)
{
  if (this->parentReference != 0) {
    Data::unsetIndexes(this->parentReference.get(), from, to);
  }
  if (this->startRef != 0 && (this->ownership & GrammarModuleMetaElement::START_REF) != 0) {
    Data::unsetIndexes(this->startRef.get(), from, to);
  }
  if (this->lexerModuleRef != 0 && (this->ownership & GrammarModuleMetaElement::LEXER_MODULE_REF) != 0) {
    Data::unsetIndexes(this->lexerModuleRef.get(), from, to);
  }
  Module::unsetIndexes(from, to);
}

} } // namespace

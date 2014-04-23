/**
 * @file Core/Data/GrammarHelper.cpp
 * Contains the implementation of class Core::Data::GrammarHelper.
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
// Misc Functions

void GrammarHelper::getValue(Reference *ref, Module *module, IdentifiableObject *&retVal, Module *&retModule) const
{
  if (module != 0) {
    if (ref->getScope() == ReferenceScope::MODULE) {
      this->seeker.getPlain(ref->getSegment().get(), module, retVal, retModule);
      return;
    } else if (ref->getScope() == ReferenceScope::PMODULE) {
      if (!module->isDerivedFrom<GrammarModule>()) {
        throw GeneralException(STR("The provided reference is of scope PMODULE but the provided module "
                                   "doesn't have a parent."),
                               STR("Core::Data::GrammarHelper::getValue"));
      }
      this->seeker.getPlain(ref->getSegment().get(), static_cast<GrammarModule*>(module)->getParent(),
                            retVal, retModule);
      return;
    }
  }
  this->seeker.getDataProvider()->getPlainValue(ref, retVal, retModule);
}


IdentifiableObject* GrammarHelper::traceValue(IdentifiableObject *val, Module *module) const
{
  if (this->seeker.getDataProvider() == 0) {
    throw GeneralException(STR("Data Provider is not set."), STR("Core::Data::GrammarHelper::traceValue"));
  }
  while (val != 0 && val->isA<Reference>()) {
    this->getValue(static_cast<Reference*>(val), module, val, module);
  }
  return val;
}


void GrammarHelper::traceValue(IdentifiableObject *val, IdentifiableObject *&retVal, Module *&retModule,
                               Module *module) const
{
  if (this->seeker.getDataProvider() == 0) {
    throw GeneralException(STR("Data Provider is not set."), STR("Core::Data::GrammarHelper::traceValue"));
  }
  while (val != 0 && val->isA<Reference>()) {
    this->getValue(static_cast<Reference*>(val), module, val, module);
  }
  retVal = val;
  retModule = module;
}


//==============================================================================
// Term Helper Functions

IdentifiableObject* GrammarHelper::getListTermData(ListTerm *term, Module *module) const
{
  IdentifiableObject *data = this->traceValue(term->getData().get(), module);
  if (data != 0 && !data->isA<List>() && !data->isA<Integer>()) {
    throw GeneralException(STR("Type of list term data is invalid"),
                           STR("Core::Data::GrammarHelper::getListTermData"));
  }
  return data;
}


Word GrammarHelper::getListTermChildCount(ListTerm *term, IdentifiableObject *data, Module *module) const
{
  if (data == 0 && term->getData() != 0) data = this->traceValue(term->getData().get(), module);

  if (term->isStatic()) {
    if (data == 0) return static_cast<List*>(term->getTerms().get())->getCount();
    else if (data->isA<Integer>()) return 1;
    else if (data->isA<List>()) return static_cast<List*>(data)->getCount();
    else {
      throw InvalidArgumentException(STR("data"),
                                     STR("Core::Data::GrammarHelper::getListTermChildCount"),
                                     STR("Must be Integer or List for static list terms."));
    }
  } else {
    if (data == 0) {
      throw InvalidArgumentException(STR("data"),
                                     STR("Core::Data::GrammarHelper::getListTermChildCount"),
                                     STR("Must not be null for dynamic list terms."));
    } else if (!data->isA<List>()) {
      throw InvalidArgumentException(STR("data"),
                                     STR("Core::Data::GrammarHelper::getListTermChildCount"),
                                     STR("Must be of type List for dynamic list terms."));
    } else {
      return static_cast<List*>(data)->getCount();
    }
  }
}


Term* GrammarHelper::useListTermChild(DataContext *context, ListTerm *term, Int index,
                                      IdentifiableObject *data, Module *module) const
{
  if (context == 0) {
    throw InvalidArgumentException(STR("context"), STR("Core::Data::GrammarHelper::useListTermChild"));
  }
  if (data == 0 && term->getData() != 0) data = this->traceValue(term->getData().get(), module);

  if (term->isStatic()) {
    if (data == 0) return term->getTerm(index).get();
    else if (data->isA<Integer>()) {
      return term->getTerm(static_cast<Integer*>(data)->get()).get();
    } else if (data->isA<List>()) {
      Integer *index2 = io_cast<Integer>(static_cast<List*>(data)->get(index).get());
      if (index2 == 0) {
        throw InvalidArgumentException(STR("data"),
                                       STR("Core::Data::GrammarHelper::useListTermChild"),
                                       STR("List must contain Integers for static list terms."));
      }
      return term->getTerm(index2->get()).get();
    } else {
      throw InvalidArgumentException(STR("data"), STR("Core::Data::GrammarHelper::useListTermChild"),
                                     STR("Must be Integer or List for static list terms."));
    }
  } else {
    if (data == 0) {
      throw InvalidArgumentException(STR("data"), STR("Core::Data::GrammarHelper::useListTermChild"),
                                     STR("Must not be null for dynamic list terms."));
    } else if (!data->isA<List>()) {
      throw InvalidArgumentException(STR("data"), STR("Core::Data::GrammarHelper::useListTermChild"),
                                     STR("Must be of type List for dynamic list terms."));
    } else {
      if (term->getTargetRef() == 0) {
        throw GeneralException(STR("Dynamic term doesn't have a target ref."),
                               STR("Core::Data::GrammarHelper::useListTermChild"));
      }
      IdentifiableObject *val = static_cast<List*>(data)->get(index).get();
      context->setPlainValue(term->getTargetRef().get(), val);
      ASSERT(term->getTerms()->isDerivedFrom<Term>());
      return static_cast<Term*>(term->getTerms().get());
    }
  }
}


Integer* GrammarHelper::getTokenTermId(TokenTerm *term, Module *module) const
{
  IdentifiableObject *id = this->traceValue(term->getTokenId().get(), module);
  if (id == 0 || !id->isA<Integer>()) {
    throw GeneralException(STR("Token term's ID is invalid."),
                           STR("Core::Data::GrammarHelper::getTokenTermId"));
  }
  return static_cast<Integer*>(id);
}


IdentifiableObject* GrammarHelper::getTokenTermText(TokenTerm *term, Module *module) const
{
  IdentifiableObject *text = this->traceValue(term->getTokenText().get(), module);
  if (text != 0 && !text->isA<String>() && !text->isA<Map>()) {
    throw GeneralException(STR("Token term's text is of invalid type."),
                           STR("Core::Data::GrammarHelper::getTokenTermText"));
  }
  return text;
}


void GrammarHelper::getReferencedDefinition(Reference *ref, Module *&retModule, SymbolDefinition *&retDef,
                                            Module *module) const
{
  IdentifiableObject *val = ref;
  do {
    this->getValue(static_cast<Reference*>(val), module, val, module);
    // If the reference points to a grammar module, then the reference wants the module's start (default) definition.
    if (val != 0 && val->isA<GrammarModule>()) {
      module = static_cast<GrammarModule*>(val);
      val = static_cast<GrammarModule*>(val)->getStartRef().get();
    }
  } while (val != 0 && val->isA<Reference>());
  if (val == 0 || !val->isDerivedFrom<SymbolDefinition>()) {
    throw GeneralException(STR("Reference does not point to a definition."),
                           STR("Core::Data::GrammarHelper::getReferencedDefinition"));
  }
  retDef = static_cast<SymbolDefinition*>(val);
  retModule = module;
}


Integer* GrammarHelper::getMultiplyTermMax(MultiplyTerm *term, Module *module) const
{
  IdentifiableObject *max = this->traceValue(term->getMaxOccurances().get(), module);
  if (max != 0 && !max->isA<Integer>()) {
    throw GeneralException(STR("Multiply term's max occurances is of invalid type."),
                           STR("Core::Data::GrammarHelper::getMultiplyTermMax"));
  }
  return static_cast<Integer*>(max);
}


Integer* GrammarHelper::getMultiplyTermMin(MultiplyTerm *term, Module *module) const
{
  IdentifiableObject *min = this->traceValue(term->getMinOccurances().get(), module);
  if (min != 0 && !min->isA<Integer>()) {
    throw GeneralException(STR("Multiply term's min occurances is of invalid type."),
                           STR("Core::Data::GrammarHelper::getMultiplyTermMin"));
  }
  return static_cast<Integer*>(min);
}


Integer* GrammarHelper::getMultiplyTermPriority(MultiplyTerm *term, Module *module) const
{
  IdentifiableObject *priority = this->traceValue(term->getPriority().get(), module);
  if (priority != 0 && !priority->isA<Integer>()) {
    throw GeneralException(STR("Multiply term's priority occurances is of invalid type."),
                           STR("Core::Data::GrammarHelper::getMultiplyTermPriority"));
  }
  return static_cast<Integer*>(priority);
}


//==============================================================================
// Symbol Definition Helper Functions

Term* GrammarHelper::getSymbolTerm(const SymbolDefinition *definition, Module *module) const
{
  IdentifiableObject *term = definition->getTerm().get();
  if (term != 0 && term->isA<Reference>()) {
    do {
      this->getValue(static_cast<Reference*>(term), module, term, module);
      // A definition could have a term reference to another definition which means it wants the terms of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (term != 0 && term->isA<SymbolDefinition>()) {
        term = static_cast<SymbolDefinition*>(term)->getTerm().get();
      }
    } while (term != 0 && term->isA<Reference>());
  }
  if (term == 0 || !term->isDerivedFrom<Term>()) {
    throw InvalidArgumentException(STR("definition"), STR("Core::Data::GrammarHelper::getSymbolTerm"),
                                   STR("Symbol's term should be of type Term or Reference to it."));
  }
  return static_cast<Term*>(term);
}


Map* GrammarHelper::getSymbolVars(const SymbolDefinition *definition, Module *module) const
{
  IdentifiableObject *vars = definition->getVars().get();
  if (vars != 0 && vars->isA<Reference>()) {
    do {
      this->getValue(static_cast<Reference*>(vars), module, vars, module);
      // A definition could have a vars reference to another definition which means it wants the vars of that
      // other definition. This is used in cases where a definition is inheriting from another definition.
      if (vars != 0 && vars->isA<SymbolDefinition>()) {
        vars = static_cast<SymbolDefinition*>(vars)->getVars().get();
      }
    } while (vars != 0 && vars->isA<Reference>());
  }
  if (vars != 0 && !vars->isA<Map>()) {
    throw InvalidArgumentException(STR("definition"), STR("Core::Data::GrammarHelper::getSymbolVars"),
                                   STR("Symbol's vars should be of type Map or Reference to it."));
  }
  return static_cast<Map*>(vars);
}

} } // namespace

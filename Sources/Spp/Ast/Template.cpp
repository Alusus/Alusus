/**
 * @file Spp/Ast/Template.cpp
 * Contains the implementation of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

//==============================================================================
// Member Functions

TioSharedPtr const& Template::getDefaultInstance(Helper *helper)
{
  if (this->body == 0) {
    throw EXCEPTION(GenericException, S("Template body is not set."));
  }

  // Do we already have a default instance?
  for (Int i = 0; i < this->instances.getCount(); ++i) {
    if (this->instances.get(i)->getCount() == 1) {
      return this->instances.get(i)->get(0);
    }
  }
  // No default instance was found, create a new one.
  auto block = newSrdObj<Core::Data::Ast::Scope>();
  block->add(Core::Data::Ast::clone(this->body.get()));
  this->instances.add(block);
  block->setOwner(this);
  return this->instances.get(this->instances.getCount() - 1)->get(0);
}


Bool Template::matchInstance(TiObject *templateInputs, Helper *helper, TioSharedPtr &result)
{
  if (this->body == 0) {
    throw EXCEPTION(GenericException, S("Template body is not set."));
  }

  SharedPtr<Core::Notices::Notice> notice;

  // Prepare the template variables.
  PlainList<TiObject> vars;
  if (!this->prepareTemplateVars(templateInputs, helper, &vars, notice)) {
    result = notice;
    return false;
  }

  // Do we already have an instance?
  auto count = this->instances.getCount();
  for (Int i = 0; i < count; ++i) {
    if (this->matchTemplateVars(&vars, this->instances.getElement(i), helper, notice)) {
      result = this->instances.get(i)->get(0);
      return true;
    } else {
      if (notice != 0) {
        result = notice;
        return false;
      }
    }
  }

  // No instance was found, create a new one.
  auto block = newSrdObj<Core::Data::Ast::Scope>();
  block->setSourceLocation(Core::Data::Ast::findSourceLocation(templateInputs));
  block->add(Core::Data::Ast::clone(this->body.get(), Core::Data::Ast::findSourceLocation(templateInputs).get()));
  if (!this->assignTemplateVars(&vars, block.get(), helper, notice)) {
    result = notice;
    return false;
  }
  this->instances.add(block);
  block->setOwner(this);
  result = this->instances.get(this->instances.getCount() - 1)->get(0);
  return true;
}


Bool Template::prepareTemplateVars(
  TiObject *templateInputs, Helper *helper, PlainList<TiObject> *vars, SharedPtr<Core::Notices::Notice> &notice
) {
  auto list = ti_cast<Core::Data::Ast::List>(templateInputs);
  if (list != 0 && list->getCount() > this->getVarDefCount()) {
    notice = newSrdObj<Spp::Notices::TemplateArgMismatchNotice>(
      Core::Data::Ast::findSourceLocation(templateInputs)
    );
    return false;
  }

  for (Int i = 0; i < this->getVarDefCount(); ++i) {
    TiObject *templateInput;
    auto varDef = this->varDefs->get(i).s_cast_get<TemplateVarDef>();
    if (varDef == 0) {
      throw EXCEPTION(GenericException, S("Invalid template variable definition."));
    }
    if (list != 0 && i < list->getCount()) templateInput = list->getElement(i);
    else if (list == 0 && i == 0) templateInput = templateInputs;
    else if (varDef->getDefaultVal() != 0) templateInput = varDef->getDefaultVal().get();
    else {
      notice = newSrdObj<Spp::Notices::TemplateArgMismatchNotice>(
        Core::Data::Ast::findSourceLocation(templateInputs)
      );
      return false;
    }

    auto var = Template::traceObject(templateInput, varDef->getType(), helper);
    if (var == 0) {
      notice = newSrdObj<Spp::Notices::InvalidTemplateArgNotice>(
        Core::Data::Ast::findSourceLocation(templateInput)
      );
      return false;
    }
    vars->add(var);
  }
  return true;
}


Bool Template::matchTemplateVars(
  Containing<TiObject> *templateInputs, Core::Data::Ast::Scope *instance, Helper *helper,
  SharedPtr<Core::Notices::Notice> &notice
) {
  for (Int i = 0; i < this->getVarDefCount(); ++i) {
    auto varDef = this->varDefs->get(i).s_cast_get<TemplateVarDef>();
    ASSERT(varDef != 0);
    if (!this->matchTemplateVar(templateInputs->getElement(i), instance, varDef, helper, notice)) return false;
  }
  return true;
}


Bool Template::matchTemplateVar(
  TiObject *templateInput, Core::Data::Ast::Scope *instance, TemplateVarDef *varDef, Helper *helper,
  SharedPtr<Core::Notices::Notice> &notice
) {
  switch (varDef->getType().get()) {
    case TemplateVarType::INTEGER: {
      auto var = static_cast<Core::Data::Ast::IntegerLiteral*>(
        Template::getTemplateVar(instance, varDef->getName().get())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      auto newVar = static_cast<Core::Data::Ast::IntegerLiteral*>(templateInput);
      ASSERT(newVar != 0);
      return std::stol(newVar->getValue().get()) == std::stol(var->getValue().get());
    }

    case TemplateVarType::STRING: {
      auto var = static_cast<Core::Data::Ast::StringLiteral*>(
        Template::getTemplateVar(instance, varDef->getName().get())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      auto newVar = static_cast<Core::Data::Ast::StringLiteral*>(templateInput);
      ASSERT(newVar != 0);
      return newVar->getValue() == var->getValue();
    }

    case TemplateVarType::MODULE: {
      auto var = Template::getTemplateVar(instance, varDef->getName().get());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return templateInput == var;
    }

    case TemplateVarType::TYPE: {
      auto var = static_cast<Spp::Ast::Type*>(
        Template::getTemplateVar(instance, varDef->getName().get())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing or invalid variable in template instance."));
      }
      auto newVar = static_cast<Spp::Ast::Type*>(templateInput);
      ASSERT(newVar != 0);
      // For performance reason we'll only use `isIdentical` in case of function types since other types are
      // guaranteed to be singletons.
      if (var->isA<Spp::Ast::FunctionType>()) return newVar->isIdentical(var, helper);
      else return newVar == var;
    }

    case TemplateVarType::FUNCTION: {
      auto var = Template::getTemplateVar(instance, varDef->getName().get());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return templateInput == var;
    }

    case TemplateVarType::AST: {
      auto var = Template::getTemplateVar(instance, varDef->getName().get());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing or invalid variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return Core::Data::Ast::isEqual(var, templateInput);
    }

    case TemplateVarType::AST_REF: {
      auto var = Template::getTemplateVar(instance, varDef->getName().get());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return templateInput == var;
    }

    default: {
      throw EXCEPTION(GenericException, S("Unexpected template var type."));
    }
  }
}


Bool Template::assignTemplateVars(
  Containing<TiObject> *templateInputs, Core::Data::Ast::Scope *instance, Helper *helper,
  SharedPtr<Core::Notices::Notice> &notice
) {
  for (Int i = 0; i < this->getVarDefCount(); ++i) {
    auto varDef = this->varDefs->get(i).s_cast_get<TemplateVarDef>();
    ASSERT(varDef != 0);
    auto var = templateInputs->getElement(i);
    auto def = Core::Data::Ast::Definition::create();
    def->setName(varDef->getName().get());
    if (varDef->getType() == TemplateVarType::INTEGER || varDef->getType() == TemplateVarType::STRING) {
      def->setTarget(Core::Data::Ast::clone(var));
    } else {
      def->setTarget(Core::Data::Ast::Passage::create(var));
    }
    instance->add(def);
  }
  return true;
}


TiObject* Template::getTemplateVar(Core::Data::Ast::Scope const *instance, Char const *name)
{
  for (Int i = 0; i < instance->getCount(); ++i) {
    auto def = ti_cast<Core::Data::Ast::Definition>(instance->getElement(i));
    if (def != 0 && def->getName() == name) {
      auto passage = def->getTarget().ti_cast_get<Core::Data::Ast::Passage>();
      if (passage != 0) return passage->get();
      else return def->getTarget().get();
    }
  }
  throw EXCEPTION(GenericException, S("Template var not found."));
}


TiObject* Template::traceObject(TiObject *ref, TemplateVarType varType, Helper *helper)
{
  if (ref->isDerivedFrom<Core::Data::Ast::Passage>()) {
    ref = static_cast<Core::Data::Ast::Passage*>(ref)->get();
  }
  TiObject *result = 0;
  Node *refNode = ti_cast<Node>(ref);
  if (refNode == 0) {
    throw EXCEPTION(GenericException, S("Invalid template variable."));
  }
  if (varType == TemplateVarType::INTEGER) {
    if (ref->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Core::Data::Ast::IntegerLiteral>(ref, refNode->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::STRING) {
    if (ref->isDerivedFrom<Core::Data::Ast::StringLiteral>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Core::Data::Ast::StringLiteral>(ref, refNode->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::FUNCTION) {
    // TODO: Replace with Helper::traceFunction that considers templates.
    if (ref->isDerivedFrom<Spp::Ast::Function>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Spp::Ast::Function>(ref, refNode->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::TYPE) {
    if (ref->isDerivedFrom<Spp::Ast::Type>()) result = ref;
    else {
      result = helper->traceType(ref);
    }
  } else if (varType == TemplateVarType::MODULE) {
    if (ref->isDerivedFrom<Spp::Ast::Module>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Spp::Ast::Module>(ref, refNode->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::AST) {
    result = ref;
  } else if (varType == TemplateVarType::AST_REF) {
    if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<TiObject>(ref, refNode->getOwner(), result, 0);
    } else result = ref;
  }
  return result;
}


//==============================================================================
// Mergeable Implementation

Bool Template::merge(TiObject *src, Core::Data::Seeker *seeker, Core::Notices::Store *noticeStore)
{
  auto mergeable = this->body.ti_cast_get<Core::Data::Ast::Mergeable>();
  if (mergeable != 0) {
    if (!mergeable->merge(src, seeker, noticeStore)) return false;
    // Merge into the body of already created instances.
    for (Int i = 0; i < this->instances.getCount(); ++i) {
      auto block = this->instances.get(i).get();
      mergeable = block->get(0).ti_cast_get<Core::Data::Ast::Mergeable>();
      if (!mergeable->merge(
        Core::Data::Ast::clone(src, Core::Data::Ast::findSourceLocation(block).get()).get(),
        seeker,
        noticeStore
      )) return false;
    }
    return true;
  } else {
    noticeStore->add(
      newSrdObj<Core::Notices::IncompatibleDefMergeNotice>(Core::Data::Ast::findSourceLocation(src))
    );
    return false;
  }
}


//==============================================================================
// Printable Implementation

void Template::print(OutStream &stream, Int indents) const
{
  stream << S("Template") ;
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
  }
  // dump var defs
  stream << S("\n");
  printIndents(stream, indents+1);
  stream << S("-varDefs:");
  for (Word i = 0; i < this->getVarDefCount(); ++i) {
    auto varDef = this->varDefs->get(i).s_cast_get<TemplateVarDef>();
    if (varDef == 0) {
      throw EXCEPTION(GenericException, S("Invalid template variable definition."));
    }
    stream << S("\n");
    printIndents(stream, indents+2);
    stream << varDef->getName() << S(": ");
    switch (varDef->getType().get()) {
      case TemplateVarType::INTEGER: stream << S("Integer"); break;
      case TemplateVarType::STRING: stream << S("String"); break;
      case TemplateVarType::MODULE: stream << S("Module"); break;
      case TemplateVarType::TYPE: stream << S("Type"); break;
      case TemplateVarType::FUNCTION: stream << S("Function"); break;
      case TemplateVarType::AST: stream << S("Ast"); break;
      case TemplateVarType::AST_REF: stream << S("AstRef"); break;
    }
  }
  // dump body
  stream << S("\n");
  printIndents(stream, indents+1);
  stream << S("-body:\n");
  printIndents(stream, indents+2);
  Core::Data::dumpData(stream, this->body.get(), indents+2);
  // dump instances
  stream << S("\n");
  printIndents(stream, indents+1);
  stream << S("-instances:");
  for (Word i = 0; i < this->instances.getCount(); ++i) {
    stream << S("\n");
    printIndents(stream, indents+2);
    Core::Data::dumpData(stream, this->instances.getElement(i), indents+2);
  }
}

} // namespace

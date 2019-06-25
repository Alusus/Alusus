/**
 * @file Spp/Ast/Template.cpp
 * Contains the implementation of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  for (Int i = 0; i < this->instances.size(); ++i) {
    if (this->instances[i]->getCount() == 1) {
      return this->instances[i]->get(0);
    }
  }
  // No default instance was found, create a new one.
  auto block = std::make_shared<Core::Data::Ast::Scope>();
  block->add(Core::Data::Ast::clone(this->body));
  this->instances.push_back(block);
  block->setOwner(this);
  return this->instances.back()->get(0);
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
  auto count = this->instances.size();
  for (Int i = 0; i < count; ++i) {
    if (this->matchTemplateVars(&vars, this->instances[i].get(), helper, notice)) {
      result = this->instances[i]->get(0);
      return true;
    } else {
      if (notice != 0) {
        result = notice;
        return false;
      }
    }
  }

  // No instance was found, create a new one.
  auto block = std::make_shared<Core::Data::Ast::Scope>();
  block->add(Core::Data::Ast::clone(this->body, Core::Data::Ast::findSourceLocation(templateInputs).get()));
  if (!this->assignTemplateVars(&vars, block.get(), helper, notice)) {
    result = notice;
    return false;
  }
  this->instances.push_back(block);
  block->setOwner(this);
  result = this->instances.back()->get(0);
  return true;
}


Bool Template::prepareTemplateVars(
  TiObject *templateInputs, Helper *helper, PlainList<TiObject> *vars, SharedPtr<Core::Notices::Notice> &notice
) {
  auto list = ti_cast<Core::Data::Ast::List>(templateInputs);
  if (list != 0 && list->getCount() > this->getVarDefCount()) {
    notice = std::make_shared<Spp::Notices::TemplateArgMismatchNotice>(
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
      notice = std::make_shared<Spp::Notices::TemplateArgMismatchNotice>(
        Core::Data::Ast::findSourceLocation(templateInputs)
      );
      return false;
    }

    switch (varDef->getType().get()) {
      case TemplateVarType::INTEGER: {
        auto var = ti_cast<Core::Data::Ast::IntegerLiteral>(
          Template::traceObject(templateInput, TemplateVarType::INTEGER, helper)
        );
        if (var == 0) {
          notice = std::make_shared<Spp::Notices::InvalidTemplateArgNotice>(
            Core::Data::Ast::findSourceLocation(templateInput)
          );
          return false;
        }
        vars->add(var);
        break;
      }

      case TemplateVarType::STRING: {
        auto var = ti_cast<Core::Data::Ast::StringLiteral>(
          Template::traceObject(templateInput, TemplateVarType::STRING, helper)
        );
        if (var == 0) {
          notice = std::make_shared<Spp::Notices::InvalidTemplateArgNotice>(
            Core::Data::Ast::findSourceLocation(templateInput)
          );
          return false;
        }
        vars->add(var);
        break;
      }

      default: {
        auto var = Template::traceObject(templateInput, varDef->getType(), helper);
        if (var == 0) {
          notice = std::make_shared<Spp::Notices::InvalidTemplateArgNotice>(
            Core::Data::Ast::findSourceLocation(templateInput)
          );
          return false;
        }
        vars->add(var);
        break;
      }
    }
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
      auto var = ti_cast<TiInt>(
        Template::getTemplateVar(instance, varDef->getName().get())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      auto newVar = static_cast<Core::Data::Ast::IntegerLiteral*>(templateInput);
      ASSERT(newVar != 0);
      return std::stol(newVar->getValue().get()) == var->get();
    }

    case TemplateVarType::STRING: {
      auto var = ti_cast<TiStr>(
        Template::getTemplateVar(instance, varDef->getName().get())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      auto newVar = static_cast<Core::Data::Ast::StringLiteral*>(templateInput);
      ASSERT(newVar != 0);
      return newVar->getValue() == var->get();
    }

    default: {
      auto var = Template::getTemplateVar(instance, varDef->getName().get());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return templateInput == var;
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
    if (varDef->getType() == TemplateVarType::INTEGER) {
      auto intLiteral = static_cast<Core::Data::Ast::IntegerLiteral*>(var);
      def->setTarget(TiInt::create(std::stol(intLiteral->getValue().get())));
    } else if (varDef->getType() == TemplateVarType::STRING) {
      auto strLiteral = static_cast<Core::Data::Ast::StringLiteral*>(var);
      def->setTarget(std::make_shared<TiStr>(strLiteral->getValue().get()));
    } else {
      def->setTarget(std::make_shared<TioWeakBox>(getWeakPtr(var)));
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
      auto box = def->getTarget().ti_cast_get<TioWeakBox>();
      if (box != 0) return box->get().lock().get();
      else return def->getTarget().get();
    }
  }
  throw EXCEPTION(GenericException, S("Template var not found."));
}


TiObject* Template::traceObject(TiObject *ref, TemplateVarType varType, Helper *helper)
{
  TiObject *result = 0;
  Node *refNode = ti_cast<Node>(ref);
  if (refNode == 0) {
    throw EXCEPTION(GenericException, S("Invalid template variable."));
  }
  if (varType == TemplateVarType::INTEGER) {
    if (ref->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) result = ref;
    else {
      helper->getSeeker()->find<Core::Data::Ast::IntegerLiteral>(ref, refNode->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::STRING) {
    if (ref->isDerivedFrom<Core::Data::Ast::StringLiteral>()) result = ref;
    else {
      helper->getSeeker()->find<Core::Data::Ast::StringLiteral>(ref, refNode->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::FUNCTION) {
    // TODO: Replace with Helper::traceFunction that considers templates.
    if (ref->isDerivedFrom<Spp::Ast::Function>()) result = ref;
    else {
      helper->getSeeker()->find<Spp::Ast::Function>(ref, refNode->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::TYPE) {
    if (ref->isDerivedFrom<Spp::Ast::Type>()) result = ref;
    else {
      result = helper->traceType(ref);
    }
  }
  return result;
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
      case TemplateVarType::INTEGER: stream << S("TiInt"); break;
      case TemplateVarType::STRING: stream << S("TiStr"); break;
      case TemplateVarType::TYPE: stream << S("Type"); break;
      case TemplateVarType::FUNCTION: stream << S("Function"); break;
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
  for (Word i = 0; i < this->instances.size(); ++i) {
    stream << S("\n");
    printIndents(stream, indents+2);
    Core::Data::dumpData(stream, this->instances[i].get(), indents+2);
  }
}

} // namespace

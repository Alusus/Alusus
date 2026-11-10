/**
 * @file Spp/Ast/Template.cpp
 * Contains the implementation of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

SharedPtr<Core::Ast::Node> const& Template::getDefaultInstance(Helper *helper)
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
  auto block = newSrdObj<Core::Ast::Scope>();
  block->add(Core::Ast::clone(this->body.get()));
  PlainList<Core::Ast::Node> vars;
  this->instancesIndex[this->hashTemplateVars(&vars)].push_back(this->instances.getCount());
  this->instances.add(block);
  block->setOwner(this);
  return this->instances.get(this->instances.getCount() - 1)->get(0);
}


Bool Template::matchInstance(
  Core::Ast::Node *templateInputs, Helper *helper, SharedPtr<Core::Ast::Node> &result,
  SharedPtr<Core::Notices::Notice> &notice
) {
  if (this->body == 0) {
    throw EXCEPTION(GenericException, S("Template body is not set."));
  }

  // Prepare the template variables.
  PlainList<Core::Ast::Node> vars;
  if (!this->prepareTemplateVars(templateInputs, helper, &vars, notice)) {
    return false;
  }

  // Do we already have an instance?
  auto &candidates = this->instancesIndex[this->hashTemplateVars(&vars)];
  for (auto i : candidates) {
    if (this->matchTemplateVars(&vars, this->instances.getElement(i), helper, notice)) {
      result = this->instances.get(i)->get(0);
      return true;
    } else {
      if (notice != 0) return false;
    }
  }

  // No instance was found, create a new one.
  auto block = newSrdObj<Core::Ast::Scope>();
  block->setSourceLocation(Core::Ast::findSourceLocation(templateInputs));
  block->add(Core::Ast::clone(this->body.get(), Core::Ast::findSourceLocation(templateInputs).get()));
  if (!this->assignTemplateVars(&vars, block.get(), helper, notice)) {
    return false;
  }
  candidates.push_back(this->instances.getCount());
  this->instances.add(block);
  block->setOwner(this);
  result = this->instances.get(this->instances.getCount() - 1)->get(0);
  return true;
}


Bool Template::prepareTemplateVars(
  Core::Ast::Node *templateInputs, Helper *helper, PlainList<Core::Ast::Node> *vars,
  SharedPtr<Core::Notices::Notice> &notice
) {
  auto list = ti_cast<Core::Ast::List>(templateInputs);
  if (list != 0 && list->getCount() > this->getVarDefCount()) {
    notice = newSrdObj<Spp::Notices::TemplateArgMismatchNotice>(
      Core::Ast::findSourceLocation(templateInputs)
    );
    return false;
  }

  for (Int i = 0; i < this->getVarDefCount(); ++i) {
    Core::Ast::Node *templateInput;
    auto varDef = this->varDefs->get(i).s_cast_get<TemplateVarDef>();
    if (varDef == 0) {
      throw EXCEPTION(GenericException, S("Invalid template variable definition."));
    }
    if (list != 0 && i < list->getCount()) templateInput = list->getElement(i);
    else if (list == 0 && i == 0) templateInput = templateInputs;
    else if (varDef->getDefaultVal() != 0) templateInput = varDef->getDefaultVal().get();
    else {
      notice = newSrdObj<Spp::Notices::TemplateArgMismatchNotice>(
        Core::Ast::findSourceLocation(templateInputs)
      );
      return false;
    }

    auto var = Template::traceObject(templateInput, varDef->getType(), helper);
    if (var == 0) {
      notice = newSrdObj<Spp::Notices::InvalidTemplateArgNotice>(
        Core::Ast::findSourceLocation(templateInput)
      );
      return false;
    }
    vars->add(var);
  }
  return true;
}


Bool Template::matchTemplateVars(
  Containing<Core::Ast::Node> *templateInputs, Core::Ast::Scope *instance, Helper *helper,
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
  Core::Ast::Node *templateInput, Core::Ast::Scope *instance, TemplateVarDef *varDef, Helper *helper,
  SharedPtr<Core::Notices::Notice> &notice
) {
  switch (varDef->getType().get()) {
    case TemplateVarType::INTEGER: {
      auto var = static_cast<Core::Ast::IntegerLiteral*>(
        Template::getTemplateVar(instance, varDef->getName().getStr())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      auto newVar = static_cast<Core::Ast::IntegerLiteral*>(templateInput);
      ASSERT(newVar != 0);
      return std::stol(newVar->getValue().get()) == std::stol(var->getValue().get());
    }

    case TemplateVarType::STRING: {
      auto var = static_cast<Core::Ast::StringLiteral*>(
        Template::getTemplateVar(instance, varDef->getName().getStr())
      );
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      auto newVar = static_cast<Core::Ast::StringLiteral*>(templateInput);
      ASSERT(newVar != 0);
      return newVar->getValue() == var->getValue();
    }

    case TemplateVarType::MODULE: {
      auto var = Template::getTemplateVar(instance, varDef->getName().getStr());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return templateInput == var;
    }

    case TemplateVarType::TYPE: {
      auto var = static_cast<Spp::Ast::Type*>(
        Template::getTemplateVar(instance, varDef->getName().getStr())
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
      auto var = Template::getTemplateVar(instance, varDef->getName().getStr());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return templateInput == var;
    }

    case TemplateVarType::AST: {
      auto var = Template::getTemplateVar(instance, varDef->getName().getStr());
      if (var == 0) {
        throw EXCEPTION(GenericException, S("Missing or invalid variable in template instance."));
      }
      ASSERT(templateInput != 0);
      return Core::Ast::isEqual(var, templateInput);
    }

    case TemplateVarType::AST_REF: {
      auto var = Template::getTemplateVar(instance, varDef->getName().getStr());
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


std::size_t Template::hashTemplateVars(Containing<Core::Ast::Node> *templateInputs)
{
  // Equal vars, as determined by matchTemplateVar, must always produce equal hashes.
  std::size_t hash = 0;
  for (Int i = 0; i < templateInputs->getElementCount(); ++i) {
    auto varDef = this->varDefs->get(i).s_cast_get<TemplateVarDef>();
    ASSERT(varDef != 0);
    auto var = templateInputs->getElement(i);
    std::size_t varHash = 0;
    switch (varDef->getType().get()) {
      case TemplateVarType::INTEGER:
        varHash = std::hash<long>{}(std::stol(static_cast<Core::Ast::IntegerLiteral*>(var)->getValue().get()));
        break;
      case TemplateVarType::STRING:
        varHash = std::hash<std::string_view>{}(static_cast<Core::Ast::StringLiteral*>(var)->getValue().get());
        break;
      case TemplateVarType::TYPE:
        // Function types are compared with isIdentical rather than by identity.
        if (!var->isA<Spp::Ast::FunctionType>()) varHash = std::hash<void*>{}(var);
        break;
      case TemplateVarType::MODULE:
      case TemplateVarType::FUNCTION:
      case TemplateVarType::AST_REF:
        varHash = std::hash<void*>{}(var);
        break;
      default:
        // AST vars are compared with isEqual.
        break;
    }
    hash ^= varHash + 0x9e3779b97f4a7c15 + (hash << 6) + (hash >> 2);
  }
  return hash;
}


Bool Template::assignTemplateVars(
  Containing<Core::Ast::Node> *templateInputs, Core::Ast::Scope *instance, Helper *helper,
  SharedPtr<Core::Notices::Notice> &notice
) {
  for (Int i = 0; i < this->getVarDefCount(); ++i) {
    auto varDef = this->varDefs->get(i).s_cast_get<TemplateVarDef>();
    ASSERT(varDef != 0);
    auto var = templateInputs->getElement(i);
    auto def = Core::Ast::Definition::create();
    def->setName(varDef->getName().get());
    if (varDef->getType() == TemplateVarType::INTEGER || varDef->getType() == TemplateVarType::STRING) {
      def->setTarget(Core::Ast::clone(var));
    } else {
      def->setTarget(Core::Ast::Passage::create(var));
    }
    instance->add(def);
  }
  return true;
}


Core::Ast::Node* Template::getTemplateVar(Core::Ast::Scope const *instance, Str const &name)
{
  auto defs = instance->findDefinitions(name);
  if (defs == 0 || defs->size() == 0) {
    throw EXCEPTION(GenericException, S("Template var not found."));
  }
  auto def = defs->front();
  auto passage = def->getTarget().ti_cast_get<Core::Ast::Passage>();
  if (passage != 0) return passage->get();
  else return def->getTarget().get();
}


Core::Ast::Node* Template::traceObject(Core::Ast::Node *ref, TemplateVarType varType, Helper *helper)
{
  if (ref->isDerivedFrom<Core::Ast::Passage>()) {
    ref = static_cast<Core::Ast::Passage*>(ref)->get();
  }
  Node *result = 0;
  if (varType == TemplateVarType::INTEGER) {
    if (ref->isDerivedFrom<Core::Ast::IntegerLiteral>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Core::Ast::IntegerLiteral>(ref, ref->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::STRING) {
    if (ref->isDerivedFrom<Core::Ast::StringLiteral>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Core::Ast::StringLiteral>(ref, ref->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::FUNCTION) {
    // TODO: Replace with Helper::traceFunction that considers templates.
    if (ref->isDerivedFrom<Spp::Ast::Function>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Spp::Ast::Function>(ref, ref->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::TYPE) {
    if (ref->isDerivedFrom<Spp::Ast::Type>()) result = ref;
    else {
      result = helper->traceType(ref);
    }
  } else if (varType == TemplateVarType::MODULE) {
    if (ref->isDerivedFrom<Spp::Ast::Module>()) result = ref;
    else if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Spp::Ast::Module>(ref, ref->getOwner(), result, 0);
    }
  } else if (varType == TemplateVarType::AST) {
    result = ref;
  } else if (varType == TemplateVarType::AST_REF) {
    if (helper->isAstReference(ref)) {
      helper->getSeeker()->find<Core::Ast::Node>(ref, ref->getOwner(), result, 0);
    } else result = ref;
  }
  return result;
}


//==============================================================================
// Mergeable Implementation

Bool Template::merge(Core::Ast::Node *src, Core::Ast::Seeker *seeker, Core::Notices::Store *noticeStore)
{
  auto mergeable = this->body.ti_cast_get<Core::Ast::Mergeable>();
  if (mergeable != 0) {
    if (!mergeable->merge(src, seeker, noticeStore)) return false;
    // Merge into the body of already created instances.
    for (Int i = 0; i < this->instances.getCount(); ++i) {
      auto block = this->instances.get(i).get();
      mergeable = block->get(0).ti_cast_get<Core::Ast::Mergeable>();
      if (!mergeable->merge(
        Core::Ast::clone(src, Core::Ast::findSourceLocation(block).get()).get(),
        seeker,
        noticeStore
      )) return false;
    }
    return true;
  } else {
    noticeStore->add(
      newSrdObj<Core::Notices::IncompatibleDefMergeNotice>(Core::Ast::findSourceLocation(src))
    );
    return false;
  }
}


//==============================================================================
// Printing Functions

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
  Node::printModifiers(stream, indents);
  Node::printMetadata(stream, indents);
  // dump body
  stream << S("\n");
  printIndents(stream, indents+1);
  stream << S("-body:\n");
  printIndents(stream, indents+2);
  Core::Ast::dumpAst(stream, this->body.get(), indents+2);
  // dump instances
  stream << S("\n");
  printIndents(stream, indents+1);
  stream << S("-instances:");
  for (Word i = 0; i < this->instances.getCount(); ++i) {
    stream << S("\n");
    printIndents(stream, indents+2);
    Core::Ast::dumpAst(stream, this->instances.getElement(i), indents+2);
  }
}

} // namespace

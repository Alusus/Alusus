/**
 * @file Spp/Ast/Template.cpp
 * Contains the implementation of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Member Functions

SharedPtr<Block> const& Template::getDefaultInstance(Seeker *seeker)
{
  if (this->templateBody == 0) {
    throw EXCEPTION(GenericException, STR("Template body is not set."));
  }

  // Do we already have a default instance?
  for (Int i = 0; i < this->instances.size(); ++i) {
    if (this->instances[i]->getCount() == 0) {
      return this->instances[i];
    }
  }
  // No default instance was found, create a new one.
  auto block = std::make_shared<Block>();
  block->setStatements(this->templateBody->clone());
  this->instances.push_back(block);
  return this->instances.back();
}

SharedPtr<Block> const& Template::getInstance(TiObject *templateInput, Seeker *seeker)
{
  if (this->templateBody == 0) {
    throw EXCEPTION(GenericException, STR("Template body is not set."));
  }

  // Do we already have a default instance?
  for (Int i = 0; i < this->instances.size(); ++i) {
    if (this->matchTemplateVars(templateInput, this->instances[0].get(), seeker)) {
      return this->instances[i];
    }
  }
  // No default instance was found, create a new one.
  auto block = std::make_shared<Block>();
  block->setStatements(this->templateBody->clone());
  this->assignTemplateVars(templateInput, block.get(), seeker);
  this->instances.push_back(block);
  return this->instances.back();
}


Bool Template::matchTemplateVars(TiObject *templateInput, Block *instance, Seeker *seeker)
{
  // TODO:
  throw EXCEPTION(GenericException, "Not implemented yet.");
}


void Template::assignTemplateVars(TiObject *templateInput, Block *instance, Seeker *seeker)
{
  // TODO:
  throw EXCEPTION(GenericException, "Not implemented yet.");
}


//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> Template::clone() const
{
  SharedPtr<Template> newTemplate = std::make_shared<Template>();
  newTemplate->setProdId(this->getProdId());
  newTemplate->setSourceLocation(this->getSourceLocation());
  for (Word i = 0; i < this->varDefs.size(); ++i) {
    newTemplate->setVarDefs(this->getVarDefs());
  }
  newTemplate->setTemplateBody(this->getTemplateBody());
  return newTemplate;
}


//==============================================================================
// Printable Implementation

void Template::print(OutStream &stream, Int indents) const
{
  stream << STR("Template") ;
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << ID_GENERATOR->getDesc(id) << STR("]");
  }
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-varDefs:");
  for (Word i = 0; i < this->varDefs.size(); ++i) {
    stream << STR("\n");
    printIndents(stream, indents+2);
    stream << this->varDefs[i].first << STR(": ");
    switch (this->varDefs[i].second.val) {
      case VarType::INTEGER: stream << STR("Integer"); break;
      case VarType::STRING: stream << STR("String"); break;
      case VarType::ALIAS: stream << STR("Alias"); break;
    }
  }
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-templateBody:\n");
  printIndents(stream, indents+2);
  Core::Data::dumpData(stream, this->templateBody->getTiObject(), indents+2);
}

} } // namespace

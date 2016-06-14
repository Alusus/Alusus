/**
 * @file Core/Data/Ast/ParamPass.cpp
 * Contains the implementation of class Core::Data::Ast::ParamPass.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data { namespace Ast
{

//==============================================================================
// Container Implementation

void ParamPass::set(Int index, TiObject *val)
{
  switch (index) {
    case 0: this->setOperand(getSharedPtr(val, true)); break;
    case 1: this->setParam(getSharedPtr(val, true)); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be either 0 or 1."), index);
  }
}

void ParamPass::remove(Int index)
{
  switch (index) {
    case 0: this->setOperand(TioSharedPtr()); break;
    case 1: this->setParam(TioSharedPtr()); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be either 0 or 1."), index);
  }
}

TiObject* ParamPass::get(Int index) const
{
  switch (index) {
    case 0: return this->operand.get();
    case 1: return this->param.get();
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be either 0 or 1."), index);
  }
}


//==============================================================================
// MetadataHolder Overrides

/**
 * @brief Override the original implementation to search the tree if needed.
 * If the value is not yet set for this object and it has children, it will
 * call getSourceLocation on the children.
 */
SourceLocation const& ParamPass::getSourceLocation() const
{
  if (MetadataHolder::getSourceLocation().line == 0) {
    for (Int i = 0; i < this->getCount(); ++i) {
      MetadataHolder *ptr = tii_cast<MetadataHolder>(this->get(i));
      if (ptr != 0) {
        SourceLocation const &sl = ptr->getSourceLocation();
        if (sl.line != 0) return sl;
      }
    }
  }
  return MetadataHolder::getSourceLocation();
}

TiObject* ParamPass::getAttribute(Char const *name)
{
  if (SBSTR(name) == STR("sourceLocation")) {
    if (MetadataHolder::getSourceLocation().line == 0) {
      for (Int i = 0; i < this->getCount(); ++i) {
        MetadataHolder *ptr = tii_cast<MetadataHolder>(this->get(i));
        if (ptr != 0) {
          if (ptr->getSourceLocation().line != 0) return ptr->getAttribute(name);
        }
      }
    }
  } else if (SBSTR(name) == STR("type")) {
    return &this->type;
  }
  return MetadataHolder::getAttribute(name);
}


//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> ParamPass::clone() const
{
  SharedPtr<ParamPass> newParamPass = std::make_shared<ParamPass>();
  newParamPass->setProdId(this->getProdId());
  for (Word i = 0; i < this->getCount(); ++i) {
    newParamPass->set(i, this->get(i));
  }
  newParamPass->setType(this->type);
  newParamPass->setSourceLocation(this->getSourceLocation());
  return newParamPass;
}


//==============================================================================
// Printable Implementation

void ParamPass::print(OutStream &stream, Int indents) const
{
  stream << STR("ParamPass ") << (this->type == BracketType::ROUND ? STR("()") : STR("[]"));
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
  }

  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-operand: ");
  dumpData(stream, this->operand.get(), indents+1);

  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("-param: ");
  dumpData(stream, this->param.get(), indents+1);
}

} } } // namespace

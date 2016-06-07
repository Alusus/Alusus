/**
 * @file Core/Data/Ast/InfixOperator.cpp
 * Contains the implementation of class Core::Data::Ast::InfixOperator.
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

void InfixOperator::set(Int index, TiObject *val)
{
  switch (index) {
    case 0: this->setFirst(getSharedPtr(val, true)); break;
    case 1: this->setSecond(getSharedPtr(val, true)); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be either 0 or 1."), index);
  }
}

void InfixOperator::remove(Int index)
{
  switch (index) {
    case 0: this->setFirst(TioSharedPtr()); break;
    case 1: this->setSecond(TioSharedPtr()); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be either 0 or 1."), index);
  }
}

TiObject* InfixOperator::get(Int index) const
{
  switch (index) {
    case 0: return this->first.get();
    case 1: return this->second.get();
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
SourceLocation const& InfixOperator::getSourceLocation() const
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

TiObject* InfixOperator::getAttribute(Char const *name)
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

SharedPtr<TiObject> InfixOperator::clone() const
{
  SharedPtr<InfixOperator> newInfixOperator = std::make_shared<InfixOperator>();
  newInfixOperator->setProdId(this->getProdId());
  for (Word i = 0; i < this->getCount(); ++i) {
    newInfixOperator->set(i, this->get(i));
  }
  newInfixOperator->setType(this->type.get());
  newInfixOperator->setSourceLocation(this->getSourceLocation());
  return newInfixOperator;
}


//==============================================================================
// Printable Implementation

void InfixOperator::print(OutStream &stream, Int indents) const
{
  stream << STR("InfixOperator") ;
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
  }
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("Type: ") << this->type.get();

  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("First: ");
  dumpData(stream, this->first.get(), indents+1);

  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("Second: ");
  dumpData(stream, this->second.get(), indents+1);
}

} } } // namespace

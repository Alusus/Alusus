/**
 * @file Core/Data/Ast/Bracket.cpp
 * Contains the implementation of class Core::Data::Ast::Bracket.
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

void Bracket::set(Int index, TiObject *val)
{
  switch (index) {
    case 0: this->setOperand(getSharedPtr(val, true)); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0."), index);
  }
}

void Bracket::remove(Int index)
{
  switch (index) {
    case 0: this->setOperand(TioSharedPtr()); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0."), index);
  }
}

TiObject* Bracket::get(Int index) const
{
  switch (index) {
    case 0: return this->operand.get();
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0."), index);
  }
}


//==============================================================================
// MetadataHolder Overrides

/**
 * @brief Override the original implementation to search the tree if needed.
 * If the value is not yet set for this object and it has children, it will
 * call getSourceLocation on the children.
 */
SourceLocation const& Bracket::getSourceLocation() const
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

TiObject* Bracket::getAttribute(Char const *name)
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

SharedPtr<TiObject> Bracket::clone() const
{
  SharedPtr<Bracket> newBracket = std::make_shared<Bracket>();
  newBracket->setProdId(this->getProdId());
  for (Word i = 0; i < this->getCount(); ++i) {
    newBracket->set(i, this->get(i));
  }
  newBracket->setType(this->type);
  newBracket->setSourceLocation(this->getSourceLocation());
  return newBracket;
}


//==============================================================================
// Printable Implementation

void Bracket::print(OutStream &stream, Int indents) const
{
  stream << STR("Bracket") ;
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
  }
  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("Type: ") << (this->type == BracketType::ROUND ? STR("()") : STR("[]"));

  stream << STR("\n");
  printIndents(stream, indents+1);
  stream << STR("Operand: ");
  dumpData(stream, this->operand.get(), indents+1);
}

} } } // namespace

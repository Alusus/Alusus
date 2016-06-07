/**
 * @file Core/Data/Ast/List.cpp
 * Contains the implementation of class Core::Data::Ast::List.
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
// MetadataHolder Overrides

/**
 * @brief Override the original implementation to search the tree if needed.
 * If the value is not yet set for this object and it has children, it will
 * call getSourceLocation on the children.
 */
SourceLocation const& List::getSourceLocation() const
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

TiObject* List::getAttribute(Char const *name)
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
  }
  return MetadataHolder::getAttribute(name);
}


//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> List::clone() const
{
  SharedPtr<List> newList = std::make_shared<List>();
  newList->setProdId(this->getProdId());
  for (Word i = 0; i < this->getCount(); ++i) {
    newList->add(this->get(i));
  }
  newList->setSourceLocation(this->getSourceLocation());
  return newList;
}


//==============================================================================
// Printable Implementation

void List::print(OutStream &stream, Int indents) const
{
  stream << STR("List") ;
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
  }
  for (Word i = 0; i < this->getCount(); ++i) {
    stream << STR("\n");
    printIndents(stream, indents+1);
    dumpData(stream, this->get(i), indents+1);
  }
}

} } } // namespace

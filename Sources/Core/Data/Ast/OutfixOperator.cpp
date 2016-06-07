/**
 * @file Core/Data/Ast/OutfixOperator.cpp
 * Contains the implementation of class Core::Data::Ast::OutfixOperator.
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

void OutfixOperator::set(Int index, TiObject *val)
{
  switch (index) {
    case 0: this->setOperand(getSharedPtr(val, true)); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0."), index);
  }
}

void OutfixOperator::remove(Int index)
{
  switch (index) {
    case 0: this->setOperand(TioSharedPtr()); break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0."), index);
  }
}

TiObject* OutfixOperator::get(Int index) const
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
SourceLocation const& OutfixOperator::getSourceLocation() const
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

TiObject* OutfixOperator::getAttribute(Char const *name)
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

} } } // namespace

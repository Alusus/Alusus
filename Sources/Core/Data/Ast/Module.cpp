/**
 * @file Core/Data/Ast/Module.cpp
 * Contains the implementation of class Core::Data::Ast::Module.
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
// Constructors & Destructor

Module::Module(const std::initializer_list<Argument<Char const*>> &args) : SharedMap(true)
{
  for (auto arg : args) {
    this->add(arg.id, arg.ioVal);
  }
}


//==============================================================================
// MetadataHolder Overrides

/**
 * @brief Override the original implementation to search the tree if needed.
 * If the value is not yet set for this object and it has children, it will
 * call getSourceLocation on the children.
 */
SourceLocation const& Module::getSourceLocation() const
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


TiObject* Module::getAttribute(Char const *name)
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

SharedPtr<TiObject> Module::clone() const
{
  SharedPtr<Module> newModule = std::make_shared<Module>();
  newModule->setProdId(this->getProdId());
  for (Word i = 0; i < this->getCount(); ++i) {
    newModule->add(this->getKey(i).c_str(), this->getShared(i));
  }
  newModule->setSourceLocation(this->getSourceLocation());
  return newModule;
}


//==============================================================================
// Printable Implementation

void Module::print(OutStream &stream, Int indents) const
{
  stream << STR("Module") ;
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
  }
  for (Word i = 0; i < this->getCount(); ++i) {
    stream << STR("\n");
    printIndents(stream, indents+1);
    stream << this->getKey(i).c_str() << STR(": ");
    dumpData(stream, this->get(i), indents+1);
  }
}

} } } // namespace

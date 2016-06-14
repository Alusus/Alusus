/**
 * @file Core/Data/Ast/Route.cpp
 * Contains the implementation of class Core::Data::Ast::Route.
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
SourceLocation const& Route::getSourceLocation() const
{
  if (MetadataHolder::getSourceLocation().line == 0) {
    MetadataHolder *ptr = tii_cast<MetadataHolder>(this->getData().get());
    if (ptr != 0) {
      SourceLocation const &sl = ptr->getSourceLocation();
      if (sl.line != 0) return sl;
    }
  }
  return MetadataHolder::getSourceLocation();
}

TiObject* Route::getAttribute(Char const *name)
{
  if (SBSTR(name) == STR("sourceLocation")) {
    if (MetadataHolder::getSourceLocation().line == 0) {
      MetadataHolder *ptr = tii_cast<MetadataHolder>(this->getData().get());
      if (ptr != 0) {
        return ptr->getAttribute(name);
      }
    }
  }
  return MetadataHolder::getAttribute(name);
}


//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> Route::clone() const
{
  SharedPtr<Route> newRoute = std::make_shared<Route>();
  newRoute->setProdId(this->getProdId());
  newRoute->setRoute(this->getRoute());
  newRoute->setData(this->getData());
  newRoute->setSourceLocation(this->getSourceLocation());
  return newRoute;
}


//==============================================================================
// Printable Implementation

void Route::print(OutStream &stream, Int indents) const
{
  stream << STR("Route");
  stream << STR(" (") << this->getRoute() << STR(")");
  Word id = this->getProdId();
  if (id != UNKNOWN_ID) {
    stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("] ");
  }
  if (this->getData() != 0) {
    stream << STR("\n");
    printIndents(stream, indents+1);
    dumpData(stream, this->getData().get(), indents+1);
  }
}

} } } // namespace


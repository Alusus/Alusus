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

} } } // namespace


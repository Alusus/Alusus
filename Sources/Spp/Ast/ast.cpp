/**
 * @file Spp/Ast/ast.cpp
 * Contains the implementations of global functions in the Ast namespace.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

Type* traceType(TiObject *ref, Core::Data::Seeker *seeker)
{
  Type *result = 0;
  auto *refNode = ti_cast<Core::Data::Node>(ref);
  if (refNode == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  if (refNode->isDerivedFrom<Type>()) {
    return static_cast<Type*>(refNode);
  }
  seeker->doForeach(refNode, refNode->getOwner(),
    [=, &result](TiObject *obj, Core::Data::Notice*)->Core::Data::Seeker::Verb
    {
      if (obj->isDerivedFrom<Type>()) {
        result = static_cast<Type*>(obj);
        return Core::Data::Seeker::Verb::STOP;
      }
      // TODO: Recurse if the object is an Alias.
      return Core::Data::Seeker::Verb::MOVE;
    }
  );
  if (result == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  return result;
}


Bool isVarDefinition(TiObject *obj)
{
  return
    obj->isDerivedFrom<Core::Data::Ast::ParamPass>() ||
    obj->isDerivedFrom<Core::Data::Ast::LinkOperator>() ||
    obj->isDerivedFrom<Core::Data::Ast::Identifier>();
}

} } // namespace

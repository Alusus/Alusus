/**
 * @file Spp/Ast/Function.cpp
 * Contains the implementation of class Spp::Ast::Function.
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

//==============================================================================
// Member Functions

Function::CallMatchStatus Function::matchCall(
  Core::Data::Container *types, ExecutionContext const *context, Core::Data::Seeker *seeker
) {
  if (types == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("types"), STR("Cannot be null."));
  }
  if (seeker == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("seeker"), STR("Cannot be null."));
  }
  if (types->getCount() < this->argTypes->getCount()) {
    return CallMatchStatus::NONE;
  } else if (types->getCount() > this->argTypes->getCount() && !this->isVariadic()) {
    return CallMatchStatus::NONE;
  } else {
    Bool casted = false;
    for (Int i = 0; i < this->argTypes->getCount(); ++i) {
      Type *wantedType = this->traceType(this->argTypes->get(i), seeker);
      Type *providedType = this->traceType(types->get(i), seeker);
      if (wantedType != providedType) {
        if (providedType->isImplicitlyCastableTo(providedType, context, seeker)) {
          casted = true;
        } else {
          return CallMatchStatus::NONE;
        }
      }
    }
    return casted ? CallMatchStatus::CASTED : CallMatchStatus::EXACT;
  }
}


Type* Function::traceType(TiObject *ref, Core::Data::Seeker *seeker)
{
  Spp::Ast::Type *result = 0;
  auto *refNode = ti_cast<Core::Data::Node>(ref);
  if (refNode == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  if (refNode->isDerivedFrom<Type>()) {
    return static_cast<Type*>(refNode);
  }
  seeker->doForeach(refNode, refNode->getOwner(), [=, &result](TiObject *obj)->Core::Data::Seeker::SeekVerb
  {
    if (obj->isDerivedFrom<Type>()) {
      result = static_cast<Type*>(obj);
      return Core::Data::Seeker::SeekVerb::STOP;
    }
    // TODO: Recurse if the object is an Alias.
    return Core::Data::Seeker::SeekVerb::MOVE;
  });
  if (result == 0) {
    throw EXCEPTION(GenericException, STR("Invalid type reference."));
  }
  return result;
}


//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> Function::clone() const
{
  SharedPtr<Function> newFunction = std::make_shared<Function>();
  newFunction->setProdId(this->getProdId());
  newFunction->setSourceLocation(this->getSourceLocation());
  for (Word i = 0; i < this->getCount(); ++i) {
    newFunction->set(this->getKey(i).c_str(), this->get(i));
  }
  newFunction->setName(this->name.get());
  newFunction->setVariadic(this->isVariadic());
  return newFunction;
}

} } // namespace

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
  Core::Basic::Container<Core::Basic::TiObject> *types, ExecutionContext const *context,
  Core::Standard::RootManager *rootManager
) {
  if (types == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("types"), STR("Cannot be null."));
  }
  if (rootManager == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("rootManager"), STR("Cannot be null."));
  }
  Word argCount = this->argTypes == 0 ? 0 : this->argTypes->getCount();
  if (argCount == 0) {
    return types->getElementCount() == 0 ? CallMatchStatus::EXACT : CallMatchStatus::NONE;
  } else if (types->getElementCount() < argCount) {
    return CallMatchStatus::NONE;
  } else if (types->getElementCount() > argCount && !this->isVariadic()) {
    return CallMatchStatus::NONE;
  } else {
    Bool casted = false;
    for (Int i = 0; i < argCount; ++i) {
      Type *wantedType = this->traceType(this->argTypes->get(i), rootManager->getSeeker());
      Type *providedType = this->traceType(types->getElement(i), rootManager->getSeeker());
      if (wantedType != providedType) {
        if (providedType->isImplicitlyCastableTo(wantedType, context, rootManager)) {
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

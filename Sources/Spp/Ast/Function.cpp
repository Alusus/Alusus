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

Type* Function::traceArgType(Int index, Helper *helper) const
{
  if (this->argTypes == 0 || this->argTypes->getCount() == 0) {
    throw EXCEPTION(GenericException, STR("Function takes no arguments."));
  }
  if (this->argTypes == 0 || index < 0 || index >= this->argTypes->getCount()) {
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range."), index);
  }
  return helper->traceType(this->argTypes->get(index));
}


Type* Function::traceRetType(Helper *helper) const
{
  return helper->traceType(this->retType.get());
}


Bool Function::isVariadic() const
{
  return
    this->argTypes != 0 &&
    this->argTypes->getCount() > 0 &&
    this->argTypes->get(this->argTypes->getCount() - 1)->isDerivedFrom<ArgPack>();
}


CallMatchStatus Function::matchCall(Core::Basic::Container<Core::Basic::TiObject> *types, Helper *helper)
{
  if (helper == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("helper"), STR("Cannot be null."));
  }

  Word argCount = this->argTypes == 0 ? 0 : this->argTypes->getCount();
  if (argCount == 0) {
    return types == 0 || types->getElementCount() == 0 ? CallMatchStatus::EXACT : CallMatchStatus::NONE;
  } else {
    Bool casted = false;
    Bool deref = false;
    Function::ArgMatchContext matchContext;
    if (types != 0) {
      for (Int i = 0; i < types->getElementCount(); ++i) {
        CallMatchStatus status = this->matchNextArg(types->getElement(i), matchContext, helper);
        if (status == CallMatchStatus::NONE) return CallMatchStatus::NONE;
        else if (status == CallMatchStatus::DEREF) deref = true;
        else if (status == CallMatchStatus::CASTED) casted = true;
      }
    }
    // Make sure there is nore more needed args.
    if (matchContext.index == -1) {
      matchContext.index = 0;
      matchContext.subIndex = -1;
    }
    while (matchContext.index < argCount) {
      auto argType = this->argTypes->get(matchContext.index);
      if (argType->isDerivedFrom<ArgPack>()) {
        auto argPack = static_cast<ArgPack*>(argType);
        if (matchContext.subIndex + 1 >= argPack->getMin().get()) {
          matchContext.index++;
          matchContext.subIndex = -1;
        } else {
          return CallMatchStatus::NONE;
        }
      } else {
        if (matchContext.subIndex == 0) {
          matchContext.index++;
          matchContext.subIndex = -1;
        } else {
          return CallMatchStatus::NONE;
        }
      }
    }
    return casted ? CallMatchStatus::CASTED : (deref ? CallMatchStatus::DEREF : CallMatchStatus::EXACT);
  }
}


CallMatchStatus Function::matchNextArg(
  Core::Basic::TiObject *nextType, ArgMatchContext &matchContext, Helper *helper
) {
  if (nextType == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("types"), STR("Cannot be null."));
  }
  if (helper == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("helper"), STR("Cannot be null."));
  }
  if (matchContext.index < -1 || matchContext.subIndex < -1) {
    throw EXCEPTION(InvalidArgumentException, STR("matchContext"), STR("Value is corrupted."));
  }

  Type *providedType = helper->traceType(nextType);
  Word argCount = this->argTypes == 0 ? 0 : this->argTypes->getCount();

  // Check if we are at an arg pack and can add more args to the pack.
  if (matchContext.index >= 0) {
    ASSERT(matchContext.subIndex >= 0);
    if (matchContext.index >= argCount) return CallMatchStatus::NONE;
    auto currentArg = this->argTypes->get(matchContext.index);
    if (currentArg->isDerivedFrom<ArgPack>()) {
      auto currentArgPack = static_cast<ArgPack*>(currentArg);
      if (currentArgPack->getMax() == 0 || matchContext.subIndex + 1 < currentArgPack->getMax().get()) {
        if (matchContext.type == providedType || matchContext.type == 0) {
          matchContext.subIndex++;
          return CallMatchStatus::EXACT;
        } else if (helper->isReferenceTypeFor(providedType, matchContext.type)) {
          matchContext.subIndex++;
          return CallMatchStatus::DEREF;
        } else if (providedType->isImplicitlyCastableTo(matchContext.type, helper)) {
          matchContext.subIndex++;
          return CallMatchStatus::CASTED;
        } else if (matchContext.subIndex + 1 < currentArgPack->getMin().get()) {
          return CallMatchStatus::NONE;
        }
      }
    }
  }

  // If we have more than one arg pack in the function we might be able to skip an arg pack entirely.
  Int steps = 1;
  while (true) {
    if (matchContext.index + steps >= argCount) return CallMatchStatus::NONE;
    auto nextArg = this->argTypes->get(matchContext.index + steps);
    if (nextArg->isDerivedFrom<ArgPack>()) {
      auto nextArgPack = static_cast<ArgPack*>(nextArg);
      Type *wantedType = nextArgPack->getArgType() == 0 ? 0 : helper->traceType(nextArgPack->getArgType().get());
      if (wantedType == providedType || wantedType == 0) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return CallMatchStatus::EXACT;
      } else if (helper->isReferenceTypeFor(providedType, wantedType)) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return CallMatchStatus::DEREF;
      } else if (providedType->isImplicitlyCastableTo(wantedType, helper)) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return CallMatchStatus::CASTED;
      } else if (nextArgPack->getMin() == 0) {
        steps++;
      } else {
        return CallMatchStatus::NONE;
      }
    } else {
      Type *wantedType = helper->traceType(nextArg);
      if (wantedType == providedType) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return CallMatchStatus::EXACT;
      } else if (helper->isReferenceTypeFor(providedType, wantedType)) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return CallMatchStatus::DEREF;
      } else if (providedType->isImplicitlyCastableTo(wantedType, helper)) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return CallMatchStatus::CASTED;
      } else {
        return CallMatchStatus::NONE;
      }
    }
  }
}

} } // namespace

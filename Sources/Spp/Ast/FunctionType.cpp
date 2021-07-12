/**
 * @file Spp/Ast/FunctionType.cpp
 * Contains the implementation of class Spp::Ast::FunctionType.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Ast
{

//==============================================================================
// Member Functions

TypeMatchStatus FunctionType::matchTargetType(
  Type const *type, Helper *helper, ExecutionContext const *ec, TypeMatchOptions opts
) const
{
  VALIDATE_NOT_NULL(type, helper);
  if (this == type) return TypeMatchStatus::EXACT;

  auto functionType = ti_cast<FunctionType>(type);
  if (functionType == 0) return TypeMatchStatus::NONE;

  // Check return type.
  auto thisRetType = this->traceRetType(helper);
  auto retType = functionType->traceRetType(helper);
  if (thisRetType == 0 || retType == 0) return TypeMatchStatus::NONE;
  if (!thisRetType->isEqual(retType, helper, ec)) return TypeMatchStatus::NONE;

  // Check args.
  Word thisArgCount = this->argTypes == 0 ? 0 : this->argTypes->getCount();
  Word argCount = functionType->argTypes == 0 ? 0 : functionType->argTypes->getCount();
  if (thisArgCount != argCount) return TypeMatchStatus::NONE;
  else if (thisArgCount == 0) return TypeMatchStatus::EXACT;
  else {
    auto result = TypeMatchStatus::EXACT;
    for (Int i = 0; i < argCount; ++i) {
      auto thisArg = this->argTypes->getElement(i);
      auto arg = functionType->argTypes->getElement(i);
      auto thisArgPack = ti_cast<ArgPack>(thisArg);
      auto argPack = ti_cast<ArgPack>(arg);

      if ((thisArgPack == 0 && argPack != 0) || (thisArgPack != 0 && argPack == 0)) return TypeMatchStatus::NONE;
      else if (argPack != 0) {
        // Check arg packs.
        if (thisArgPack->getMin() <= argPack->getMin() || thisArgPack->getMax() >= argPack->getMax()) {
          return TypeMatchStatus::NONE;
        } else if (thisArgPack->getMin() != argPack->getMin() || thisArgPack->getMax() != argPack->getMax()) {
          result = TypeMatchStatus::IMPLICIT_CAST;
        }
        Type *thisArgType = thisArgPack->getArgType() == 0 ? 0 : helper->traceType(thisArgPack->getArgType().get());
        Type *argType = argPack->getArgType() == 0 ? 0 : helper->traceType(argPack->getArgType().get());
        if ((thisArgType == 0 && argType != 0) || (thisArgType != 0 && argType == 0)) return TypeMatchStatus::NONE;
        else if (thisArgType != 0 && !thisArgType->isEqual(argType, helper, ec)) return TypeMatchStatus::NONE;
      } else {
        // Check regular args.
        Type *thisArgType = thisArg == 0 ? 0 : helper->traceType(thisArg);
        Type *argType = arg == 0 ? 0 : helper->traceType(arg);
        if (thisArgType == 0 || argType == 0) return TypeMatchStatus::NONE;
        if (!thisArgType->isEqual(argType, helper, ec)) return TypeMatchStatus::NONE;
      }
    }
    return result;
  }
}


Bool FunctionType::isIdentical(Type const *type, Helper *helper) const
{
  if (this == type) return true;

  auto functionType = ti_cast<FunctionType>(type);
  if (functionType == 0) return false;

  if (this->isMember() != functionType->isMember()) return false;

  // Check return type.
  auto thisRetType = this->traceRetType(helper);
  auto retType = functionType->traceRetType(helper);
  if (thisRetType == 0 || retType == 0) return false;
  if (!thisRetType->isIdentical(retType, helper)) return false;

  // Check args.
  Word thisArgCount = this->argTypes == 0 ? 0 : this->argTypes->getCount();
  Word argCount = functionType->argTypes == 0 ? 0 : functionType->argTypes->getCount();
  if (thisArgCount != argCount) return false;

  for (Int i = 0; i < argCount; ++i) {
    auto thisArg = this->argTypes->getElement(i);
    auto arg = functionType->argTypes->getElement(i);
    auto thisArgPack = ti_cast<ArgPack>(thisArg);
    auto argPack = ti_cast<ArgPack>(arg);

    if ((thisArgPack == 0 && argPack != 0) || (thisArgPack != 0 && argPack == 0)) return false;
    else if (argPack != 0) {
      // Check arg packs.
      if (thisArgPack->getMin() != argPack->getMin() || thisArgPack->getMax() != argPack->getMax()) return false;
      Type *thisArgType = thisArgPack->getArgType() == 0 ? 0 : helper->traceType(thisArgPack->getArgType().get());
      Type *argType = argPack->getArgType() == 0 ? 0 : helper->traceType(argPack->getArgType().get());
      if ((thisArgType == 0 && argType != 0) || (thisArgType != 0 && argType == 0)) return false;
      if (thisArgType != 0 && !thisArgType->isIdentical(argType, helper)) return false;
    } else {
      // Check regular args.
      Type *thisArgType = thisArg == 0 ? 0 : helper->traceType(thisArg);
      Type *argType = arg == 0 ? 0 : helper->traceType(arg);
      if (thisArgType == 0 || argType == 0) return false;
      if (!thisArgType->isIdentical(argType, helper)) return false;
    }
  }
  return true;
}


Type* FunctionType::traceArgType(Int index, Helper *helper) const
{
  if (this->argTypes == 0 || this->argTypes->getCount() == 0) {
    throw EXCEPTION(GenericException, S("Function takes no arguments."));
  }
  if (this->argTypes == 0 || index < 0 || index >= this->argTypes->getCount()) {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
  }
  return helper->traceType(this->argTypes->getElement(index));
}


Bool FunctionType::isVariadic() const
{
  return
    this->argTypes != 0 &&
    this->argTypes->getCount() > 0 &&
    this->argTypes->get(this->argTypes->getCount() - 1)->isDerivedFrom<ArgPack>();
}


Type* FunctionType::traceRetType(Helper *helper) const
{
  if (this->retType == 0) {
    return helper->getVoidType();
  } else {
    return helper->traceType(this->retType.get());
  }
}


TypeMatchStatus FunctionType::matchCall(
  Containing<TiObject> *types, Helper *helper, Spp::ExecutionContext const *ec
) {
  if (helper == 0) {
    throw EXCEPTION(InvalidArgumentException, S("helper"), S("Cannot be null."));
  }

  Word argCount = this->argTypes == 0 ? 0 : this->argTypes->getCount();
  if (argCount == 0) {
    return types == 0 || types->getElementCount() == 0 ? TypeMatchStatus::EXACT : TypeMatchStatus::NONE;
  } else {
    TypeMatchStatus result = TypeMatchStatus::EXACT;
    FunctionType::ArgMatchContext matchContext;
    if (types != 0) {
      for (Int i = 0; i < types->getElementCount(); ++i) {
        TypeMatchStatus status = this->matchNextArg(types->getElement(i), matchContext, helper, ec);
        if (status <= TypeMatchStatus::EXPLICIT_CAST) return TypeMatchStatus::NONE;
        else if (status < result) result = status;
      }
    }
    // Make sure there is no more needed args.
    if (matchContext.index == -1) {
      matchContext.index = 0;
      matchContext.subIndex = -1;
    }
    while (matchContext.index < argCount) {
      auto argType = this->argTypes->getElement(matchContext.index);
      if (argType->isDerivedFrom<ArgPack>()) {
        auto argPack = static_cast<ArgPack*>(argType);
        if (matchContext.subIndex + 1 >= argPack->getMin().get()) {
          matchContext.index++;
          matchContext.subIndex = -1;
        } else {
          return TypeMatchStatus::NONE;
        }
      } else {
        if (matchContext.subIndex == 0) {
          matchContext.index++;
          matchContext.subIndex = -1;
        } else {
          return TypeMatchStatus::NONE;
        }
      }
    }
    return result;
  }
}


TypeMatchStatus FunctionType::matchNextArg(
  TiObject *nextType, ArgMatchContext &matchContext, Helper *helper, Spp::ExecutionContext const *ec
) {
  if (nextType == 0) {
    throw EXCEPTION(InvalidArgumentException, S("nextType"), S("Cannot be null."));
  }
  if (helper == 0) {
    throw EXCEPTION(InvalidArgumentException, S("helper"), S("Cannot be null."));
  }
  if (matchContext.index < -1 || matchContext.subIndex < -1) {
    throw EXCEPTION(InvalidArgumentException, S("matchContext"), S("Value is corrupted."));
  }

  Type *providedType = helper->traceType(nextType);
  Word argCount = this->argTypes == 0 ? 0 : this->argTypes->getCount();

  // Check if we are at an arg pack and can add more args to the pack.
  if (matchContext.index >= 0) {
    ASSERT(matchContext.subIndex >= 0);
    if (matchContext.index >= argCount) return TypeMatchStatus::NONE;
    auto currentArg = this->argTypes->getElement(matchContext.index);
    if (currentArg->isDerivedFrom<ArgPack>()) {
      auto currentArgPack = static_cast<ArgPack*>(currentArg);
      if (currentArgPack->getMax() == 0 || matchContext.subIndex + 1 < currentArgPack->getMax().get()) {
        if (matchContext.type == 0 || matchContext.type->isEqual(providedType, helper, ec)) {
          matchContext.subIndex++;
          return TypeMatchStatus::EXACT;
        } else {
          Function *caster;
          auto status = helper->matchTargetType(providedType, matchContext.type, ec, caster);
          if (status >= TypeMatchStatus::CUSTOM_CASTER) {
            matchContext.subIndex++;
            return status;
          } else if (matchContext.subIndex + 1 < currentArgPack->getMin().get()) {
            return TypeMatchStatus::NONE;
          }
        }
      }
    }
  }

  // If we have more than one arg pack in the function we might be able to skip an arg pack entirely.
  Int steps = 1;
  while (true) {
    if (matchContext.index + steps >= argCount) return TypeMatchStatus::NONE;
    auto nextArg = this->argTypes->getElement(matchContext.index + steps);
    if (nextArg->isDerivedFrom<ArgPack>()) {
      auto nextArgPack = static_cast<ArgPack*>(nextArg);
      Type *wantedType = nextArgPack->getArgType() == 0 ? 0 : helper->traceType(nextArgPack->getArgType().get());
      if (wantedType == 0) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return TypeMatchStatus::EXACT;
      } else {
        Function *caster;
        auto status = helper->matchTargetType(providedType, wantedType, ec, caster);
        if (status >= TypeMatchStatus::CUSTOM_CASTER) {
          matchContext.type = wantedType;
          matchContext.index += steps;
          matchContext.subIndex = 0;
          return status;
        } else if (nextArgPack->getMin() == 0) {
          steps++;
        } else {
          return TypeMatchStatus::NONE;
        }
      }
    } else {
      Type *wantedType = helper->traceType(nextArg);
      if (wantedType == 0) return TypeMatchStatus::NONE;
      Function *caster;
      auto status = helper->matchTargetType(providedType, wantedType, ec, caster);
      if (status >= TypeMatchStatus::CUSTOM_CASTER) {
        matchContext.type = wantedType;
        matchContext.index += steps;
        matchContext.subIndex = 0;
        return status;
      } else {
        return TypeMatchStatus::NONE;
      }
    }
  }
}

} // namespace

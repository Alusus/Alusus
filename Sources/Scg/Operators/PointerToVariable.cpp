/**
 * @file Scg/Operators/PointerToVariable.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>
// LLVM header files
#include <llvm/IR/IRBuilder.h>
// Scg files
#include <Containers/Block.h>
#include <Operators/PointerToVariable.h>
#include <Types/PointerType.h>

namespace Scg
{
PointerToVariable::~PointerToVariable()
{
  if (this->valueType != nullptr)
    delete this->valueType;
}

//------------------------------------------------------------------------------

const ValueType *PointerToVariable::GetValueType() const
{
  BLOCK_CHECK;

  if (this->valueType)
    return this->valueType;

  auto var = GetBlock()->GetVariable(this->name);
  if (var == 0)
    THROW_EXCEPTION(UndefinedVariableException,
        "Referencing undefined variable: " + this->name);

  this->valueType = new PointerType(*var->GetValueType());
  return this->valueType;
}

//------------------------------------------------------------------------------

CodeGenerationResult PointerToVariable::GenerateCode()
{
  BLOCK_CHECK;

  // Look up the variable.
  auto var = GetBlock()->GetVariable(this->name);
  if (!var)
    THROW_EXCEPTION(UndefinedVariableException, "Undefined variable: " + this->name);

  // The LLVM Alloca instruction used to allocate the variable is a pointer
  // to the variable, so we just return it.
  return var->GetLlvmValue();
}

//----------------------------------------------------------------------------

std::string PointerToVariable::ToString()
{
  // TODO: Do we need to use std::stringstream?
  return this->name + "~ptr";
}
}

/**
 * @file Scg/Operators/PointerToMemberField.cpp
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
#include <Operators/PointerToMemberField.h>
#include <Values/IntegerConst.h>
#include <Types/IntegerType.h>
#include <Types/PointerType.h>
#include <Types/StructType.h>

namespace Scg
{
PointerToMemberField::~PointerToMemberField()
{
  if (this->valueType != nullptr)
    delete this->valueType;
}

//------------------------------------------------------------------------------

const ValueType *PointerToMemberField::GetValueType() const
{
  if (this->valueType)
    return this->valueType;

  // TODO: Don't use dynamic_cast.
  auto pointerType = dynamic_cast<PointerType*>(this->expression->GetValueType());
  if (pointerType == nullptr)
    THROW_EXCEPTION(InvalidArgumentException, "The expression passed to "
        "PointerToMemberField should be a pointer to a structure.");
  auto structType = dynamic_cast<StructType*>(const_cast<ValueType*>(pointerType->GetContentType()));
  if (structType == nullptr)
    THROW_EXCEPTION(InvalidArgumentException, "Non-structure variable types "
        "doesn't have fields to access.");

  this->valueType = new PointerType(*structType->GetFieldByName(this->fieldName).GetValueType());
  return this->valueType;
}

//----------------------------------------------------------------------------

CodeGenerationResult PointerToMemberField::GenerateCode()
{
  BLOCK_CHECK;

  // Finds the index of the field in the structure.
  // TODO: Don't use dynamic_cast.
  // TODO: Finding the index by name is an O(n) operation, so consider it
  // saving the index here or in GetPointedToType() to speed up compilation
  // time.
  auto pointerType = dynamic_cast<PointerType*>(this->expression->GetValueType());
  if (pointerType == nullptr)
    THROW_EXCEPTION(InvalidArgumentException, "The expression passed to "
        "PointerToMemberField should be a pointer to a structure.");
  auto structType = dynamic_cast<StructType*>(const_cast<ValueType*>(pointerType->GetContentType()));
  if (structType == nullptr)
    THROW_EXCEPTION(InvalidArgumentException, "Non-structure variable types "
        "doesn't have fields to access.");
  auto zero = IntegerType::GetSingleton()->GetLlvmConstant(0);
  auto index = IntegerType::GetSingleton()->GetLlvmConstant(
      structType->GetFieldIndex(this->fieldName));

  // Generates the code of the structure which will return a pointer to the
  // structure, which we will use to generate a pointer to the required field.
  auto llvmPtr = this->expression->GenerateCode().exprValue;

  // Generates a pointer to the required field.
  auto irb = GetBlock()->GetIRBuilder();
  // TODO: We need to delete this pointer in the PostGenerateCode() function.
  this->llvmPointer = irb->CreateGEP(llvmPtr,
      llvm::makeArrayRef(std::vector<llvm::Value*>({zero, index})), "");

  return CodeGenerationResult(this->llvmPointer);
}

//----------------------------------------------------------------------------

std::string PointerToMemberField::ToString()
{
  // TODO: Implement this function.
  return "";
}
}

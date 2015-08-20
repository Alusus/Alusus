/**
 * @file Scg/Operators/MemberFieldReference.cpp
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
#include <Operators/MemberFieldReference.h>
#include <Values/IntegerConst.h>
#include <Types/IntegerType.h>
#include <Types/PointerType.h>
#include <Types/StructType.h>

namespace Scg
{
const ValueTypeSpec * MemberFieldReference::GetValueTypeSpec() const
{
  if (this->valueType)
    return this->valueType->GetValueTypeSpec();

  // TODO: Don't use dynamic_cast.
  auto module = GetModule();
  auto typeSpec = this->expression->GetValueTypeSpec();
  auto pointerType = dynamic_cast<PointerType*>(typeSpec->ToValueType(*module));
  if (pointerType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
        "MemberFieldReference should be a pointer to a structure.");
  auto contType = pointerType->GetContentType();
  auto structType = dynamic_cast<StructType*>(const_cast<ValueType*>(contType));
  if (structType == nullptr) {
    throw EXCEPTION(InvalidArgumentException, "Non-structure variable types "
        "doesn't have fields to access.");
  }

  this->valueType = PointerType::Get(structType->GetFieldByName(this->fieldName).GetValueType());
  return this->valueType->GetValueTypeSpec();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage MemberFieldReference::GenerateCode()
{
  BLOCK_CHECK;

  // Finds the index of the field in the structure.
  // TODO: Don't use dynamic_cast.
  // TODO: Finding the index by name is an O(n) operation, so consider it
  // saving the index here or in GetPointedToType() to speed up compilation
  // time.
  auto module = GetModule();
  auto pointerType = dynamic_cast<PointerType*>(this->expression->GetValueTypeSpec()->ToValueType(*module));
  if (pointerType == nullptr) {
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
        "MemberFieldReference should be a pointer to a structure.");
  }
  auto structType = dynamic_cast<StructType*>(const_cast<ValueType*>(pointerType->GetContentType()));
  if (structType == nullptr) {
    throw EXCEPTION(InvalidArgumentException, "Non-structure variable types "
        "doesn't have fields to access.");
  }
  auto zero = IntegerType::Get()->GetLlvmConstant(0);
  auto index = IntegerType::Get()->GetLlvmConstant(
      structType->GetFieldIndex(this->fieldName));

  // Generates the code of the structure which will return a pointer to the
  // structure, which we will use to generate a pointer to the required field.
  auto llvmPtr = this->expression->GetGeneratedLlvmValue();

  // Generates a pointer to the required field.
  auto irb = GetBlock()->GetIRBuilder();
  // TODO: We need to delete this pointer in the PostGenerateCode() function.
  // TODO: generatedLlvmValue is a duplicate of llvmPointer. Should we just use
  // generatedLlvmValue?
  this->generatedLlvmValue = this->llvmPointer = irb->CreateGEP(llvmPtr,
      llvm::makeArrayRef(std::vector<llvm::Value*>({zero, index})), "");

  return Expression::GenerateCode();
}

//----------------------------------------------------------------------------

std::string MemberFieldReference::ToString()
{
  // TODO: Implement this function.
  return "";
}
}

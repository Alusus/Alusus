/**
 * @file Scg/Operators/ArrayElementReference.cpp
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
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Operators/ArrayElementReference.h>
#include <Values/IntegerConst.h>
#include <Types/ArrayType.h>
#include <Types/IntegerType.h>
#include <Types/PointerType.h>

namespace Scg
{
const ValueTypeSpec * ArrayElementReference::GetValueTypeSpec() const
{
  if (this->valueType)
    return this->valueType->GetValueTypeSpec();

  auto module = GetModule();
  // TODO: Don't use dynamic_cast.
  auto pointerType = dynamic_cast<PointerType*>(this->expression->GetValueTypeSpec()->ToValueType(*module));
  if (pointerType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
        "ArrayElementReference should be a pointer to an array.");
  auto arrayType = dynamic_cast<ArrayType*>(const_cast<ValueType*>(pointerType->GetContentType()));
  if (arrayType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "Non-array variable types "
        "doesn't have elements to access.");

  this->valueType = PointerType::Get(arrayType->GetElementsType());
  return this->valueType->GetValueTypeSpec();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage ArrayElementReference::GenerateCode()
{
  BLOCK_CHECK;

  // Finds the index of the field in the structure.
  // TODO: Don't use dynamic_cast.
  auto module = GetModule();
  auto pointerType = dynamic_cast<PointerType*>(this->expression->GetValueTypeSpec()->ToValueType(*module));
  if (pointerType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
        "ArrayElementReference should be a pointer to an array.");
  auto valType = dynamic_cast<ArrayType*>(const_cast<ValueType*>(pointerType->GetContentType()));
  if (valType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
        "ArrayElementReference should be a pointer to an array.");

  // Generates the code of the structure which will return a pointer to the
  // structure, which we will use to generate a pointer to the required field.
  auto llvmPtr = this->expression->GetGeneratedLlvmValue();
  auto zero = IntegerType::Get()->GetLlvmConstant(0);
  auto index = this->elementIndexExpr->GetGeneratedLlvmValue();

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

std::string ArrayElementReference::ToString()
{
  // TODO: Implement this function.
  return "";
}
}

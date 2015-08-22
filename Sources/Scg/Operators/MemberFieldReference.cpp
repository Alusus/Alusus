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
const ValueTypeSpec * MemberFieldReference::getValueTypeSpec() const
{
  if (this->valueType)
    return this->valueType->getValueTypeSpec();

  // TODO: Don't use dynamic_cast.
  auto module = getModule();
  auto typeSpec = this->expression->getValueTypeSpec();
  auto pointerType = dynamic_cast<PointerType*>(typeSpec->toValueType(*module));

  if (pointerType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
                    "MemberFieldReference should be a pointer to a structure.");

  auto contType = pointerType->getContentType();
  auto structType = dynamic_cast<StructType*>(const_cast<ValueType*>(contType));

  if (structType == nullptr) {
    throw EXCEPTION(InvalidArgumentException, "Non-structure variable types "
                    "doesn't have fields to access.");
  }

  this->valueType = PointerType::get(structType->getFieldByName(this->fieldName).getValueType());
  return this->valueType->getValueTypeSpec();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage MemberFieldReference::generateCode()
{
  BLOCK_CHECK;

  // Finds the index of the field in the structure.
  // TODO: Don't use dynamic_cast.
  // TODO: Finding the index by name is an O(n) operation, so consider it
  // saving the index here or in GetPointedToType() to speed up compilation
  // time.
  auto module = getModule();
  auto pointerType = dynamic_cast<PointerType*>(this->expression->getValueTypeSpec()->toValueType(*module));

  if (pointerType == nullptr) {
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
                    "MemberFieldReference should be a pointer to a structure.");
  }

  auto structType = dynamic_cast<StructType*>(const_cast<ValueType*>(pointerType->getContentType()));

  if (structType == nullptr) {
    throw EXCEPTION(InvalidArgumentException, "Non-structure variable types "
                    "doesn't have fields to access.");
  }

  auto zero = IntegerType::get()->getLlvmConstant(0);
  auto index = IntegerType::get()->getLlvmConstant(
                 structType->getFieldIndex(this->fieldName));

  // Generates the code of the structure which will return a pointer to the
  // structure, which we will use to generate a pointer to the required field.
  auto llvmPtr = this->expression->getGeneratedLlvmValue();

  // Generates a pointer to the required field.
  auto irb = getBlock()->getIRBuilder();
  // TODO: We need to delete this pointer in the PostGenerateCode() function.
  // TODO: generatedLlvmValue is a duplicate of llvmPointer. Should we just use
  // generatedLlvmValue?
  this->generatedLlvmValue = this->llvmPointer = irb->CreateGEP(llvmPtr,
                             llvm::makeArrayRef(std::vector<llvm::Value*>({ zero, index })), "");

  return Expression::generateCode();
}

//----------------------------------------------------------------------------

std::string MemberFieldReference::toString()
{
  // TODO: Implement this function.
  return "";
}
}

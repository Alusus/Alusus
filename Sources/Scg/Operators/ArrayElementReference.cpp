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
const ValueTypeSpec * ArrayElementReference::getValueTypeSpec() const
{
  if (this->valueType)
    return this->valueType->getValueTypeSpec();

  auto module = getModule();
  // TODO: Don't use dynamic_cast.
  auto pointerType = dynamic_cast<PointerType*>(this->expression->getValueTypeSpec()->toValueType(*module));

  if (pointerType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
                    "ArrayElementReference should be a pointer to an array.");

  auto arrayType = dynamic_cast<ArrayType*>(const_cast<ValueType*>(pointerType->getContentType()));

  if (arrayType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "Non-array variable types "
                    "doesn't have elements to access.");

  this->valueType = PointerType::get(arrayType->getElementsType());
  return this->valueType->getValueTypeSpec();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage ArrayElementReference::generateCode()
{
  BLOCK_CHECK;

  // Finds the index of the field in the structure.
  // TODO: Don't use dynamic_cast.
  auto module = getModule();
  auto pointerType = dynamic_cast<PointerType*>(this->expression->getValueTypeSpec()->toValueType(*module));

  if (pointerType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
                    "ArrayElementReference should be a pointer to an array.");

  auto valType = dynamic_cast<ArrayType*>(const_cast<ValueType*>(pointerType->getContentType()));

  if (valType == nullptr)
    throw EXCEPTION(InvalidArgumentException, "The expression passed to "
                    "ArrayElementReference should be a pointer to an array.");

  // Generates the code of the structure which will return a pointer to the
  // structure, which we will use to generate a pointer to the required field.
  auto llvmPtr = this->expression->getGeneratedLlvmValue();
  auto zero = IntegerType::get()->getLlvmConstant(0);
  auto index = this->elementIndexExpr->getGeneratedLlvmValue();

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

std::string ArrayElementReference::toString()
{
  // TODO: Implement this function.
  return "";
}
}

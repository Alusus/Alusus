/**
 * @file Scg/Operators/BinaryOperator.cpp
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
#include <Operators/Content.h>
#include <Operators/BinaryOperator.h>
#include <Types/IntegerType.h>

using namespace llvm;

namespace Scg
{
const ValueTypeSpec * BinaryOperator::getValueTypeSpec() const
{
  // FIXME: Fix this!
  return IntegerType::get()->getValueTypeSpec();
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage BinaryOperator::generateCode()
{
  BLOCK_CHECK;

  auto lhs = getLHS()->getGeneratedLlvmValue();
  auto rhs = getRHS()->getGeneratedLlvmValue();

  if (lhs == nullptr) {
    throw EXCEPTION(InvalidValueException,
                    ("Right-hand side of '=' doesn't evaluate to a value: "
                     + getLHS()->toString()).c_str());
  }

  if (rhs == nullptr) {
    throw EXCEPTION(InvalidValueException,
                    ("Right-hand side of '=' doesn't evaluate to a value: "
                     + getRHS()->toString()).c_str());
  }

  auto irb = getBlock()->getIRBuilder();

  switch (this->opType) {
  case ADD:
  case SUBTRACT:
  case MULTIPLY:
  case DIVISION:
    throw EXCEPTION(RetiredCodeException, "Use __add, __sub, __mul, __div instead.");

  case GREATERTHAN:
    this->llvmValue = irb->CreateICmpSGT(lhs, rhs);
    break;

  case GREATERTHAN_EQUAL:
    this->llvmValue = irb->CreateICmpSGE(lhs, rhs);
    break;

  case LESSTHAN:
    this->llvmValue = irb->CreateICmpSLT(lhs, rhs);
    break;

  case LESSTHAN_EQUAL:
    this->llvmValue = irb->CreateICmpSLE(lhs, rhs);
    break;

  case EQUAL:
    this->llvmValue = irb->CreateICmpEQ(lhs, rhs);
    break;

  case NOTEQUAL:
    this->llvmValue = irb->CreateICmpNE(lhs, rhs);
    break;

  default:
    throw EXCEPTION(UnreachableCodeException, "This code shouldn't be reached.");
  }

  // TODO: generatedLlvmValue is a duplicate of llvmValue. Should we just use
  // generatedLlvmValue?
  this->generatedLlvmValue = llvmValue;
  return Expression::generateCode();
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage BinaryOperator::postGenerateCode()
{
  if (!this->llvmValue->hasNUses(0))
    // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;

  // TODO: Is it possible anymore for this->llvmValue to be of Instruction type?
  auto asInst = dyn_cast<llvm::Instruction>(this->llvmValue);

  if (asInst != nullptr)
    asInst->eraseFromParent();
  else
    delete this->llvmValue;

  this->llvmValue = nullptr;

  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string BinaryOperator::toString()
{
  switch (this->opType) {
  case ADD:
    return getLHS()->toString() + " + " + getRHS()->toString();

  case SUBTRACT:
    return getLHS()->toString() + " - " + getRHS()->toString();

  case MULTIPLY:
    return getLHS()->toString() + " * " + getRHS()->toString();

  case DIVISION:
    return getLHS()->toString() + " / " + getRHS()->toString();

  case GREATERTHAN:
    return getLHS()->toString() + " > " + getRHS()->toString();

  case GREATERTHAN_EQUAL:
    return getLHS()->toString() + " >= " + getRHS()->toString();

  case LESSTHAN:
    return getLHS()->toString() + " < " + getRHS()->toString();

  case LESSTHAN_EQUAL:
    return getLHS()->toString() + " <= " + getRHS()->toString();

  case EQUAL:
    return getLHS()->toString() + " == " + getRHS()->toString();

  case NOTEQUAL:
    return getLHS()->toString() + " != " + getRHS()->toString();

  default:
    return "";
  }
}
}

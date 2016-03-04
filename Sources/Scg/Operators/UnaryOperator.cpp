/**
 * @file Scg/Operators/UnaryOperator.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
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
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Operators/Content.h>
#include <Operators/UnaryOperator.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>
#include <Values/Variable.h>

using namespace llvm;

namespace Scg
{

AstNode::CodeGenerationStage UnaryOperator::generateCode(CodeGenUnit *codeGenUnit)
{
  BLOCK_CHECK;

  // Special case '=' because we don't want to emit the LHS as an expression.
  auto irb = this->findOwner<Block>()->getIRBuilder();

  auto operand = getOperand().io_cast_get<Content>();

  if (operand == nullptr)
    throw EXCEPTION(InvalidOperationException, "The operand of a unary "
                    "operator must be the content of a pointer.");

  auto operandType = operand->getValueTypeSpec()->toValueType(*this->findOwner<Module>());

  if (operandType != IntegerType::get() &&
      operandType != FloatType::get() &&
      operandType != DoubleType::get())
    throw EXCEPTION(InvalidLhsException, "Left-hand side of increment/decrement "
                    "operator must be numeric.");

  // Evaluates the expression of the operand, which should result in a
  // variable.
  auto variable = getOperand()->getGeneratedLlvmValue();

  if (this->operatorType == INCREMENT) {
    if (operandType == IntegerType::get())
      this->llvmValue = irb->CreateAdd(variable, IntegerType::get()->getLlvmConstant(1));
    else if (operandType == FloatType::get())
      this->llvmValue = irb->CreateFAdd(variable, FloatType::get()->getLlvmConstant(1));
    else if (operandType == DoubleType::get())
      this->llvmValue = irb->CreateFAdd(variable, DoubleType::get()->getLlvmConstant(1));
  } else if (this->operatorType == DECREMENT) {
    if (operandType == IntegerType::get())
      this->llvmValue = irb->CreateSub(variable, IntegerType::get()->getLlvmConstant(1));
    else if (operandType == FloatType::get())
      this->llvmValue = irb->CreateFSub(variable, FloatType::get()->getLlvmConstant(1));
    else if (operandType == DoubleType::get())
      this->llvmValue = irb->CreateFSub(variable, DoubleType::get()->getLlvmConstant(1));
  } else if (this->operatorType == NEGATIVE) {
    if (operandType == IntegerType::get())
      this->llvmValue = irb->CreateNeg(variable);
    else if (operandType == FloatType::get())
      this->llvmValue = irb->CreateFNeg(variable);
    else if (operandType == DoubleType::get())
      this->llvmValue = irb->CreateFNeg(variable);
  } else {
    throw EXCEPTION(InvalidOperationException,
                    "Unary operator must be INCREMENT, DECREMENT, or NEGATIVE.");
  }

  this->llvmStoreInst = irb->CreateStore(this->llvmValue, operand->getLlvmPointer());

  this->generatedLlvmValue = this->llvmValue;

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage UnaryOperator::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  if (this->llvmStoreInst) {
    if (!this->llvmStoreInst->hasNUses(0))
      // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;

    this->llvmStoreInst->eraseFromParent();
    this->llvmStoreInst = nullptr;
  } else if (this->llvmValue) {
    if (!this->llvmValue->hasNUses(0))
      // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;

    // TODO: Is there a solution better than dynamic casting?
    auto asInst = dyn_cast<llvm::Instruction>(this->llvmValue);

    if (asInst != 0)
      asInst->eraseFromParent();
    else
      delete this->llvmValue;

    this->llvmValue = nullptr;
  }

  return CodeGenerationStage::None;
}


std::string UnaryOperator::toString()
{
  if (this->operatorType == INCREMENT)
    return getOperand()->toString() + "++";
  else
    return getOperand()->toString() + "--";
}

} // namespace

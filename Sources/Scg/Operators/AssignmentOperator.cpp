/**
 * @file Scg/Operators/AssignmentOperator.cpp
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
#include <Operators/AssignmentOperator.h>

using namespace llvm;

namespace Scg
{

AstNode::CodeGenerationStage AssignmentOperator::generateCode(CodeGenUnit *codeGenUnit)
{
  BLOCK_CHECK;

  auto irb = this->findOwner<Block>()->getIRBuilder();

  // TODO: Don't use dynamic_cast.
  auto lhs = getLHS().io_cast_get<Content>();

  if (lhs == nullptr)
    throw EXCEPTION(InvalidOperationException, "The left-hand side of an "
                    "assignment must be the content of a pointer.");

  if (lhs->getCodeGenerationStage() == AstNode::CodeGenerationStage::CodeGeneration)
    if (lhs->callGenerateCode(codeGenUnit) == AstNode::CodeGenerationStage::CodeGeneration)
      return AstNode::CodeGenerationStage::CodeGeneration;

  // Find the value of the right-hand side expression.
  if (getRHS()->getCodeGenerationStage() == AstNode::CodeGenerationStage::CodeGeneration)
    if (getRHS()->callGenerateCode(codeGenUnit) == AstNode::CodeGenerationStage::CodeGeneration)
      return AstNode::CodeGenerationStage::CodeGeneration;

  auto rhs = getRHS()->getGeneratedLlvmValue();

  if (rhs == 0)
    throw EXCEPTION(InvalidValueException,
                    ("Right-hand side of '=' doesn't evaluate to a value: "
                     + getRHS()->toString()).c_str());

  auto module = this->findOwner<Module>();
  auto targetType = getLHS()->getValueTypeSpec()->toValueType(*module);
  auto sourceType = getRHS()->getValueTypeSpec()->toValueType(*module);

  if (!sourceType->isImplicitlyCastableTo(targetType)) {
    throw EXCEPTION(CompilationErrorException,
                    "The right-hand side cannot be assigned to the left-hand side because "
                    "it has a different type.");
  }

  // Add a store instruction to set the value of the variable.
  if (sourceType != targetType) {
    this->llvmStoreInst = irb->CreateStore(
                            sourceType->createCastInst(irb, rhs, targetType), lhs->getLlvmPointer());
  } else {
    this->llvmStoreInst = irb->CreateStore(rhs, lhs->getLlvmPointer());
  }

  // The generated value for an assignment is the right-hand side of the
  // assignment itself.
  generatedLlvmValue = rhs;

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage AssignmentOperator::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  if (!this->llvmStoreInst->hasNUses(0)) {
    // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;
  }

  this->llvmStoreInst->eraseFromParent();
  this->llvmStoreInst = nullptr;

  return AstNode::postGenerateCode(codeGenUnit);
}


std::string AssignmentOperator::toString()
{
  return getLHS()->toString() + " := " + getRHS()->toString();
}

} // namespace

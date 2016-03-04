/**
 * @file Scg/Operators/Content.cpp
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
#include <Types/PointerType.h>

namespace Scg
{

SharedPtr<ValueTypeSpec> const& Content::getValueTypeSpec() const
{
  // TODO: Don't use dynamic_cast.
  auto pointerType = dynamic_cast<PointerType*>(
    this->expression->getValueTypeSpec()->toValueType(*this->findOwner<Module>()));

  if (pointerType == nullptr)
    throw EXCEPTION(InvalidOperationException, "Trying to find the content "
                    "of a non-pointer type.");

  // TODO: Change the return type to a reference or change getContentType().
  return pointerType->getContentType()->getValueTypeSpec();
}


AstNode::CodeGenerationStage Content::generateCode(CodeGenUnit *codeGenUnit)
{
  BLOCK_CHECK;

  this->pointer = this->expression->getGeneratedLlvmValue();

  // TODO: generatedLlvmValue is a duplicate of llvmPointer. Should we just use
  // generatedLlvmValue?
  if (this->pointer != 0) {
    this->generatedLlvmValue = this->loadInst =
      this->findOwner<Block>()->getIRBuilder()->CreateLoad(this->pointer);
  }

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage Content::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  if (this->loadInst == nullptr)
    return CodeGenerationStage::None;

  if (!this->loadInst->hasNUses(0))
    // Cannot delete the instruction yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;

  this->loadInst->eraseFromParent();
  this->loadInst = nullptr;
  return AstNode::postGenerateCode(codeGenUnit);
}


std::string Content::toString()
{
  // TODO: Implement this function.
  return "";
}

} // namespace

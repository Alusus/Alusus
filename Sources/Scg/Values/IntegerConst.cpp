/**
 * @file Scg/Values/IntegerConst.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>
#include <llvm/IR/Constants.h>

// Scg files
#include <CodeGenUnit.h>
#include <Values/IntegerConst.h>
#include <Types/IntegerType.h>

namespace Scg
{

SharedPtr<ValueTypeSpec> const& IntegerConst::getValueTypeSpec() const
{
  return IntegerType::get()->getValueTypeSpec();
}


AstNode::CodeGenerationStage IntegerConst::generateCode(CodeGenUnit *codeGenUnit)
{
  // TODO: generatedLlvmValue is a duplicate of llvmValue. Should we just use
  // generatedLlvmValue?
  this->generatedLlvmValue = this->llvmValue =
    IntegerType::get()->getLlvmConstant(this->value);
  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage IntegerConst::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  if (this->llvmValue == nullptr)
    // Nothing to delete
    return CodeGenerationStage::None;

  if (!this->llvmValue->hasNUses(0))
    // The value is still in use; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;

  // We don't need to delete the constant, it is handled by LLVM.
  this->llvmValue = nullptr;
  return CodeGenerationStage::None;
}


std::string IntegerConst::toString()
{
  return boost::lexical_cast<std::string>(this->value);
}

} // namespace

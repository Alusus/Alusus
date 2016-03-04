/**
 * @file Scg/Instructions/Return.cpp
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
#include <Instructions/Return.h>

using namespace llvm;

namespace Scg
{

AstNode::CodeGenerationStage Return::generateCode(CodeGenUnit *codeGenUnit)
{
  if (getExpression()->getGeneratedLlvmValue() == nullptr)
    throw EXCEPTION(EvaluationException,
                    ("Expression doesn't evaluate to a value: " + getExpression()->toString()).c_str());

  IRBuilder<> *irBuilder = this->findOwner<Block>()->getIRBuilder();
  irBuilder->CreateRet(getExpression()->getGeneratedLlvmValue());
  this->termInstGenerated = true;
  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage Return::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  SAFE_DELETE_LLVM_INST(this->retInst);
  return CodeGenerationStage::None;
}


std::string Return::toString()
{
  return "return " + getExpression()->toString() + ";";
}

} // namespace

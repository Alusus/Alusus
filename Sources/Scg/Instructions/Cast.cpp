/**
 * @file Scg/Instructions/Cast.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// STL header files

// Scg files
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Instructions/Cast.h>
#include <Operators/AssignmentOperator.h>
#include <Operators/Content.h>
#include <Types/ValueType.h>
#include <Values/Variable.h>

// LLVM header files

using namespace llvm;

namespace Scg
{

AstNode::CodeGenerationStage Cast::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  this->type = this->typeSpec->toValueType(*this->findOwner<Module>());
  return CodeGenerationStage::CodeGeneration;
}


AstNode::CodeGenerationStage Cast::generateCode(CodeGenUnit *codeGenUnit)
{
  auto sourceType = this->expr->getValueTypeSpec()->toValueType(*this->findOwner<Module>());
  auto irb = this->findOwner<Block>()->getIRBuilder();
  // TODO: Should delete the cast instruction.
  generatedLlvmValue = sourceType->createCastInst(irb,
                       this->expr->getGeneratedLlvmValue(), this->type);
  return AstNode::generateCode(codeGenUnit);
}


std::string Cast::toString()
{
  return expr->toString() + "cast[" + type->toString() + "]";
}

} // namespace

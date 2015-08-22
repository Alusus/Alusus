/**
 * @file Scg/Instructions/Cast.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// STL header files

// Scg files
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
Cast::~Cast()
{
  delete this->typeSpec;
}

//----------------------------------------------------------------------------

const ValueTypeSpec *Cast::getValueTypeSpec() const
{
  return this->typeSpec;
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Cast::preGenerateCode()
{
  this->type = this->typeSpec->toValueType(*getModule());
  return CodeGenerationStage::CodeGeneration;
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage Cast::generateCode()
{
  auto sourceType = this->expr->getValueTypeSpec()->toValueType(*getModule());
  auto irb = getBlock()->getIRBuilder();
  // TODO: Should delete the cast instruction.
  generatedLlvmValue = sourceType->createCastInst(irb,
                       this->expr->getGeneratedLlvmValue(), this->type);
  return Expression::generateCode();
}

//----------------------------------------------------------------------------

std::string Cast::toString()
{
  return expr->toString() + "cast[" + type->toString() + "]";
}
}

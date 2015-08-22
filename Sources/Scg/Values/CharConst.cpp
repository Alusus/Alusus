/**
 * @file Scg/Values/CharConst.cpp
 *
 * @copyright Copyright (C) 2015 Hicham OUALI ALAMI
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

#include <llvm/IR/Constants.h>

// Scg files
#include <Values/CharConst.h>
#include <Types/CharType.h>

// LLVM header files

namespace Scg
{
const ValueTypeSpec *CharConst::getValueTypeSpec() const
{
  return CharType::get()->getValueTypeSpec();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage CharConst::generateCode()
{
  // TODO: generatedLlvmValue is a duplicate of llvmValue. Should we just use
  // generatedLlvmValue?
  this->generatedLlvmValue = this->llvmValue =
                               CharType::get()->getLlvmConstant(this->value);
  return Expression::generateCode();
}

//----------------------------------------------------------------------------

Expression::CodeGenerationStage CharConst::postGenerateCode()
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

//----------------------------------------------------------------------------

std::string CharConst::toString()
{
  return boost::lexical_cast<std::string>(this->value);
}
}

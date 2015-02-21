/**
 * @file Scg/Values/FloatConst.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

#include <llvm/IR/Constants.h>

// Scg files
#include <Values/FloatConst.h>
#include <Types/FloatType.h>

// LLVM header files

namespace Scg
{
  ValueType *FloatConst::GetValueType() const
  {
    return FloatType::GetSingleton();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage FloatConst::GenerateCode()
  {
  // TODO: generatedLlvmValue is a duplicate of llvmValue. Should we just use
  // generatedLlvmValue?
    this->generatedLlvmValue = this->llvmValue =
    		FloatType::GetSingleton()->GetLlvmConstant(this->value);
    return Expression::GenerateCode();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage FloatConst::PostGenerateCode()
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

  std::string FloatConst::ToString()
  {
    return boost::lexical_cast<std::string>(this->value);
  }
}

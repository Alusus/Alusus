/**
 * @file Scg/Values/StringConst.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg files
#include <LlvmContainer.h>
#include <Values/StringConst.h>
#include <Types/StringType.h>

// LLVM header files
#include <llvm/IR/Value.h>

namespace Scg
{
  ValueType *StringConst::GetValueType() const
  {
    return StringType::GetSingleton();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage StringConst::GenerateCode()
  {
  // TODO: generatedLlvmValue is a duplicate of llvmValue. Should we just use
  // generatedLlvmValue?
    this->generatedLlvmValue = this->llvmValue =
    		StringType::GetSingleton()->GetLlvmConstant(GetModule(), this->value);
    return Expression::GenerateCode();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage StringConst::PostGenerateCode()
  {
    if (this->llvmValue == nullptr)
      // Nothing to delete
      return CodeGenerationStage::None;
    if (!this->llvmValue->hasNUses(0))
      // The value is still in use; stay in PostCodeGeneration stage.
      return CodeGenerationStage::PostCodeGeneration;
    // TODO: Do we need to delete this? In IntegerConst, FloatConst, and
    // DoubleConst we are not doing this and saying that LLVM handles it!
    delete this->llvmValue;
    this->llvmValue = nullptr;
    return CodeGenerationStage::None;
  }

  //----------------------------------------------------------------------------

  std::string StringConst::ToString()
  {
    return this->value;
  }
}

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
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

// Scg files
#include <Values/FloatConst.h>
#include <Types/FloatType.h>

// LLVM header files
#include <llvm/IR/Value.h>

namespace Scg
{
  ValueType *FloatConst::GetValueType() const
  {
    return FloatType::GetSingleton();
  }

  //----------------------------------------------------------------------------

  CodeGenerationResult FloatConst::GenerateCode()
  {
    this->llvmValue = FloatType::GetSingleton()->GetLlvmConstant(this->value);
    return CodeGenerationResult(this->llvmValue);
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

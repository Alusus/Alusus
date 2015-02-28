/**
 * @file Scg/Instructions/Return.cpp
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

// LLVM header files
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/Block.h>
#include <Instructions/Return.h>

using namespace llvm;

namespace Scg
{
  Expression::CodeGenerationStage Return::GenerateCode()
  {
    if (GetExpression()->GetGeneratedLlvmValue() == nullptr)
      throw EXCEPTION(EvaluationException,
          ("Expression doesn't evaluate to a value: " + GetExpression()->ToString()).c_str());

    IRBuilder<> *irBuilder = GetBlock()->GetIRBuilder();
    irBuilder->CreateRet(GetExpression()->GetGeneratedLlvmValue());
    this->termInstGenerated = true;
    return Expression::GenerateCode();
  }

  //----------------------------------------------------------------------------

  Expression::CodeGenerationStage Return::PostGenerateCode()
  {
    SAFE_DELETE_LLVM_INST(this->retInst);
    return CodeGenerationStage::None;
  }

  //----------------------------------------------------------------------------

  std::string Return::ToString()
  {
    return "return " + GetExpression()->ToString() + ";";
  }
}

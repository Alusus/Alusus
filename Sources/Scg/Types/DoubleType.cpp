/**
 * @file Scg/Types/DoubleType.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg header files
#include <Types/DoubleType.h>
#include <LlvmContainer.h>

// LLVM header files
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/ADT/APFloat.h>

namespace Scg
{
  DoubleType *DoubleType::s_singleton = nullptr;

  //----------------------------------------------------------------------------

  DoubleType::DoubleType() : typeSpec("double")
  {
    this->llvmType = llvm::Type::getDoubleTy(LlvmContainer::GetContext());
    if (s_singleton == nullptr)
      s_singleton = this;
  }

  //----------------------------------------------------------------------------

  llvm::Constant *DoubleType::GetLlvmConstant(double value) const
  {
    return llvm::ConstantFP::get(LlvmContainer::GetContext(),
      llvm::APFloat(value));
  }
}

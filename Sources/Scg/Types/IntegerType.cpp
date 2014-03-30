/**
 * @file Scg/Types/IntegerType.cpp
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
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

// Scg header files
#include <Types/IntegerType.h>
#include <LlvmContainer.h>

// LLVM header files
#include <llvm/ADT/APInt.h>

namespace Scg
{
  IntegerType *IntegerType::s_singleton = nullptr;

  //----------------------------------------------------------------------------

  IntegerType::IntegerType() : typeSpec("int")
  {
    this->llvmType = llvm::Type::getInt32Ty(LlvmContainer::GetContext());
    if (s_singleton == nullptr)
      s_singleton = this;
  }

  //----------------------------------------------------------------------------

  llvm::Constant *IntegerType::GetLlvmConstant(int value) const
  {
    return llvm::ConstantInt::get(LlvmContainer::GetContext(),
      llvm::APInt(sizeof(value) * 8, value, true));
  }
}

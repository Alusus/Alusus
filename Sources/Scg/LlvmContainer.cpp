/**
 * @file Scg/LlvmContainer.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// LLVM header files
#pragma warning(disable: 4146 4800 4355 4996)
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#pragma warning(default: 4146 4800 4355 4996)

// Scg header files
#include <prerequisites.h>
#include <LlvmContainer.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>
#include <Types/StringType.h>
#include <Types/ValueType.h>
#include <Types/VoidType.h>

namespace Scg
{

llvm::DataLayout *LlvmContainer::dataLayout = 0;

void LlvmContainer::initialize()
{
  llvm::InitializeNativeTarget();
  LlvmContainer::dataLayout = new llvm::DataLayout("");
}

void LlvmContainer::finalize()
{
  /*
  delete DoubleType::s_singleton;
  delete FloatType::s_singleton;
  delete IntegerType::s_singleton;
  delete StringType::s_singleton;
  delete VoidType::s_singleton;

  DoubleType::s_singleton = nullptr;
  FloatType::s_singleton= nullptr;
  IntegerType::s_singleton= nullptr;
  StringType::s_singleton= nullptr;
  VoidType::s_singleton= nullptr;
  */
}

llvm::LLVMContext &LlvmContainer::getContext()
{
  return llvm::getGlobalContext();
}

llvm::DataLayout* LlvmContainer::getDataLayout()
{
  return LlvmContainer::dataLayout;
}

}

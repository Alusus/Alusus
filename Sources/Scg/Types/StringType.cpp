/**
* @file Scg/Types/StringType.cpp
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
#include <Containers/Module.h>
#include <LlvmContainer.h>
#include <Types/StringType.h>

// LLVM header files
#include <llvm/IR/GlobalVariable.h>

namespace Scg
{
StringType *StringType::s_singleton = nullptr;

//----------------------------------------------------------------------------

StringType::StringType() : typeSpec("string")
{
  this->llvmType = llvm::Type::getInt8PtrTy(LlvmContainer::getContext());

  if (s_singleton == nullptr)
    s_singleton = this;
}

void StringType::initCastingTargets() const
{
  this->implicitCastingTargets.push_back(StringType::get());

  this->explicitCastingTargets.push_back(StringType::get());
}

//----------------------------------------------------------------------------

llvm::Constant *StringType::getLlvmConstant(Module *module, const std::string &value) const
{
  // TODO: This function should be optimised to ensure the same string is not
  // defined twice in the same module.

  // Define a global variable containing the string value.
  auto charType = llvm::Type::getInt8Ty(LlvmContainer::getContext());
  auto strType = llvm::ArrayType::get(charType, value.size() + 1);

  std::vector<llvm::Constant *> strChars;
  strChars.reserve(value.size() + 1);

  for (unsigned i = 0; i < value.size(); i++)
    strChars.push_back(llvm::ConstantInt::get(charType, value[i]));

  strChars.push_back(llvm::ConstantInt::get(charType, 0));

  auto strConst = llvm::ConstantArray::get(strType, strChars);
  auto var = new llvm::GlobalVariable(*module->getLlvmModule(), strType, true,
                                      llvm::GlobalValue::PrivateLinkage, strConst, module->getTempVarName().c_str());
  var->setAlignment(1);

  // Get a pointer to the global variable and return it as the value of the string.
  std::vector<llvm::Constant *> indices;
  llvm::ConstantInt *zero = llvm::ConstantInt::get(LlvmContainer::getContext(),
                            llvm::APInt(64, 0));
  indices.push_back(zero);
  indices.push_back(zero);
  return llvm::ConstantExpr::getGetElementPtr(var, indices);
}

StringType *StringType::get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new StringType();
  }

  return s_singleton;
}
}

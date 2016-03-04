/**
* @file Scg/Types/VoidType.cpp
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#include <prerequisites.h>

// LLVM header files
#include <llvm/IR/Constants.h>

// Scg header files
#include <Types/VoidType.h>
#include <LlvmContainer.h>

namespace Scg
{

//==============================================================================
// Static Variables

VoidType *VoidType::s_singleton = nullptr;


//==============================================================================
// Constructors & Destructor

VoidType::VoidType()
{
  this->typeSpec = std::make_shared<ValueTypeSpecByName>(STR("void"));
  this->llvmType = llvm::Type::getVoidTy(LlvmContainer::getContext());

  if (s_singleton == nullptr)
    s_singleton = this;
}


//==============================================================================
// Member Functions

void VoidType::initCastingTargets() const
{
}


VoidType* VoidType::get()
{
  // PERFORMANCE: What is the impact of running an unnecessary if statement
  // thousands of times?
  if (s_singleton == nullptr) {
    s_singleton = new VoidType();
  }

  return s_singleton;
}

} // namespace

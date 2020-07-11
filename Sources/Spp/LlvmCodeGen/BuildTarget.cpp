/**
 * @file Spp/LlvmCodeGen/BuildTarget.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::BuildTarget.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::LlvmCodeGen
{

llvm::Type* BuildTarget::getVaListType()
{
  if (this->vaListType == 0) {
    auto int32Type = llvm::Type::getIntNTy(*this->getLlvmContext(), 32);
    auto int8Type = llvm::Type::getIntNTy(*this->getLlvmContext(), 8);
    auto int8PtrType = int8Type->getPointerTo();

    this->vaListType = llvm::StructType::create(*this->getLlvmContext(), "__VaList");
    std::vector<llvm::Type*> structMembers({
      int32Type,
      int32Type,
      int8PtrType,
      int8PtrType
    });
    this->vaListType->setBody(structMembers);
  }
  return this->vaListType;
}

} // namespace

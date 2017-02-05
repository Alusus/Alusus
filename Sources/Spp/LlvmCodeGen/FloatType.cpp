/**
 * @file Spp/LlvmCodeGen/FloatType.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::FloatType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace LlvmCodeGen
{

//==============================================================================
// Constructor

FloatType::FloatType(Int bits) : bitCount(bits)
{
  switch (bits) {
    case 32:
      this->llvmType = llvm::Type::getFloatTy(llvm::getGlobalContext());
      break;
    case 64:
      this->llvmType = llvm::Type::getDoubleTy(llvm::getGlobalContext());
      break;
    case 128:
      this->llvmType = llvm::Type::getFP128Ty(llvm::getGlobalContext());
      break;
    default:
      throw EXCEPTION(InvalidArgumentException, STR("bits"), STR("Should be either 32, 64, or 128."), bits);
  }
}


//==============================================================================
// Member Functions

Bool FloatType::isImplicitlyCastableTo(Type const *type, ExecutionContext const *context) const
{
  auto floatType = tio_cast<FloatType>(type);
  if (floatType != 0 && floatType->getBitCount() >= this->bitCount) return true;
  else return false;
}


Bool FloatType::isExplicitlyCastableTo(Type const *type, ExecutionContext const *context) const
{
  if (type->isDerivedFrom<IntegerType>() || type->isDerivedFrom<FloatType>()) return true;
  else return false;
}

} } // namespace

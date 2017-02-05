/**
 * @file Spp/LlvmCodeGen/IntegerType.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::IntegerType.
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
// Member Functions

Bool IntegerType::isImplicitlyCastableTo(Type const *type, ExecutionContext const *context) const
{
  auto integerType = tio_cast<IntegerType>(type);
  if (integerType != 0 && integerType->getBitCount() >= this->bitCount) return true;
  else return false;
}


Bool IntegerType::isExplicitlyCastableTo(Type const *type, ExecutionContext const *context) const
{
  if (type->isDerivedFrom<IntegerType>() || type->isDerivedFrom<FloatType>()) return true;
  else if (type->isDerivedFrom<PointerType>() && context->getPointerBitCount() == this->bitCount) return true;
  else return false;
}

} } // namespace

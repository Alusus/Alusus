/**
 * @file Spp/Ast/FloatType.cpp
 * Contains the implementation of class Spp::Ast::FloatType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace Ast
{

//==============================================================================
// Member Functions

bool FloatType::isImplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const
{
  auto FloatType = tio_cast<FloatType>(type);
  if (FloatType != 0 && FloatType->getBitCount() >= this->bitCount) return true;
}


bool FloatType::isExplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const
{
  if (type->isDerivedFrom<IntegerType>() || type->isDerivedFrom<FloatType>()) return true;
  else return false;
}

} } // namespace

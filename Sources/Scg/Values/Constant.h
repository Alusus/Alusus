/**
 * @file Scg/Values/Constant.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CONSTANT_H
#define SCG_CONSTANT_H

#include "core.h"
#include <typedefs.h>
#include <Values/Value.h>
#include <llvm_fwd.h>

namespace Scg
{

class ValueType;

/**
 * Represent a constant.
 */
class Constant : public Value
{
  //============================================================================
  // Type Info

  TYPE_INFO(Constant, Value, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The LLVM Value object representing the constant.
  protected: llvm::Value *llvmValue;

}; // class

} // namespace

#endif

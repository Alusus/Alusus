/**
 * @file Scg/Values/Constant.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Constant_h__
#define __Constant_h__

// Scg header files
#include <typedefs.h>
#include <Values/Value.h>

// LLVM class declarations
#include <llvm_fwd.h>

namespace Scg
{
  class ValueType;
}

namespace Scg
{
  /**
   * Represent a constant.
   */
  class Constant : public Value
  {
  protected:
    //! The LLVM Value object representing the constant.
    llvm::Value *llvmValue;

  public:
    /**
     * Get the LLVM type representing that is used to represent values of this type.
     */
    virtual ValueType *GetValueType() const = 0;
  };
}

#endif // __Constant_h__

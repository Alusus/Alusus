/**
 * @file Scg/Types/IntegerType.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __IntegerType_h__
#define __IntegerType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
  //! Represent the integer type.
  class IntegerType : public ValueType
  {
    friend class LlvmContainer;
    static IntegerType *s_singleton;
    ValueTypeSpecByName typeSpec;

  public:
    //! Constructs an integer type.
    IntegerType();

    //! Class destructor.
    virtual ~IntegerType() {}

    //! @copydoc ValueType::GetName()
    virtual const std::string GetName() const { return "int"; }

    /**
     * Get an LLVM constant value of integral type.
     * @param[in] value The value of the constant.
     * @return The llvm::Value object representing the constant.
     */
    llvm::Constant *GetLlvmConstant(int value) const;

    //! @copydoc ValueType::GetDefaultLLVMValue()
    virtual llvm::Constant *GetDefaultLLVMValue() const
    {
      return GetLlvmConstant(0);
    }

    //! @copydoc ValueType::GetValueTypeSpec()
    virtual const ValueTypeSpec *GetValueTypeSpec() const
    {
      return &typeSpec;
    }

    static IntegerType *GetSingleton() { return s_singleton; }
  };
}

#endif // __IntegerType_h__

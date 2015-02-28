/**
 * @file Scg/Types/VoidType.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __VoidType_h__
#define __VoidType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
  /**
   * Represent a variable type.
   */
  class VoidType : public ValueType
  {
    friend class LlvmContainer;
    static VoidType *s_singleton;
    ValueTypeSpecByName typeSpec;

  public:
    /**
     * Constructs a void type.
     */
    VoidType();

    /**
     * Class destructor.
     */
    virtual ~VoidType() {}

    //! @copydoc ValueType::GetName()
    virtual const std::string GetName() const { return "void"; }

    static VoidType *GetSingleton() { return s_singleton; }

    //! @copydoc ValueType::GetDefaultLLVMValue()
    virtual llvm::Constant *GetDefaultLLVMValue() const
    {
      throw EXCEPTION(NotImplementedException, "Not implemented yet!");
    }

    //! @copydoc ValueType::GetValueTypeSpec()
    virtual const ValueTypeSpec *GetValueTypeSpec() const override
    {
      return &typeSpec;
    }

    //! @copydoc ValueType::IsEqualTo()
    virtual bool IsEqualTo(const ValueType *other) const
    {
      // TODO: Should we throw InvalidOperationException here?
      return dynamic_cast<const VoidType*>(other) != nullptr;
    }

    //! @copydoc ValueType::GetImplicitCastingOperator()
    virtual const ValueTypeArray &GetImplicitCastingTargets() const
    {
      return this->implicitCastingTargets;
    }

    //! @copydoc ValueType::GetExplicitCastingTargets()
    virtual const ValueTypeArray &GetExplicitCastingTargets() const
    {
      return this->explicitCastingTargets;
    }

    //! @copydoc ValueType::GetImplicitCastingOperator()
    virtual CastingOperator *GetImplicitCastingOperator(const ValueType *targetType, Expression *expr) const
    {
      throw EXCEPTION(InvalidOperationException, "Void type cannot be casted.");
    };

    //! @copydoc ValueType::GetExplicitCastingOperator()
    virtual CastingOperator *GetExplicitCastingOperator(const ValueType *targetType, Expression *expr) const
    {
      throw EXCEPTION(NotImplementedException, "Void type cannot be casted.");
    }
  };
}

#endif // __VoidType_h__

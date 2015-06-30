/**
* @file Scg/Types/PointerType.h
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __PointerType_h__
#define __PointerType_h__

// STL header files
#include <unordered_map>

// LLVM forward declarations
#include <llvm_fwd.h>

// Scg header files
#include <typedefs.h>
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <unordered_map>

namespace Scg
{
/**
* Represent a pointer type.
*/
class PointerType : public ValueType
{
  //! The name of the type, for example ptr[int].
  std::string name;
  //! The type of the content of this pointer.
  const ValueType *contentType;
  PointerValueTypeSpec typeSpec;
  //!
  std::unordered_map<ValueType *, PointerType *> definedPointerTypes;

private:
  // TODO: We should
  /**
  * Constructs a pointer type.
  * @param[in] pointeeType The type of the variable to point to.
  */
  PointerType(const ValueType *contentType);

protected:
  //! @copydoc ValueType::InitCastingTargets()
  virtual void InitCastingTargets() const override;

public:
  //! @copydoc ValueType::GetName()
  virtual const std::string GetName() const
  {
    return name;
  }

  //! @copydoc ValueType::GetDefaultLLVMValue()
  virtual llvm::Constant *GetDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  /**
  * Retrieves the type of the content of this pointer.
  * @return A pointer to the type of the content of this pointer.
  */
  virtual const ValueType *GetContentType() const
  {
    return contentType;
  }

  //! @copydoc ValueType::GetValueTypeSpec()
  virtual const ValueTypeSpec *GetValueTypeSpec() const override
  {
    return &typeSpec;
  }

  //! @copydoc ValueType::IsEqualTo()
  virtual bool IsEqualTo(const ValueType *other) const;

  //! @copydoc ValueType::CreateCastInst()
  virtual llvm::Value *CreateCastInst(llvm::IRBuilder<> *irb,
      llvm::Value *value, const ValueType *targetType) const override;

  //! @copydoc ValueType::GetImplicitCastingOperator()
  virtual CastingOperator *GetImplicitCastingOperator(const ValueType *targetType, Expression *expr) const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet for pointers.");
  }

  //! @copydoc ValueType::GetExplicitCastingOperator()
  virtual CastingOperator *GetExplicitCastingOperator(const ValueType *targetType, Expression *expr) const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet for pointers.");
  }

  //! Stores all pointer types used so far, so that we can reuse them.
  static std::unordered_map<const ValueType *, PointerType *> usedPointerTypes;

  /**
  * Retrieves a pointer type whose content type is the given value type.
  * @note This function is guaranteed to retrieve the same object for the same
  * content type so that we can easily compare types by comparing their
  * memory location.
  * @param[in] contentType The type of the content of this pointer type.
  * @return The pointer type.
  */
  static PointerType *Get(const ValueType *contentType);
};
}

#endif // __PointerType_h__

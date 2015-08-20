/**
* @file Scg/Types/CharType.h
*
* @copyright Copyright (C) 2014 Hicham OUALI ALAMI
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __CharType_h__
#define __CharType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
//! Represent the char type.
class CharType : public ValueType
{
  friend class LlvmContainer;

  static CharType *s_singleton;
  ValueTypeSpecByName typeSpec;

private:
  //! Constructs an char type.
  CharType();

  //! Class destructor.
  virtual ~CharType()
  {
  }

protected:
  //! @copydoc ValueType::InitCastingTargets()
  virtual void InitCastingTargets() const override;

public:
  //! @copydoc ValueType::GetName()
  virtual const std::string GetName() const
  {
    return "char";
  }

  /**
  * Get an LLVM constant value of integral type.
  * @param[in] value The value of the constant.
  * @return The llvm::Value object representing the constant.
  */
  llvm::Constant *GetLlvmConstant(char value) const;

  //! @copydoc ValueType::GetDefaultLLVMValue()
  virtual llvm::Constant *GetDefaultLLVMValue() const
  {
    return GetLlvmConstant(0);
  }

  //! @copydoc ValueType::GetValueTypeSpec()
  virtual const ValueTypeSpec *GetValueTypeSpec() const override
  {
    return &typeSpec;
  }

  //! @copydoc ValueType::IsEqualTo()
  virtual bool IsEqualTo(const ValueType *other) const
  {
    return dynamic_cast<const CharType *>(other) != nullptr;
  }

  //! @copydoc ValueType::CreateCastInst()
  virtual llvm::Value *CreateCastInst(llvm::IRBuilder<> *irb,
      llvm::Value *value, const ValueType *targetType) const override;

  static CharType *Get();
};
}

#endif // __CharType_h__

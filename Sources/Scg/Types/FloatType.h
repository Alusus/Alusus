/**
* @file Scg/Types/FloatType.h
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __FloatType_h__
#define __FloatType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
//! Represents the float type.
class FloatType : public ValueType
{
  friend class LlvmContainer;

  static FloatType *s_singleton;
  ValueTypeSpecByName typeSpec;

private:
  //! Constructs a float type.
  FloatType();

  //! Class destructor.
  virtual ~FloatType()
  {
  }

protected:
  //! @copydoc ValueType::initCastingTargets()
  virtual void initCastingTargets() const override;

public:
  /**
  * Gets a constant value of float type.
  * @param[in] value The value of the constant.
  */
  llvm::Constant *getLlvmConstant(float value)

  const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  virtual llvm::Constant *
  getDefaultLLVMValue(
  )
  const
  {
    return
      getLlvmConstant(0.0f);
  }

  //! @copydoc ValueType::getName()
  virtual
  const std::string
  getName()

  const
  {
    return "float";
  }

  //! @copydoc ValueType::getValueTypeSpec()
  virtual const ValueTypeSpec *
  getValueTypeSpec() const
  override
  {
    return &

           typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  virtual
  bool isEqualTo(const
                 ValueType *other) const
  {
    return dynamic_cast<const FloatType *>(other)
           !=

           nullptr;
  }

  //! @copydoc ValueType::createCastInst()
  virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                      llvm::Value *value, const ValueType *targetType) const override;

  static FloatType *get();
};
}

#endif // __FloatType_h__

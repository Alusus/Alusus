/**
* @file Scg/Types/DoubleType.h
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __DoubleType_h__
#define __DoubleType_h__

// Scg header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
//! Represents the double type.
class DoubleType : public ValueType
{
  friend class LlvmContainer;

  static DoubleType *s_singleton;
  ValueTypeSpecByName typeSpec;

private:
  //! Constructs a double type.
  DoubleType();

  //! Destructor.
  virtual ~DoubleType()
  {
  }

protected:
  //! @copydoc ValueType::initCastingTargets()
  virtual void initCastingTargets() const override;

public:
  /**
  * Gets a constant value of double type.
  * @param[in] value The value of the constant.
  */
  llvm::Constant *getLlvmConstant(double value) const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  virtual llvm::Constant *getDefaultLLVMValue() const
  {
    return getLlvmConstant(0.0);
  }

  //! @copydoc ValueType::getName()
  virtual const std::string getName() const
  {
    return "double";
  }

  //! @copydoc ValueType::getValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override
  {
    return &typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  virtual bool isEqualTo(const ValueType *other) const
  {
    return dynamic_cast<const DoubleType *>(other) != nullptr;
  }

  //! @copydoc ValueType::createCastInst()
  virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                      llvm::Value *value, const ValueType *targetType) const override;

  static DoubleType *get();
};
}

#endif // __DoubleType_h__

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

private:
  //! Constructs an integer type.
  IntegerType();

  //! Class destructor.
  virtual ~IntegerType()
  {
  }

protected:
  //! @copydoc ValueType::initCastingTargets()
  virtual void initCastingTargets() const override;

public:
  //! @copydoc ValueType::getName()
  virtual const std::string getName() const
  {
    return "int";
  }

  /**
  * Get an LLVM constant value of integral type.
  * @param[in] value The value of the constant.
  * @return The llvm::Value object representing the constant.
  */
  llvm::Constant *getLlvmConstant(int value) const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  virtual llvm::Constant *getDefaultLLVMValue() const
  {
    return getLlvmConstant(0);
  }

  //! @copydoc ValueType::getValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override
  {
    return &typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  virtual bool isEqualTo(const ValueType *other) const
  {
    return dynamic_cast<const IntegerType *>(other) != nullptr;
  }

  //! @copydoc ValueType::createCastInst()
  virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                      llvm::Value *value, const ValueType *targetType) const override;

  static IntegerType *get();
};
}

#endif // __IntegerType_h__

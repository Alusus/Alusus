/**
* @file Scg/Types/DoubleType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_DOUBLETYPE_H
#define SCG_DOUBLETYPE_H

#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>

namespace Scg
{

//! Represents the double type.
class DoubleType : public ValueType
{
  friend class LlvmContainer;

  //============================================================================
  // Type Info

  TYPE_INFO(DoubleType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  private: static DoubleType *s_singleton;
  private: SharedPtr<ValueTypeSpec> typeSpec;


  //============================================================================
  // Constructors & Destructor

  //! Constructs a double type.
  private: DoubleType();

  //! Destructor.
  private: virtual ~DoubleType()
  {
  }


  //============================================================================
  // Member Functions

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  /**
  * Gets a constant value of double type.
  * @param[in] value The value of the constant.
  */
  public: llvm::Constant *getLlvmConstant(double value) const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  public: virtual llvm::Constant *getDefaultLLVMValue() const
  {
    return getLlvmConstant(0.0);
  }

  //! @copydoc ValueType::getName()
  public: virtual std::string const& getName() const
  {
    static std::string name("double");
    return name;
  }

  //! @copydoc ValueType::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  public: virtual bool isEqualTo(ValueType const *other) const
  {
    return dynamic_cast<const DoubleType *>(other) != nullptr;
  }

  //! @copydoc ValueType::createCastInst()
  public: virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                              llvm::Value *value,
                                              const ValueType *targetType) const override;

  public: static DoubleType *get();

}; // class

} // namespace

#endif

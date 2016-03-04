/**
* @file Scg/Types/IntegerType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_INTEGERTYPE_H
#define SCG_INTEGERTYPE_H

#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>

namespace Scg
{

//! Represent the integer type.
class IntegerType : public ValueType
{
  friend class LlvmContainer;

  //============================================================================
  // Type Info

  TYPE_INFO(IntegerType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  private: static IntegerType *s_singleton;
  private: SharedPtr<ValueTypeSpec> typeSpec;


  //============================================================================
  // Construtors & Destructor

  //! Constructs an integer type.
  private: IntegerType();

  //! Class destructor.
  private: virtual ~IntegerType()
  {
  }


  //============================================================================
  // Member Functions

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  //! @copydoc ValueType::getName()
  public: virtual std::string const& getName() const
  {
    static std::string name("int");
    return name;
  }

  /**
   * Get an LLVM constant value of integral type.
   * @param[in] value The value of the constant.
   * @return The llvm::Value object representing the constant.
   */
  public: llvm::Constant* getLlvmConstant(int value) const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  public: virtual llvm::Constant* getDefaultLLVMValue() const
  {
    return getLlvmConstant(0);
  }

  //! @copydoc ValueType::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  public: virtual bool isEqualTo(const ValueType *other) const
  {
    return dynamic_cast<const IntegerType *>(other) != nullptr;
  }

  //! @copydoc ValueType::createCastInst()
  public: virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                              llvm::Value *value,
                                              const ValueType *targetType) const override;

  public: static IntegerType *get();

}; // class

} // namespace

#endif

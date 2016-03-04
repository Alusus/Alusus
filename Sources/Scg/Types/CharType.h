/**
* @file Scg/Types/CharType.h
*
* @copyright Copyright (C) 2016 Hicham OUALI ALAMI
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_CHARTYPE_H
#define SCG_CHARTYPE_H

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

  //============================================================================
  // Type Info

  TYPE_INFO(CharType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  private: static CharType *s_singleton;
  private: SharedPtr<ValueTypeSpec> typeSpec;


  //============================================================================
  // Constructors & Destructor

  //! Constructs an char type.
  private: CharType();

  //! Class destructor.
  private: virtual ~CharType()
  {
  }


  //============================================================================
  // Member Functions

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  //! @copydoc ValueType::getName()
  public: virtual std::string const& getName() const
  {
    static std::string name("char");
    return name;
  }

  /**
  * Get an LLVM constant value of integral type.
  * @param[in] value The value of the constant.
  * @return The llvm::Value object representing the constant.
  */
  public: llvm::Constant *getLlvmConstant(char value) const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  public: virtual llvm::Constant *getDefaultLLVMValue() const
  {
    return getLlvmConstant(0);
  }

  //! @copydoc ValueType::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  public: virtual bool isEqualTo(ValueType const *other) const
  {
    return dynamic_cast<const CharType *>(other) != nullptr;
  }

  //! @copydoc ValueType::createCastInst()
  public: virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                              llvm::Value *value,
                                              ValueType const *targetType) const override;

  public: static CharType *get();

}; // class

} // namespace

#endif

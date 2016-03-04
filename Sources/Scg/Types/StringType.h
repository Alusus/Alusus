/**
* @file Scg/Types/StringType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_STRINGTYPE_H
#define SCG_STRINGTYPE_H

#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>

namespace Scg
{

class Module;
class CodeGenUnit;

/**
* Represent a variable type.
*/
class StringType : public ValueType
{
  friend class LlvmContainer;

  //============================================================================
  // Type Info

  TYPE_INFO(StringType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  private: static StringType *s_singleton;
  private: SharedPtr<ValueTypeSpec> typeSpec;


  //============================================================================
  // Constructors & Destructor

  //! Constructs a string type.
  private: StringType();

  //! Class destructor.
  private: virtual ~StringType()
  {
  }


  //============================================================================
  // Member Functions

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  //! @copydoc ValueType::getName()
  public: virtual std::string const& getName() const
  {
    static std::string name("string");
    return name;
  }

  /**
  * Get an constant value of string type.
  * @param[in] value The value of the constant.
  * @param[in] value The value of the constant.
  */
  // TODO: Can we find some way to remove module?
  public: llvm::Constant* getLlvmConstant(CodeGenUnit *codeGenUnit, Module *module, const std::string &value) const;

  //! @copydoc ValueType::getDefaultLLVMValue()
  public: virtual llvm::Constant* getDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  //! @copydoc ValueType::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  public: virtual bool isEqualTo(const ValueType *other) const
  {
    return dynamic_cast<const StringType *>(other) != nullptr;
  }

  public: static StringType *get();

}; // class

} // namespace

#endif

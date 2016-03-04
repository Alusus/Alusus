/**
* @file Scg/Types/VoidType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_VOIDTYPE_H
#define SCG_VOIDTYPE_H

#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>

namespace Scg
{

/**
 * Represent a variable type.
 */
class VoidType : public ValueType
{
  friend class LlvmContainer;

  //============================================================================
  // Type Info

  TYPE_INFO(VoidType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  private: static VoidType *s_singleton;
  private: SharedPtr<ValueTypeSpec> typeSpec;


  //============================================================================
  // Constructors & Destructor

  private: VoidType();

  private: virtual ~VoidType()
  {
  }


  //============================================================================
  // Member Functions

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  //! @copydoc ValueType::getName()
  public: virtual std::string const& getName() const
  {
    static std::string name("void");
    return name;
  }

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
  public: virtual bool isEqualTo(ValueType const *other) const
  {
    // TODO: Should we throw InvalidOperationException here?
    return dynamic_cast<VoidType const*>(other) != nullptr;
  }

  public: static VoidType *get();

}; // class

} // namespace

#endif

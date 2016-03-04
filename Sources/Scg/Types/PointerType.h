/**
* @file Scg/Types/PointerType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_POINTERTYPE_H
#define SCG_POINTERTYPE_H

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
  //============================================================================
  // Type Info

  TYPE_INFO(PointerType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The name of the type, for example ptr[int].
  private: std::string name;
  //! The type of the content of this pointer.
  private: const ValueType *contentType;
  private: SharedPtr<ValueTypeSpec> typeSpec;

  //! Stores all pointer types used so far, so that we can reuse them.
  public: static std::unordered_map<ValueType const*, PointerType*> usedPointerTypes;


  //============================================================================
  // Constructors & Destructor

  // TODO: We should
  /**
  * Constructs a pointer type.
  * @param[in] pointeeType The type of the variable to point to.
  */
  private: PointerType(const ValueType *contentType);


  //============================================================================
  // Member Functions

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  //! @copydoc ValueType::getName()
  public: virtual std::string const& getName() const
  {
    return this->name;
  }

  //! @copydoc ValueType::getDefaultLLVMValue()
  public: virtual llvm::Constant* getDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  /**
  * Retrieves the type of the content of this pointer.
  * @return A pointer to the type of the content of this pointer.
  */
  public: virtual ValueType const* getContentType() const
  {
    return this->contentType;
  }

  //! @copydoc ValueType::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc ValueType::isEqualTo()
  public: virtual bool isEqualTo(const ValueType *other) const;

  //! @copydoc ValueType::createCastInst()
  public: virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                              llvm::Value *value,
                                              const ValueType *targetType) const override;

  /**
  * Retrieves a pointer type whose content type is the given value type.
  * @note This function is guaranteed to retrieve the same object for the same
  * content type so that we can easily compare types by comparing their
  * memory location.
  * @param[in] contentType The type of the content of this pointer type.
  * @return The pointer type.
  */
  public: static PointerType *get(const ValueType *contentType);

}; // class

} // namespace

#endif

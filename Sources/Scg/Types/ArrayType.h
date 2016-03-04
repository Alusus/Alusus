/**
* @file Scg/Types/ArrayType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_ARRAYTYPE_H
#define SCG_ARRAYTYPE_H

#include <unordered_map>
#include "core.h"
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>

namespace Scg
{

/**
* Represents a structure type with custom fields.
*/
class ArrayType : public ValueType
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArrayType, ValueType, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The name of the type, for example ary[int, 10].
  private: std::string name;
  private: SharedPtr<ValueTypeSpec> typeSpec;
  private: const ValueType *elementsType;
  private: unsigned int arraySize;

  //! Stores all array types used so far, so that we can reuse them.
  public: static std::unordered_map<ValueType const*, ArrayType*> usedArrayTypes;


  //============================================================================
  // Constructors & Destructor

  /**
  * Constructs a new array type.
  * @param[in] elementsType  The type of the elements of the array.
  * @param[in] arraySize     The number of elements in the array.
  */
  private: ArrayType(const ValueType *elementsType, unsigned int arraySize);


  //============================================================================
  // Member Functions

  //! @copydoc ValueType::initCastingTargets()
  protected: virtual void initCastingTargets() const override;

  //! @copydoc ValueType::getName()
  public: virtual std::string const& getName() const
  {
    return this->name;
  }

  /**
  * Retrieves the type of the elements of the array.
  * @return A pointer to the type of the elements of the array.
  */
  public: virtual ValueType const* getElementsType() const
  {
    return this->elementsType;
  }

  /**
  * Retrieves the number of the elements of the array.
  * @return The number of the elements of the array.
  */
  public: unsigned int getArraySize() const
  {
    return this->arraySize;
  }

  //! @copydoc ValueType::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc ValueType::getDefaultLLVMValue()
  public: virtual llvm::Constant* getDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  //! @copydoc ValueType::isEqualTo()
  public: virtual bool isEqualTo(const ValueType *other) const;

  /**
  * Retrieves an array type whose elements type is the given value type.
  * @note This function is guaranteed to retrieve the same object for the same
  * elements type and array size so that we can easily compare types by
  * comparing their memory location.
  * @param[in] elementsType The type of the elements of this array type.
  * @param[in] arraySize The number of elements in this array type.
  * @return The array type.
  */
  public: static ArrayType* get(const ValueType *elementsType, int arraySize);

}; // class

} // namespace

#endif

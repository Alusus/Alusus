/**
* @file Scg/Types/ArrayType.h
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef __ArrayType_h__
#define __ArrayType_h__

// STL header files
#include <unordered_map>

// SCG header files
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
/**
* Represents a structure type with custom fields.
*/
class ArrayType : public ValueType
{
  //! The name of the type, for example ary[int, 10].
  std::string name;
  ArrayValueTypeSpec typeSpec;
  const ValueType *elementsType;
  unsigned int arraySize;

private:
  /**
  * Constructs a new array type.
  * @param[in] elementsType  The type of the elements of the array.
  * @param[in] arraySize     The number of elements in the array.
  */
  ArrayType(const ValueType *elementsType, unsigned int arraySize);

protected:
  //! @copydoc ValueType::InitCastingTargets()
  virtual void InitCastingTargets() const override;

public:
  //! @copydoc ValueType::GetName()
  virtual const std::string GetName() const
  {
    return name;
  }

  /**
  * Retrieves the type of the elements of the array.
  * @return A pointer to the type of the elements of the array.
  */
  virtual const ValueType *GetElementsType() const
  {
    return elementsType;
  }

  /**
  * Retrieves the number of the elements of the array.
  * @return The number of the elements of the array.
  */
  unsigned int GetArraySize() const
  {
    return arraySize;
  }

  //! @copydoc ValueType::GetValueTypeSpec()
  virtual const ValueTypeSpec *GetValueTypeSpec() const override
  {
    return &typeSpec;
  }

  //! @copydoc ValueType::GetDefaultLLVMValue()
  virtual llvm::Constant *GetDefaultLLVMValue() const
  {
    throw EXCEPTION(NotImplementedException, "Not implemented yet!");
  }

  //! @copydoc ValueType::IsEqualTo()
  virtual bool IsEqualTo(const ValueType *other) const;

  //! Stores all array types used so far, so that we can reuse them.
  static std::unordered_map<const ValueType *, ArrayType *> usedArrayTypes;

  /**
  * Retrieves an array type whose elements type is the given value type.
  * @note This function is guaranteed to retrieve the same object for the same
  * elements type and array size so that we can easily compare types by
  * comparing their memory location.
  * @param[in] elementsType The type of the elements of this array type.
  * @param[in] arraySize The number of elements in this array type.
  * @return The array type.
  */
  static ArrayType *Get(const ValueType *elementsType, int arraySize);
};
}

#endif // __ArrayType_h__

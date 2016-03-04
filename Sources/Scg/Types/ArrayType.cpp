/**
* @file Scg/Types/ArrayType.cpp
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#include <prerequisites.h>

// LLVM header files
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/DerivedTypes.h>

// Scg header files
#include <Types/ArrayType.h>

// Boost header files
#include <boost/lexical_cast.hpp>

namespace Scg
{

//==============================================================================
// Static Variables

std::unordered_map<ValueType const*, ArrayType*> ArrayType::usedArrayTypes;


//==============================================================================
// Constructors & Destructor

// TODO: We are calling clone() because the passed type spec gets automatically
// freed. Is there a better way to avoid extra memory allocation and free?
ArrayType::ArrayType(const ValueType *elemsType, unsigned int size) :
  elementsType(elemsType),
  arraySize(size)
{
  if (elemsType == nullptr) {
    throw EXCEPTION(InvalidArgumentException, "elemsType argument cannot be nullptr.");
  }

  this->typeSpec = std::make_shared<ArrayValueTypeSpec>(elemsType->getValueTypeSpec()->clone(), size);

  this->name = elementsType->getName() +
               "[" + boost::lexical_cast<std::string>(arraySize)+"]";
  this->llvmType = llvm::ArrayType::get(
                     const_cast<llvm::Type *>(this->elementsType->getLlvmType()), this->arraySize);
}


//==============================================================================
// Member Functions

void ArrayType::initCastingTargets() const
{
  // TODO: Implement this.
}


bool ArrayType::isEqualTo(const ValueType *other) const
{
  auto otherAsArray = dynamic_cast<const ArrayType *>(other);

  if (otherAsArray == nullptr) {
    return false;
  }

  if (this->arraySize != otherAsArray->arraySize ||
      !this->elementsType->isEqualTo(otherAsArray->elementsType)) {
    return false;
  }

  return true;
}


ArrayType* ArrayType::get(const ValueType *elementsType, int arraySize)
{
  auto it = ArrayType::usedArrayTypes.find(elementsType);

  if (it != ArrayType::usedArrayTypes.end()) {
    return it->second;
  }

  // TODO: We need to create a map of all the so-far allocated value types to
  // avoid creating a value type twice.
  // TODO: Change ValueType& to ValueType* or change the constructor of
  // ArrayType.
  auto arrayType = new ArrayType(elementsType, arraySize);
  ArrayType::usedArrayTypes.insert(std::make_pair(elementsType, arrayType));
  return arrayType;
}

} // namespace

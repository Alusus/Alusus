/**
 * @file Scg/Types/ArrayType.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
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
#include <llvm/IR/Type.h>

// Scg header files
#include <Types/ArrayType.h>

namespace Scg
{
// TODO: We are calling Clone() because the passed type spec gets automatically
// freed. Is there a better way to avoid extra memory allocation and free?
ArrayType::ArrayType(ValueType *eType, unsigned int size) :
    typeSpec(const_cast<ValueTypeSpec*>(eType->GetValueTypeSpec()->Clone()), size),
    elementsType(eType),
    arraySize(size)
{
  this->name = elementsType->GetName() +
      "[" + boost::lexical_cast<std::string>(arraySize) + "]";
  this->llvmType = llvm::ArrayType::get(this->elementsType->GetLlvmType(), this->arraySize);
}

//------------------------------------------------------------------------------

bool ArrayType::IsEqualTo(const ValueType *other) const
{
  auto otherAsArray = dynamic_cast<const ArrayType*>(other);
  if (otherAsArray == nullptr) {
    return false;
  }
  if (this->arraySize != otherAsArray->arraySize ||
      !this->elementsType->IsEqualTo(otherAsArray->elementsType)) {
    return false;
  }
  return true;
}
}

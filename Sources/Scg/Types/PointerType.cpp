/**
* @file Scg/Types/PointerType.cpp
*
* @copyright Copyright (C) 2014 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

// Class not needed at the moment.

#include <prerequisites.h>

// Scg header files
#include "Types/PointerType.h"
#include "Types/IntegerType.h"
#include "LlvmContainer.h"

// LLVM header files

namespace Scg
{
std::unordered_map<const ValueType *, PointerType *> PointerType::usedPointerTypes;

// TODO: We are calling clone() because the passed type spec gets automatically
// freed. Is there a better way to avoid extra memory allocation and free?
PointerType::PointerType(const ValueType *cntType) :
  contentType(cntType),
  typeSpec(const_cast<ValueTypeSpec *>(cntType->getValueTypeSpec()->clone()))
{
  this->llvmType = const_cast<llvm::Type *>(this->contentType->getLlvmType())->getPointerTo(0);
}

void PointerType::initCastingTargets() const
{
  this->implicitCastingTargets.push_back(this);

  this->explicitCastingTargets.push_back(this);
}

bool PointerType::isEqualTo(const ValueType *other) const
{
  auto otherAsPointer = dynamic_cast<const PointerType *>(other);

  if (otherAsPointer == nullptr) {
    return false;
  }

  // TODO: Why is PointerType::contentType reference while
  // ArrayType::elementsType is pointer?
  if (!this->contentType->isEqualTo(otherAsPointer->contentType)) {
    return false;
  }

  return true;
}


PointerType *PointerType::get(const ValueType *contentType)
{
  auto it = PointerType::usedPointerTypes.find(contentType);

  if (it != PointerType::usedPointerTypes.end()) {
    return it->second;
  }

  // TODO: We need to create a map of all the so-far allocated value types to
  // avoid creating a value type twice.
  auto type = new PointerType(contentType);
  PointerType::usedPointerTypes.insert(std::make_pair(contentType, type));
  return type;
}

llvm::Value *PointerType::createCastInst(llvm::IRBuilder<> *irb,
    llvm::Value *value, const ValueType *targetType) const
{
  if (dynamic_cast<PointerType const*>(targetType) != nullptr) {
    // Casting to another pointer type.
    return irb->CreateBitCast(value, const_cast<llvm::Type*>(targetType->getLlvmType()));
  } else if (dynamic_cast<IntegerType const*>(targetType) != nullptr) {
    Int thisBitCount = LlvmContainer::getDataLayout()->getPointerSizeInBits();
    Int targetBitCount = LlvmContainer::getDataLayout()->getTypeSizeInBits(const_cast<llvm::Type*>
                         (targetType->getLlvmType()));

    if (targetBitCount == thisBitCount) {
      return irb->CreateBitCast(value, const_cast<llvm::Type*>(targetType->getLlvmType()));
    }
  }

  // TODO: Improve the message.
  throw EXCEPTION(InvalidCastException, "Invalid cast.");
}

}

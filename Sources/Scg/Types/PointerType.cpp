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
#include <Types/PointerType.h>

// LLVM header files

namespace Scg
{
// TODO: We are calling Clone() because the passed type spec gets automatically
// freed. Is there a better way to avoid extra memory allocation and free?
PointerType::PointerType(const ValueType &cntType) :
    contentType(cntType),
    typeSpec(const_cast<ValueTypeSpec*>(cntType.GetValueTypeSpec()->Clone()))
{
  this->llvmType = const_cast<llvm::Type*>(this->contentType.GetLlvmType())->getPointerTo(0);
}

//------------------------------------------------------------------------------

bool PointerType::IsEqualTo(const ValueType *other) const
{
  auto otherAsPointer = dynamic_cast<const PointerType*>(other);
  if (otherAsPointer == nullptr) {
    return false;
  }
  // TODO: Why is PointerType::contentType reference while
  // ArrayType::elementsType is pointer?
  if (!this->contentType.IsEqualTo(&otherAsPointer->contentType)) {
    return false;
  }
  return true;
}
}

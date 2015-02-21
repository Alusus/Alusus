/**
 * @file Scg/Types/StructType.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg header files
#include <Types/StructType.h>

// LLVM header files

namespace Scg
{
StructType::StructType(const std::string &name)
  : name(name), typeSpec("string")
{
  // We don't delete a structure type we create as it is defined using
  // LLVM's BumpPtrAllocator.
  this->llvmType = llvm::StructType::create(LlvmContainer::GetContext(), this->name.c_str());
}

//------------------------------------------------------------------------------

void StructType::SetFields(const ValueTypeNameArray &fields)
{
  this->fields = fields;
  std::vector<llvm::Type*> llvmFields;
  for (auto field : this->fields)
    llvmFields.push_back(field.GetValueType()->GetLlvmType());
  static_cast<llvm::StructType*>(this->llvmType)->setBody(llvmFields);
}

//------------------------------------------------------------------------------

bool StructType::IsEqualTo(const ValueType *other) const
{
  auto otherAsStruct = dynamic_cast<const StructType*>(other);
  if (otherAsStruct == nullptr) {
    return false;
  }
  // TODO: For now we are just comparing the name. Later
  return otherAsStruct->GetName().compare(otherAsStruct->GetName()) == 0;
}
}

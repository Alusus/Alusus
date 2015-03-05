/**
 * @file Scg/Types/ValueTypeSpec.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg files
#include <Containers/Module.h>
#include <Memory/AutoDeleteAllocator.h>
#include <Types/ArrayType.h>
#include <Types/PointerType.h>

// STL header files

namespace Scg
{
TypeComparisonResult ValueTypeSpec::Compare(const Module &module,
    const ValueTypeSpec *other) const
{
  auto thisType = this->ToValueType(module);
  auto otherType = other->ToValueType(module);
  return thisType->Compare(otherType);
}

//------------------------------------------------------------------------------

void *ValueTypeSpec::operator new (size_t size)
{
  return AutoDeleteAllocator::GetSingleton().Allocate(size);
}

//------------------------------------------------------------------------------

void ValueTypeSpec::operator delete (void *ptr)
{
  AutoDeleteAllocator::GetSingleton().Free(ptr);
}

//===== ValueTypeSpecByName ====================================================

TypeComparisonResult ValueTypeSpecByName::Compare(const Module &module, const ValueTypeSpec *other) const
{
  auto thisType = this->ToValueType(module);
  auto otherType = other->ToValueType(module);
  if (thisType == otherType) {
    return TypeComparisonResult::Equivalent;
  } else if (thisType->IsImplicitlyCastableTo(otherType)) {
    return TypeComparisonResult::ImplicitlyEquivalent;
  } else {
    return TypeComparisonResult::NotEquivalent;
  }
}

//------------------------------------------------------------------------------

ValueType *ValueTypeSpecByName::ToValueType(const Module &module) const
{
  return module.GetValueTypeByName(this->name);
}

//------------------------------------------------------------------------------

std::string ValueTypeSpecByName::ToString() const
{
  return this->name;
}

//===== PointerValueTypeSpec ===================================================

PointerValueTypeSpec::~PointerValueTypeSpec()
{
  delete this->contentTypeSpec;
}

//------------------------------------------------------------------------------

ValueType *PointerValueTypeSpec::ToValueType(const Module &module) const
{
  auto contentType = this->contentTypeSpec->ToValueType(module);
  if (contentType == nullptr)
    return nullptr;
  return PointerType::Get(contentType);
}

//------------------------------------------------------------------------------

std::string PointerValueTypeSpec::ToString() const
{
  std::stringstream ss;
  ss << "ptr[";
  ss << this->contentTypeSpec->ToString();
  ss << "]";
  return ss.str();
}

//===== ArrayValueTypeSpec =====================================================

ArrayValueTypeSpec::~ArrayValueTypeSpec()
{
  delete this->elementsTypeSpec;
}

//------------------------------------------------------------------------------

ValueType *ArrayValueTypeSpec::ToValueType(const Module &module) const
{
  auto elementsType = this->elementsTypeSpec->ToValueType(module);
  if (elementsType == nullptr)
    return nullptr;
  return ArrayType::Get(elementsType, this->arraySize);
}

//------------------------------------------------------------------------------

std::string ArrayValueTypeSpec::ToString() const
{
  std::stringstream ss;
  ss << "arr[";
  ss << this->elementsTypeSpec->ToString();
  ss << ", ";
  ss << this->arraySize;
  ss << "]";
  return ss.str();
}

//===== ValueTypeSpecArray =====================================================

bool ValueTypeSpecArray::IsEqualTo(const ValueTypeSpecArray *other,
    int sizeLimit) const
{
  if (sizeLimit == 0) {
    if (this->size() != other->size()) {
      return false;
    }
    sizeLimit = this->size();
  } else {
    if (this->size() < sizeLimit || other->size() < sizeLimit) {
      return false;
    }
  }

  for (auto i = sizeLimit - 1; i >= 0; i--) {
    if (!this->at(i)->IsEqualTo(other->at(i))) {
      return false;
    }
  }
  return true;
}

//===== VariableDefinitionArray ================================================

bool VariableDefinitionArray::AreTypesEqualTo(const ValueTypeSpecArray *other,
    int sizeLimit) const
{
  if (sizeLimit == 0) {
    if (this->size() != other->size()) {
      return false;
    }
    sizeLimit = this->size();
  } else {
    if (this->size() < sizeLimit || other->size() < sizeLimit) {
      return false;
    }
  }

  for (auto i = sizeLimit - 1; i >= 0; i--) {
    if (!this->at(i).GetTypeSpec()->IsEqualTo(other->at(i))) {
      return false;
    }
  }
  return true;
}
}

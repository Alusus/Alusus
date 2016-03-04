/**
* @file Scg/Types/ValueTypeSpec.cpp
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#include <prerequisites.h>

// Scg files
#include <Containers/Module.h>
#include <Types/ArrayType.h>
#include <Types/PointerType.h>

// STL header files

namespace Scg
{

//============================================================================
// ValueTypeSpec

TypeComparisonResult ValueTypeSpec::compare(Module const &module,
    ValueTypeSpec const *other) const
{
  auto thisType = this->toValueType(module);
  auto otherType = other->toValueType(module);
  return thisType->compare(otherType);
}


//============================================================================
// ValueTypeSpecByName

TypeComparisonResult ValueTypeSpecByName::compare(const Module &module, const ValueTypeSpec *other) const
{
  auto thisType = this->toValueType(module);
  auto otherType = other->toValueType(module);

  if (thisType == otherType) {
    return TypeComparisonResult::Equivalent;
  } else if (thisType->isImplicitlyCastableTo(otherType)) {
    return TypeComparisonResult::ImplicitlyEquivalent;
  } else {
    return TypeComparisonResult::NotEquivalent;
  }
}


ValueType* ValueTypeSpecByName::toValueType(const Module &module) const
{
  return module.getValueTypeByName(this->name);
}


std::string ValueTypeSpecByName::toString() const
{
  return this->name;
}


//============================================================================
// PointerValueTypeSpec

ValueType *PointerValueTypeSpec::toValueType(const Module &module) const
{
  auto contentType = this->contentTypeSpec->toValueType(module);

  if (contentType == nullptr)
    return nullptr;

  return PointerType::get(contentType);
}


std::string PointerValueTypeSpec::toString() const
{
  std::stringstream ss;
  ss << "ptr[";
  ss << this->contentTypeSpec->toString();
  ss << "]";
  return ss.str();
}


//============================================================================
// ArrayValueTypeSpec

ValueType *ArrayValueTypeSpec::toValueType(const Module &module) const
{
  auto elementsType = this->elementsTypeSpec->toValueType(module);

  if (elementsType == nullptr)
    return nullptr;

  return ArrayType::get(elementsType, this->arraySize);
}


std::string ArrayValueTypeSpec::toString() const
{
  std::stringstream ss;
  ss << "ary[";
  ss << this->elementsTypeSpec->toString();
  ss << ", ";
  ss << this->arraySize;
  ss << "]";
  return ss.str();
}


//============================================================================
// ValueTypeSpecArray

bool ValueTypeSpecArray::isEqualTo(const ValueTypeSpecArray *other,
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
    if (!this->at(i)->isEqualTo(other->at(i).get())) {
      return false;
    }
  }

  return true;
}


//============================================================================
// VariableDefinitionArray

bool VariableDefinitionArray::areTypesEqualTo(const ValueTypeSpecArray *other,
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
    if (!this->at(i).getTypeSpec()->isEqualTo(other->at(i).get())) {
      return false;
    }
  }

  return true;
}

} // namespace

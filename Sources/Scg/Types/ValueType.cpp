/**
* @file Scg/Types/ValueType.cpp
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
#include <llvm/IR/DerivedTypes.h>

// Scg files
#include <LlvmContainer.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>
#include <Types/CharType.h>
#include <Types/StringType.h>
#include <Types/PointerType.h>
#include <Types/VoidType.h>
#include <Values/Variable.h>

namespace Scg
{

ValueType* ValueType::getPrimitiveType(std::string const &typeName)
{
  if (typeName == "int") {
    return IntegerType::get();
  } else if (typeName == "char") {
    return CharType::get();
  } else if (typeName == "float") {
    return FloatType::get();
  } else if (typeName == "double") {
    return DoubleType::get();
  } else if (typeName == "string") {
    return StringType::get();
  } else if (typeName == "ptr") {
    return PointerType::get(CharType::get());
  } else if (typeName == "void" || typeName == "") {
    return VoidType::get();
  }

  return nullptr;
}


SharedPtr<Variable> ValueType::newVariable(std::string const &name,
                                           llvm::Argument *llvmArgument) const
{
  this->varCount++;
  return std::make_shared<Variable>(name, const_cast<ValueType *>(this), llvmArgument);
}


void ValueType::releaseVariable(Variable *var) const
{
  this->varCount--;
}


Int ValueType::getAllocationSize() const
{
  return LlvmContainer::getDataLayout()->getTypeAllocSize(this->getLlvmType());
}

} // namespace

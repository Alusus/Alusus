/**
* @file Scg/Types/ValueType.cpp
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
#include <llvm/IR/DerivedTypes.h>

// Scg files
#include <LlvmContainer.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>
#include <Types/StringType.h>
#include <Types/VoidType.h>
#include <Values/Variable.h>
#include <Memory/AutoDeleteAllocator.h>

namespace Scg
{
ValueType *ValueType::GetPrimitiveType(const std::string &typeName)
{
  if (typeName == "int") {
    return IntegerType::Get();
  } else if (typeName == "float") {
    return FloatType::Get();
  } else if (typeName == "double") {
    return DoubleType::Get();
  } else if (typeName == "string") {
    return StringType::Get();
  } else if (typeName == "void" || typeName == "") {
    return VoidType::Get();
  }
  return nullptr;
}

//----------------------------------------------------------------------------

Variable *ValueType::NewVariable(const std::string &name,
    llvm::Argument *llvmArgument) const
{
  this->varCount++;
  return new Variable(name, const_cast<ValueType *>(this), llvmArgument);
}

//----------------------------------------------------------------------------

void ValueType::DeleteVariable(Variable *var) const
{
  this->varCount--;
  delete var;
}

//------------------------------------------------------------------------------

void *ValueType::operator new(size_t size)
{
  return AutoDeleteAllocator::GetSingleton().Allocate(size);
}

//------------------------------------------------------------------------------

void ValueType::operator delete(void *ptr)
{
  AutoDeleteAllocator::GetSingleton().Free(ptr);
}
}

/**
 * @file Scg/BuiltInFunctions/AddFloats.cpp
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// LLVM header files
#include <llvm/IR/IRBuilder.h>

// SCG files
#include <BuiltInFunctions/AddFloats.h>
#include <Types/FloatType.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{
AddFloats::AddFloats()
{
  this->argTypeSpecs.push_back(new ValueTypeSpecByName("float"));
  this->argTypeSpecs.push_back(new ValueTypeSpecByName("float"));
}

AddFloats::~AddFloats()
{
  delete this->argTypeSpecs[0];
  delete this->argTypeSpecs[1];
  this->argTypeSpecs.clear();
}

llvm::Value *AddFloats::CreateLLVMInstruction(llvm::IRBuilder<> *irb,
		const std::vector<llvm::Value*> &args) const
{
	if (args.size() != 2)
		// TODO: The exception message shouldn't use AddFloats as this is the class name.
		// Instead, it should use a user friendly name, e.g. operator +.
		// TODO: Should we use CompilationErrorException? Should we derive from that an
		// exception specific for the invalid number of arguments.
		throw EXCEPTION(CompilationErrorException,
				"AddFloats built-in function requires two arguments.");

  return irb->CreateFAdd(args[0], args[1]);
}

//----------------------------------------------------------------------------------------

const ValueType *AddFloats::GetArgumentType(int n) const
{
	if (n < 0 || n > 1)
		throw EXCEPTION(ArgumentOutOfRangeException,
				"AddFloats built-in function accepts only two arguments.");
  return FloatType::GetSingleton();
}

//----------------------------------------------------------------------------------------

const ValueTypeSpec *AddFloats::GetValueTypeSpec() const
{
	return FloatType::GetSingleton()->GetValueTypeSpec();
}
}


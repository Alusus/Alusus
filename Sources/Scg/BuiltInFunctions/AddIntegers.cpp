/**
 * @file Scg/BuiltInFunctions/AddIntegers.cpp
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
#include <BuiltInFunctions/AddIntegers.h>
#include <Containers/Block.h>
#include <Types/IntegerType.h>

namespace Scg
{
llvm::Value *AddIntegers::CreateLLVMInstruction(llvm::IRBuilder<> *irb,
		const List &args) const
{
	if (args.GetElementCount() != 2)
		// TODO: The exception message shouldn't use AddIntegers as this is the class name.
		// Instead, it should use a user friendly name, e.g. operator +.
		// TODO: Should we use CompilationErrorException? Should we derive from that an
		// exception specific for the invalid number of arguments.
		THROW_EXCEPTION(CompilationErrorException,
				"AddIntegers built-in function requires two arguments.");
	auto arg1 = args.GetElement(0);
	auto arg2 = args.GetElement(1);
	if (arg1->GetValueType() != IntegerType::GetSingleton() ||
			arg1->GetValueType() != IntegerType::GetSingleton())
		THROW_EXCEPTION(CompilationErrorException,
				"AddIntegers built-in function requires arguments of integer type.");

  return irb->CreateAdd(arg1->GetGeneratedLlvmValue(), arg2->GetGeneratedLlvmValue());
}

//----------------------------------------------------------------------------------------

const ValueType *AddIntegers::GetArgumentType(int n) const
{
	if (n < 0 || n > 1)
		THROW_EXCEPTION(ArgumentOutOfRangeException,
				"AddIntegers built-in function accepts only two arguments.");
  return IntegerType::GetSingleton();
}
}


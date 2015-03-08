/**
 * @file Scg/BuiltInFunctions/BasicUnaryBuiltInFunction.cpp
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
#include <BuiltInFunctions/BasicUnaryBuiltInFunction.h>
#include <Types/DoubleType.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{
BasicUnaryBuiltInFunction::BasicUnaryBuiltInFunction(const Char *name, const Char *retType,
      const Char *argType, BasicUnaryBuiltInFunction::Callback function)
: name(name)
, function(function)
{
  this->argTypeSpecs.push_back(new ValueTypeSpecByName(argType));
  this->retTypeSpec = new ValueTypeSpecByName(retType);
}

BasicUnaryBuiltInFunction::~BasicUnaryBuiltInFunction()
{
  delete this->argTypeSpecs[0];
  delete this->argTypeSpecs[1];
  delete this->retTypeSpec;
  this->argTypeSpecs.clear();
}

llvm::Value *BasicUnaryBuiltInFunction::CreateLLVMInstruction(llvm::IRBuilder<> *irb,
    const std::vector<llvm::Value*> &args) const
{
	if (args.size() != 1)
		// TODO: The exception message shouldn't use BasicUnaryBuiltInFunction as this is the class name.
		// Instead, it should use a user friendly name, e.g. operator +.
		// TODO: Should we use CompilationErrorException? Should we derive from that an
		// exception specific for the invalid number of arguments.
		throw EXCEPTION(CompilationErrorException,
				"BasicUnaryBuiltInFunction built-in function requires one arguments.");
  return this->function(irb, args[0]);
}

const ValueTypeSpec *BasicUnaryBuiltInFunction::GetValueTypeSpec() const
{
	return this->retTypeSpec;
}
}


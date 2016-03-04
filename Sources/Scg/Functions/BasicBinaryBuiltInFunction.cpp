/**
 * @file Scg/Functions/BasicBuiltInFunction.cpp
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
#include <llvm/IR/IRBuilder.h>

// SCG files
#include <Functions/BasicBinaryBuiltInFunction.h>

namespace Scg
{

using namespace Core::Basic;

BasicBinaryBuiltInFunction::BasicBinaryBuiltInFunction(const Char *name, const Char *retType,
  const Char *arg1Type, const Char *arg2Type, BasicBinaryBuiltInFunction::Callback function)
  : name(name)
  , function(function)
{
  this->argTypeSpecs.push_back(std::make_shared<ValueTypeSpecByName>(arg1Type));
  this->argTypeSpecs.push_back(std::make_shared<ValueTypeSpecByName>(arg1Type));
  this->retTypeSpec = std::make_shared<ValueTypeSpecByName>(retType);
}


llvm::Value *BasicBinaryBuiltInFunction::createLLVMInstruction(llvm::IRBuilder<> *irb,
  std::vector<llvm::Value*> const &args) const
{
  if (args.size() != 2)
    // TODO: The exception message shouldn't use BasicBinaryBuiltInFunction as this is the class name.
    // Instead, it should use a user friendly name, e.g. operator +.
    // TODO: Should we use CompilationErrorException? Should we derive from that an
    // exception specific for the invalid number of arguments.
    throw EXCEPTION(CompilationErrorException,
                    "BasicBinaryBuiltInFunction built-in function requires two arguments.");

  return this->function(irb, args[0], args[1]);
}

} // namespace

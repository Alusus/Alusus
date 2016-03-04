/**
 * @file Scg/Instructions/CallFunction.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// STL header files

// LLVM header files
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// Scg header files
#include <Containers/Block.h>
#include <Containers/List.h>
#include <Containers/Module.h>
#include <CodeGenUnit.h>
#include <Functions/UserDefinedFunction.h>
#include <Instructions/CallFunction.h>
#include <Instructions/DefineFunction.h>
#include <Types/VoidType.h>

using namespace llvm;

namespace Scg
{

SharedPtr<ValueTypeSpec> const& CallFunction::getValueTypeSpec() const
{
  if (this->function == nullptr) {
    throw EXCEPTION(InvalidOperationException, "This function should only "
                    "be called after the pre-code generation step has finished, because "
                    "we don't have a reference to the function before that.");
  }

  SharedPtr<ValueTypeSpec> const &funcRetType = this->function->getValueTypeSpec();

  if (funcRetType == nullptr) {
    return VoidType::get()->getValueTypeSpec();
  }

  return funcRetType;
}


AstNode::CodeGenerationStage CallFunction::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  // Ensures that all children have finished pre-code generation step so that
  // we can grab their types.
  for (auto i = 0; i < this->args->getCount(); i++) {
    auto stage = static_cast<AstNode*>(this->args->get(i))->getCodeGenerationStage();

    if (stage < CodeGenerationStage::CodeGeneration
        || stage == CodeGenerationStage::None) {
      return CodeGenerationStage::PreCodeGeneration;
    }
  }

  // Finds the types of the arguments.
  if (argTypes.size() != this->args->getCount()) {
    argTypes.resize(this->args->getCount());

    for (auto i = 0; i < this->args->getCount(); i++) {
      argTypes[i] = static_cast<AstNode*>(this->args->get(i))->getValueTypeSpec();
    }
  }

  // Try to find the function in the same module.
  Module *module = this->findOwner<Module>();
  this->function = module->matchFunction(this->funcName, argTypes);

  if (this->function == nullptr) {
    // We couldn't find it in the current module, search the whole unit.
    auto matches = codeGenUnit->matchFunction(this->funcName, argTypes);

    switch (matches.size()) {
    case 0:
      // We couldn't find the function in the whole program, but it might
      // not have been generated yet.
      if (module->hasFunction(this->funcName, argTypes) ||
          codeGenUnit->hasFunction(this->funcName, argTypes)) {
        // Yes, wait until it is generated.
        return CodeGenerationStage::PreCodeGeneration;
      } else {
        // No match, throw a CompilationErrorException.
        throw EXCEPTION(CompilationErrorException,
                        ("Calling undefined function " + this->funcName).c_str());
      }
      break;

    case 1:
      // Found the function, use it.
      this->function = matches[0];
      break;

    default:
      // Found more than one match, throw a CompilationErrorException.
      // TODO: Improve the exception so that it shows the candidate functions.
      throw EXCEPTION(CompilationErrorException,
                      ("Found multiple matches for " + this->funcName).c_str());
    }
  }

  return CodeGenerationStage::CodeGeneration;
}


AstNode::CodeGenerationStage CallFunction::generateCode(CodeGenUnit *codeGenUnit)
{
  MODULE_CHECK;
  BLOCK_CHECK;

  // Generate code for the arguments.
  std::vector<llvm::Value*> args;

  // If argument mismatch error.
  // TODO: Check the types of arguments as well.
  if ((this->function->isVarArgs() &&
       this->function->getArgumentTypeSpecs().size() > getArguments()->getCount()) ||
      (!this->function->isVarArgs() &&
       this->function->getArgumentTypeSpecs().size() != getArguments()->getCount())) {
    std::stringstream str;
    str << "Function " << this->funcName << " expects "
        << this->function->getArgumentTypeSpecs().size() << " arguments, but got "
        << getArguments()->getCount();
    throw EXCEPTION(ArgumentMismatchException, str.str().c_str());
  }

  auto block = this->findOwner<Block>();

  for (size_t i = 0, e = getArguments()->getCount(); i != e; ++i) {
    auto module = this->findOwner<Module>();

    auto expectedArgType = i < this->function->getArgumentTypeSpecs().size() ?
                           this->function->getArgumentTypeSpecs()[i]->toValueType(*module) :
                           nullptr;
    auto argType = static_cast<AstNode*>(this->args->get(i))->getValueTypeSpec()->toValueType(*module);
    auto argValue = static_cast<AstNode*>(this->args->get(i))->getGeneratedLlvmValue();

    if (argValue == nullptr)
      throw EXCEPTION(EvaluationException,
                      ("Expression doesn't evaluate to a value: "
                       + static_cast<AstNode*>(getArguments()->get(i))->toString()).c_str());

    if (expectedArgType != nullptr &&
        argType->compare(expectedArgType) != TypeComparisonResult::Equivalent) {
      // Need to cast the type before sending it to the function.
      args.push_back(argType->createCastInst(block->getIRBuilder(),
                                             argValue, expectedArgType));
    } else {
      args.push_back(argValue);
    }
  }

  // Generate code for calling the function.
  this->generatedLlvmValue = this->callInst =
                               this->function->createLLVMInstruction(block->getIRBuilder(), args);

  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage CallFunction::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  this->function = nullptr;
  // TODO: Do something about deleting callInst. I commented it out because
  // its type was converted to llvm::Value object following the change of
  // the code generation to call Function::createLLVMInstruction which
  // returns a generic llvm::Value object.
  //SAFE_DELETE_LLVM_INST(this->callInst);
  return CodeGenerationStage::None;
}


std::string CallFunction::toString()
{
  return this->funcName + getArguments()->toString();
}

} // namespace

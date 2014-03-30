/**
 * @file Scg/Instructions/CallFunction.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// STL header files
#include <sstream>

// LLVM header files
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// Scg header files
#include <Containers/Block.h>
#include <Containers/List.h>
#include <Containers/Module.h>
#include <Containers/Program.h>
#include <Instructions/CallFunction.h>
#include <Instructions/DeclareExtFunction.h>
#include <Values/Function.h>

using namespace llvm;

namespace Scg
{
const ValueType *CallFunction::GetValueType() const
{
  if (this->function == nullptr) {
    THROW_EXCEPTION(InvalidOperationException, "This function should only "
        "be called after the pre-code generation step has finished, because "
        "we don't have a reference to the function before that.");
  }
  auto funcRetType = this->function->GetReturnType();
  if (funcRetType == nullptr) {
    return GetModule()->GetValueTypeByName("void");
  }
  return funcRetType->ToValueType(*GetModule());
}

//------------------------------------------------------------------------------

void CallFunction::DeclareRequiredFunction()
{
  auto program = this->module->GetProgram();
  if (program == nullptr) {
    THROW_EXCEPTION(UndefinedFunctionException,
        "Calling undefined function: " + this->funcName);
  }
  auto defFuncMatches = program->FindDefineFunction(this->funcName, this->argTypes);
  if (defFuncMatches.size() > 1) {
    THROW_EXCEPTION(UndefinedFunctionException,
        "Found multiple matches of function with name: " + this->funcName);
  } else if (defFuncMatches.size() == 1) {
    // Found a match. Create a DeclareExtFunction instruction in this module
    // to link to it.
    auto declFunc = new DeclareExtFunction(defFuncMatches[0]);
    this->module->PrependComplementaryExpression(declFunc);
    this->module->GetAutoDeclFuncSet().insert(this->funcName);
  } else {
    // Couldn't find a function defined in other modules. Try to find functions
    // declared in other modules, i.e. C functions.
    auto declFuncMatches = program->FindDeclareFunction(this->funcName, this->argTypes);
    if (declFuncMatches.size() > 1) {
      THROW_EXCEPTION(UndefinedFunctionException,
          "Found multiple matches of function with name: " + this->funcName);
    } else if (declFuncMatches.size() == 1) {
      // Found a match. Create a DeclareExtFunction instruction in this module
      // to link to it.
      auto declFunc = new DeclareExtFunction(declFuncMatches[0]);
      this->module->PrependComplementaryExpression(declFunc);
      this->module->GetAutoDeclFuncSet().insert(this->funcName);
    } else {
      THROW_EXCEPTION(UndefinedFunctionException,
          "Calling undefined function: " + this->funcName);
    }
  }

}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage CallFunction::PreGenerateCode()
{
  if (GetModule()->GetAutoDeclFuncSet().find(this->funcName) !=
      GetModule()->GetAutoDeclFuncSet().end()) {
    // A link to the function is already being added by another CallFunction
    // instruction. Wait till it is generated.
    return CodeGenerationStage::PreCodeGeneration;
  }

  // Ensures that all children have finished pre-code generation step so that
  // we can grab their types.
  for (auto i = 0; i < this->args->GetElementCount(); i++) {
    auto stage = this->args->GetElement(i)->GetCodeGenerationStage();
    if (stage < CodeGenerationStage::CodeGeneration
        || stage == CodeGenerationStage::None) {
      return CodeGenerationStage::PreCodeGeneration;
    }
  }

  // Finds the types of the arguments.
  argTypes.resize(this->args->GetElementCount());
  for (auto i = 0; i < this->args->GetElementCount(); i++) {
    argTypes[i] =
        const_cast<ValueTypeSpec*>(this->args->GetElement(i)->GetValueType()->GetValueTypeSpec());
  }

  this->function = GetModule()->GetFunction(this->funcName, argTypes);
  if (this->function == nullptr) {
    // The function is not available in this module, so we assume it is defined
    // in another module and automatically declare it in this module.
    DeclareRequiredFunction();
    // Asks the module to stay in pre-code generation stage until the newly
    // added declaration is processed by the module.
    return CodeGenerationStage::PreCodeGeneration;
  }
  return CodeGenerationStage::CodeGeneration;
}

//------------------------------------------------------------------------------

CodeGenerationResult CallFunction::GenerateCode()
{
  MODULE_CHECK;
  BLOCK_CHECK;

  // Generate code for the arguments.
  std::vector<llvm::Value*> args;

  // If argument mismatch error.
  // TODO: Check the types of arguments as well.
  if ((this->function->IsVarArgs() &&
          this->function->GetArgumentCount() > GetArguments()->GetElementCount()) ||
      (!this->function->IsVarArgs() &&
          this->function->GetArgumentCount() != GetArguments()->GetElementCount()))
  {
    std::stringstream str;
    str << "Function " << this->funcName << " expects "
        << this->function->GetArgumentCount() << " arguments, but got "
        << GetArguments()->GetElementCount();
    THROW_EXCEPTION(ArgumentMismatchException, str.str());
  }

  for (size_t i = 0, e = GetArguments()->GetElementCount(); i != e; ++i)
  {
    auto arg = GetArguments()->GetElement(i)->GenerateCode().exprValue;
    if (arg == 0)
      THROW_EXCEPTION(EvaluationException,
          "Expression doesn't evaluate to a value: "
              + GetArguments()->GetElement(i)->ToString());
    args.push_back(arg);
  }

  // Generate code for calling the function.
  return CodeGenerationResult(
      this->callInst = GetBlock()->GetIRBuilder()->CreateCall(this->function->GetLlvmFunction(), args));
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage CallFunction::PostGenerateCode()
{
  this->function = nullptr;
  SAFE_DELETE_LLVM_INST(this->callInst);
  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string CallFunction::ToString()
{
  return this->funcName + GetArguments()->ToString();
}
}

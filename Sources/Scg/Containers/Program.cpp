/**
 * @file Scg/Containers/Program.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg include files
#include <BuiltInFunctions/BasicBinaryBuiltInFunction.h>
#include <BuiltInFunctions/BasicUnaryBuiltInFunction.h>
#include <Containers/Module.h>
#include <Containers/Program.h>
#include <LlvmContainer.h>
#include <Types/DoubleType.h>
#include <Types/FloatType.h>
#include <Types/IntegerType.h>

// LLVM header files
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/Module.h>
#include <llvm/Linker.h>
#include <llvm/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

namespace Scg
{
void Program::InitialiseBuiltInFunctions()
{
  // Add functions
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__add",
      "int", "int", "int",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateAdd(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__add",
      "float", "float", "float",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFAdd(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__add",
      "double", "double", "double",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFAdd(a, b);
  }));

  // Subtract functions
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__sub",
      "int", "int", "int",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateSub(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__sub",
      "float", "float", "float",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFSub(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__sub",
      "double", "double", "double",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFSub(a, b);
  }));

  // Multiplication functions
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__mul",
      "int", "int", "int",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateMul(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__mul",
      "float", "float", "float",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFMul(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__mul",
      "double", "double", "double",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFMul(a, b);
  }));

  // Division functions
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__div",
      "int", "int", "int",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateSDiv(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__div",
      "float", "float", "float",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFDiv(a, b);
  }));
  this->builtInFunctions.Add(new BasicBinaryBuiltInFunction("__div",
      "double", "double", "double",
      [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFDiv(a, b);
  }));

  // Negative functions
  this->builtInFunctions.Add(new BasicUnaryBuiltInFunction("__neg",
      "int", "int",
      [](llvm::IRBuilder<> *irb, llvm::Value *a) {
      return irb->CreateNeg(a);
  }));
  this->builtInFunctions.Add(new BasicUnaryBuiltInFunction("__neg",
      "float", "float",
      [](llvm::IRBuilder<> *irb, llvm::Value *a) {
    return irb->CreateFNeg(a);
  }));
  this->builtInFunctions.Add(new BasicUnaryBuiltInFunction("__neg",
      "double", "double",
      [](llvm::IRBuilder<> *irb, llvm::Value *a) {
    return irb->CreateFNeg(a);
  }));
}

bool Program::HasFunction(const std::string &name,
		const ValueTypeSpecArray &arguments) const
{
	auto nonConstThis = const_cast<Program*>(this);
	return nonConstThis->FindDefineFunction(name, arguments).size() > 0 ||
			nonConstThis->FindDeclareFunction(name, arguments).size() > 0;
}

std::vector<Function *> Program::GetFunction(
    const std::string &funcName, const ValueTypeSpecArray &arguments)
{
  std::vector<Function *> matches;
  for (auto module : this->modules)
  {
    auto defFunc = module->GetFunction(funcName, arguments);
    if (defFunc != nullptr)
      matches.push_back(defFunc);
  }
  return matches;
}

std::vector<Function *> Program::MatchFunction(
    const std::string &funcName, const ValueTypeSpecArray &arguments)
{
  std::vector<Function *> matches;
  for (auto module : this->modules) {
    auto defFunc = module->MatchFunction(funcName, arguments);
    if (defFunc != nullptr) {
      matches.push_back(defFunc);
    }
  }

  if (matches.empty()) {
    // The module is not needed, so we pass the first one.
    // TODO: Remove the need to pass a module when it is not needed.
    auto match = this->builtInFunctions.Match(*this->modules[0], funcName,
        arguments);
    if (match != nullptr) {
      matches.push_back(match);
    }
  }

  return matches;
}

std::vector<DefineFunction*> Program::FindDefineFunction(
    const std::string &funcName, const ValueTypeSpecArray &arguments)
{
  std::vector<DefineFunction*> matches;
  for (auto module : this->modules)
  {
    auto defFunc = module->FindDefineFunction(funcName, arguments);
    if (defFunc != nullptr)
      matches.push_back(defFunc);
  }
  return matches;
}

std::vector<DeclareExtFunction*> Program::FindDeclareFunction(const std::string &funcName,
    const ValueTypeSpecArray &arguments)
{
  std::vector<DeclareExtFunction*> matches;
  for (auto module : this->modules)
  {
    auto declFunc = module->FindDeclareFunction(funcName, arguments);
    if (declFunc != nullptr)
      matches.push_back(declFunc);
  }
  return matches;
}

std::string Program::Compile()
{
  if (this->modules.empty())
    throw EXCEPTION(InvalidObjectException, "The program doesn't contain any "
        "module and cannot be executed.");

  this->llvmModule = new llvm::Module("AlususProgram", LlvmContainer::GetContext());

  for (auto module : this->modules) {
    module->SetProgram(this);
  }

  // Repeatedly call pre-code generation until all modules are ready for code generation.
  bool repeat = true;
  int repeatCount = 0;
  while (repeat && repeatCount < MAX_PRE_CODE_GEN_REPEAT) {
    repeat = false; repeatCount++;
    for (auto module : this->modules) {
      if (module->CallPreGenerateCode() != Expression::CodeGenerationStage::CodeGeneration) {
      	repeat = true;
      }
    }
  }
  if (repeat == true) {
    throw EXCEPTION(SystemException,
  			"Couldn't finish the pre-code generation step of compilation.");
  }

  // Generates code for all modules.
  for (auto module : this->modules) {
  	module->CallGenerateCode();
  }

  std::string out;
  llvm::raw_string_ostream ostream(out);
  llvm::PassManager passManager;
  passManager.add(llvm::createPrintModulePass(&ostream));
  passManager.run(*this->llvmModule);

  // Call post-code generation for all modules.
  for (auto module : this->modules) {
    module->CallPostGenerateCode();
    module->SetProgram(nullptr);
  }

  delete llvmModule;

  return out;
}

void Program::Execute(const char *functionName)
{
	if (this->modules.empty())
    throw EXCEPTION(InvalidObjectException, "The program doesn't contain any "
        "module and cannot be executed.");

  this->llvmModule = new llvm::Module("AlususProgram", LlvmContainer::GetContext());

  for (auto module : this->modules) {
    module->SetProgram(this);
  }

  // Repeatedly call pre-code generation until all modules are ready for code generation.
  bool repeat = true;
  int repeatCount = 0;
  while (repeat && repeatCount < MAX_PRE_CODE_GEN_REPEAT) {
    repeat = false; repeatCount++;
    for (auto module : this->modules) {
      if (module->CallPreGenerateCode() != Expression::CodeGenerationStage::CodeGeneration) {
      	repeat = true;
      }
    }
  }
  if (repeat == true) {
    throw EXCEPTION(SystemException,
  			"Couldn't finish the pre-code generation step of compilation.");
  }

  // Generates code for all modules.
  repeat = true;
  repeatCount = 0;
  while (repeat && repeatCount < MAX_PRE_CODE_GEN_REPEAT) {
    repeat = false; repeatCount++;
    for (auto module : this->modules) {
      if (module->CallGenerateCode() != Expression::CodeGenerationStage::PostCodeGeneration) {
      	repeat = true;
      }
    }
  }
  if (repeat == true) {
    throw EXCEPTION(SystemException,
  			"Couldn't finish the code generation step of compilation.");
  }

  // Execute the generated IR code.
  llvm::InitializeNativeTarget();
  auto ee = llvm::ExecutionEngine::createJIT(this->llvmModule);
  auto func = ee->FindFunctionNamed(functionName);
  std::vector<llvm::GenericValue> args(0);
  ee->runFunction(func, args);

  // Call post-code generation for all modules.
  repeat = true;
  repeatCount = 0;
  while (repeat && repeatCount < MAX_PRE_CODE_GEN_REPEAT) {
    repeat = false; repeatCount++;
    for (auto module : this->modules) {
      if (module->CallPostGenerateCode() != Expression::CodeGenerationStage::None) {
      	repeat = true;
      }
    }
  }
  // FIXME: There is a problem with post-code generation at the moment and we end up
  // getting the exception below. We need to fix that.
  /*if (repeat == true) {
  	// TODO: Should we just log this and report that there is a possible memory leak?
    throw EXCEPTION(SystemException,
  			"Couldn't finish the post-code generation step of compilation.");
  }*/
  for (auto module : this->modules) {
    module->SetProgram(nullptr);
  }

  delete llvmModule;
}
}

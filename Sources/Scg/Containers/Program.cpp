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
#include <Containers/Program.h>
#include <exceptions.h>

// LLVM header files
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/IR/Module.h>
#include <llvm/Linker.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>

namespace Scg
{
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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

std::string Program::Compile()
{
  if (this->modules.empty())
    THROW_EXCEPTION(InvalidObjectException, "The program doesn't contain any "
        "module and cannot be executed.");

  // Generates the code for the modules making up the program.
  for (auto module : this->modules)
  {
    module->SetProgram(this);
    module->PreGenerateCode();
    module->GenerateCode();
  }

  // Links the modules together.
  auto module = this->modules[0];
  llvm::Linker linker(module->GetLlvmModule());
  for (auto i = 1; i < this->modules.size(); i++)
    linker.linkInModule(this->modules[i]->GetLlvmModule(),
        llvm::Linker::PreserveSource, nullptr);

  std::string out;
  llvm::raw_string_ostream ostream(out);
  llvm::PassManager passManager;
  passManager.add(llvm::createPrintModulePass(&ostream));
  passManager.run(*linker.getModule());

  // Finalises the modules making up the program.
  for (auto module : this->modules)
  {
    module->PostGenerateCode();
    module->SetProgram(nullptr);
  }

  return out;
}

//------------------------------------------------------------------------------

void Program::Execute(const char *functionName)
{
  if (this->modules.empty())
    THROW_EXCEPTION(InvalidObjectException, "The program doesn't contain any "
        "module and cannot be executed.");

  // Generates the code for the modules making up the program.
  for (auto module : this->modules)
  {
    module->SetProgram(this);
    module->PreGenerateCode();
    module->GenerateCode();
  }

  // Links the modules together.
  auto module = this->modules[0];
  llvm::Linker linker(module->GetLlvmModule());
  for (auto i = 1; i < this->modules.size(); i++)
    linker.linkInModule(this->modules[i]->GetLlvmModule(),
        llvm::Linker::PreserveSource, nullptr);

  // Execute the generated IR code.
  llvm::InitializeNativeTarget();
  auto ee = llvm::ExecutionEngine::createJIT(linker.getModule());
  auto func = ee->FindFunctionNamed(functionName);
  std::vector<llvm::GenericValue> args(0);
  ee->runFunction(func, args);

  // Finalises the modules making up the program.
  for (auto module : this->modules)
    module->PostGenerateCode();
}
}

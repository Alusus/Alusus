/**
 * @file Scg/CodeGenUnit.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg include files
#include <Functions/BasicBinaryBuiltInFunction.h>
#include <Functions/BasicUnaryBuiltInFunction.h>
#include <Containers/Module.h>
#include <CodeGenUnit.h>
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

void CodeGenUnit::initialiseBuiltInFunctions()
{
  // add functions
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__add",
                             "int", "int", "int",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateAdd(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__add",
                             "float", "float", "float",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFAdd(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__add",
                             "double", "double", "double",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFAdd(a, b);
  }));

  // Subtract functions
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__sub",
                             "int", "int", "int",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateSub(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__sub",
                             "float", "float", "float",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFSub(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__sub",
                             "double", "double", "double",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFSub(a, b);
  }));

  // Multiplication functions
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__mul",
                             "int", "int", "int",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateMul(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__mul",
                             "float", "float", "float",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFMul(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__mul",
                             "double", "double", "double",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFMul(a, b);
  }));

  // Division functions
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__div",
                             "int", "int", "int",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateSDiv(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__div",
                             "float", "float", "float",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFDiv(a, b);
  }));
  this->builtInFunctions.add(std::make_shared<BasicBinaryBuiltInFunction>("__div",
                             "double", "double", "double",
  [](llvm::IRBuilder<> *irb, llvm::Value *a, llvm::Value *b) {
    return irb->CreateFDiv(a, b);
  }));

  // Negative functions
  this->builtInFunctions.add(std::make_shared<BasicUnaryBuiltInFunction>("__neg",
                             "int", "int",
  [](llvm::IRBuilder<> *irb, llvm::Value *a) {
    return irb->CreateNeg(a);
  }));
  this->builtInFunctions.add(std::make_shared<BasicUnaryBuiltInFunction>("__neg",
                             "float", "float",
  [](llvm::IRBuilder<> *irb, llvm::Value *a) {
    return irb->CreateFNeg(a);
  }));
  this->builtInFunctions.add(std::make_shared<BasicUnaryBuiltInFunction>("__neg",
                             "double", "double",
  [](llvm::IRBuilder<> *irb, llvm::Value *a) {
    return irb->CreateFNeg(a);
  }));
}

bool CodeGenUnit::hasFunction(const std::string &name,
                          const ValueTypeSpecArray &arguments) const
{
  auto nonConstThis = const_cast<CodeGenUnit*>(this);
  return nonConstThis->findDefineFunction(name, arguments).size() > 0 ||
         nonConstThis->findDeclareFunction(name, arguments).size() > 0;
}

std::vector<Function *> CodeGenUnit::getFunction(
  const std::string &funcName, const ValueTypeSpecArray &arguments)
{
  std::vector<Function *> matches;

  for (auto module : this->modules) {
    auto defFunc = module->getFunction(funcName, arguments).get();

    if (defFunc != nullptr)
      matches.push_back(defFunc);
  }

  return matches;
}

std::vector<Function *> CodeGenUnit::matchFunction(
  const std::string &funcName, const ValueTypeSpecArray &arguments)
{
  std::vector<Function *> matches;

  for (auto module : this->modules) {
    auto defFunc = module->matchFunction(funcName, arguments);

    if (defFunc != nullptr) {
      matches.push_back(defFunc);
    }
  }

  if (matches.empty()) {
    // The module is not needed, so we pass the first one.
    // TODO: Remove the need to pass a module when it is not needed.
    auto match = this->builtInFunctions.match(*this->modules[0], funcName, arguments);

    if (match != nullptr) {
      matches.push_back(match);
    }
  }

  return matches;
}

std::vector<DefineFunction*> CodeGenUnit::findDefineFunction(
  const std::string &funcName, const ValueTypeSpecArray &arguments)
{
  std::vector<DefineFunction*> matches;

  for (auto module : this->modules) {
    auto defFunc = module->findDefineFunction(funcName, arguments);

    if (defFunc != nullptr)
      matches.push_back(defFunc);
  }

  return matches;
}

std::vector<DeclareExtFunction*> CodeGenUnit::findDeclareFunction(const std::string &funcName,
    const ValueTypeSpecArray &arguments)
{
  std::vector<DeclareExtFunction*> matches;

  for (auto module : this->modules) {
    auto declFunc = module->findDeclareFunction(funcName, arguments);

    if (declFunc != nullptr)
      matches.push_back(declFunc);
  }

  return matches;
}

std::string CodeGenUnit::compile()
{
  if (this->modules.empty())
    throw EXCEPTION(InvalidObjectException, "The program doesn't contain any "
                    "module and cannot be executed.");

  this->llvmModule = new llvm::Module("AlususProgram", LlvmContainer::getContext());
  this->llvmModule->setDataLayout(LlvmContainer::getDataLayout()->getStringRepresentation());

  // Repeatedly call pre-code generation until all modules are ready for code generation.
  bool repeat = true;
  int repeatCount = 0;

  while (repeat && repeatCount < MAX_PRE_CODE_GEN_REPEAT) {
    repeat = false;
    repeatCount++;

    for (auto module : this->modules) {
      if (module->callPreGenerateCode(this) != AstNode::CodeGenerationStage::CodeGeneration) {
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
    module->callGenerateCode(this);
  }

  std::string out;
  llvm::raw_string_ostream ostream(out);
  llvm::PassManager passManager;
  passManager.add(llvm::createPrintModulePass(&ostream));
  passManager.run(*this->llvmModule);

  // Call post-code generation for all modules.
  for (auto module : this->modules) {
    module->callPostGenerateCode(this);
  }

  delete llvmModule;

  return out;
}

void CodeGenUnit::execute(const char *functionName)
{
  if (this->modules.empty())
    throw EXCEPTION(InvalidObjectException, "The program doesn't contain any "
                    "module and cannot be executed.");

  this->llvmModule = new llvm::Module("AlususProgram", LlvmContainer::getContext());

  // Repeatedly call pre-code generation until all modules are ready for code generation.
  bool repeat = true;
  int repeatCount = 0;

  while (repeat && repeatCount < MAX_PRE_CODE_GEN_REPEAT) {
    repeat = false;
    repeatCount++;

    for (auto module : this->modules) {
      if (module->callPreGenerateCode(this) != AstNode::CodeGenerationStage::CodeGeneration) {
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
    repeat = false;
    repeatCount++;

    for (auto module : this->modules) {
      if (module->callGenerateCode(this) != AstNode::CodeGenerationStage::PostCodeGeneration) {
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
    repeat = false;
    repeatCount++;

    for (auto module : this->modules) {
      if (module->callPostGenerateCode(this) != AstNode::CodeGenerationStage::None) {
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

  delete llvmModule;
}

} // namespace

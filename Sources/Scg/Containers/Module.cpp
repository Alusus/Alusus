/**
 * @file Scg/Containers/Module.cpp
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

// LLVM include files
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Module.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/PassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

// Scg include files
#include <Containers/Module.h>
#include <Containers/Program.h>
#include <Expression.h>
#include <Instructions/CallFunction.h>
#include <Instructions/DeclareExtFunction.h>
#include <Instructions/DefineFunction.h>
#include <Types/ArrayType.h>
#include <Types/PointerType.h>
#include <Types/StringType.h>
#include <Types/ValueType.h>
#include <LlvmContainer.h>

using namespace llvm;

namespace Scg
{
Module::Module(const std::string& name) : functionStore(*this)
{
  this->llvmModule = new llvm::Module(name, LlvmContainer::GetContext());
}

//------------------------------------------------------------------------------

Module::~Module()
{
  for(auto expr : this->expressions)
    delete expr;
  for (auto type : this->allocatedTypes)
    delete type;
  delete this->llvmModule;
}

//------------------------------------------------------------------------------

ValueType *Module::GetValueTypeByName(const std::string &typeName) const
{
  ValueType *type;
  // First try the types defined in the module.
  auto it = this->typeMap.find(typeName);
  if (it != this->typeMap.end())
    type = it->second;
  else
    type = ValueType::GetPrimitiveType(typeName);
  if (type == nullptr)
    // TODO: String concatenation might allocate memory which will reduce
    // compilation speed. We should use pre-allocated memory instead.
    THROW_EXCEPTION(InvalidTypeException, "Use of undefined type: " + typeName);
  return type;
}

//------------------------------------------------------------------------------

ArrayType *Module::GetArrayValueType(ValueType &elementsType, int arraySize) const
{
  // TODO: We need to create a map of all the so-far allocated value types to
  // avoid creating a value type twice.
  // TODO: Change ValueType& to ValueType* or change the constructor of
  // ArrayType.
  auto arrayType = new ArrayType(&elementsType, arraySize);
  this->allocatedTypes.push_back(arrayType);
  return arrayType;
}

//------------------------------------------------------------------------------

PointerType *Module::GetPointerValueType(ValueType &contentType) const
{
  // TODO: We need to create a map of all the so-far allocated value types to
  // avoid creating a value type twice.
  auto type = new PointerType(contentType);
  this->allocatedTypes.push_back(type);
  return type;
}

//------------------------------------------------------------------------------

DefineFunction *Module::FindDefineFunction(const std::string &name,
    const ValueTypeSpecArray &arguments)
{
  // TODO: We need to create a map for faster retrieval.
  // TODO: We need to raise an error if there is more than one match.
  for (auto expr : this->expressions)
  {
    // TODO: Don't use dynamic_cast.
    auto defFunc = dynamic_cast<DefineFunction*>(expr);
    if (defFunc == nullptr)
      continue;
    if (defFunc->GetName().compare(name) == 0 &&
        defFunc->GetArguments().AreTypesEqualTo(&arguments))
      return defFunc;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

DeclareExtFunction *Module::FindDeclareFunction(const std::string &name,
    const ValueTypeSpecArray &arguments)
{
  // TODO: We need to create a map for faster retrieval.
  // TODO: We need to raise an error if there is more than one match.
  for (auto expr : this->expressions)
  {
    // TODO: Don't use dynamic_cast.
    auto declFunc = dynamic_cast<DeclareExtFunction*>(expr);
    if (declFunc == nullptr)
      continue;
    if (declFunc->GetName().compare(name) == 0 &&
        declFunc->GetArgumentTypes().IsEqualTo(&arguments,
            declFunc->GetArgumentTypes().size()))
      return declFunc;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

std::string Module::GetTempVarName()
{
  std::stringstream str;
  str << "_tmp" << this->tempIndex++;
  return str.str();
}

//------------------------------------------------------------------------------

void Module::PrependExpression(Expression *expr)
{
  this->expressions.insert(this->expressions.begin(), expr);
  expr->SetModule(this);
  expr->SetBlock(nullptr);
}

//------------------------------------------------------------------------------

void Module::PrependExpressions(ExpressionArray &expr)
{
  this->expressions.insert(this->expressions.begin(), expr.begin(), expr.end());
  for (auto e : expr)
  {
    e->SetModule(this);
    e->SetBlock(nullptr);
  }
}

//------------------------------------------------------------------------------

void Module::AppendExpression(Expression *expr)
{
  this->expressions.push_back(expr);
  expr->SetModule(this);
  expr->SetBlock(nullptr);
}

//------------------------------------------------------------------------------

void Module::AppendExpressions(ExpressionArray &expr)
{
  this->expressions.insert(this->expressions.end(), expr.begin(), expr.end());
  for (auto e : expr)
  {
    e->SetModule(this);
    e->SetBlock(nullptr);
  }
}

//------------------------------------------------------------------------------

void Module::Compile()
{
  PreGenerateCode();
  GenerateCode();

  // TODO: This is only put here for testing purposes. Remove!
  PassManager passManager;
  passManager.add(createPrintModulePass(&outs()));
  passManager.run(*this->llvmModule);

  PostGenerateCode();
}

//------------------------------------------------------------------------------

void Module::Compile(std::string &out)
{
  PreGenerateCode();
  GenerateCode();

  // TODO: This is only put here for testing purposes. Remove!
  raw_string_ostream ostream(out);
  PassManager passManager;
  passManager.add(createPrintModulePass(&ostream));
  passManager.run(*this->llvmModule);

  PostGenerateCode();
}

//------------------------------------------------------------------------------

void Module::Execute(const char *functionName)
{
  PreGenerateCode();
  GenerateCode();

  // Execute the generated IR code.
  llvm::InitializeNativeTarget();
  llvm::ExecutionEngine *ee = llvm::ExecutionEngine::createJIT(this->llvmModule);
  llvm::Function* func = ee->FindFunctionNamed(functionName);
  std::vector<llvm::GenericValue> args(0);
  ee->runFunction(func, args);

  PostGenerateCode();
}

//------------------------------------------------------------------------------

void Module::PreGenerateCode()
{
  // It is required to call this multiple times because code dependencies might
  // some time prevent freeing memory.
  bool repeat = true;
  int repeatCount = 0;
  while (repeat && repeatCount < MAX_PRE_CODE_GEN_REPEAT)
  {
    // Inserts complementary expressions awaiting addition to the main
    // expression list.
    PrependExpressions(this->headerComplExprs);
    this->headerComplExprs.clear();
    AppendExpressions(this->footerComplExprs);
    this->footerComplExprs.clear();
    this->autoLinkFuncSet.clear();

    repeat = false; repeatCount++;
    for (auto expr : this->expressions)
      if (expr->CallPreGenerateCode() != Expression::CodeGenerationStage::CodeGeneration)
        repeat = true;
  }
  if (repeat) {
    // We repeated a maximum of MAX_PRE_CODE_GEN_REPEAT and failed to finish
    // code generation. This indicates a problem in our code that need to be
    // solved.
    THROW_EXCEPTION(SystemException, "Couldn't finish pre-code generation!");
  }
}

//------------------------------------------------------------------------------

void Module::GenerateCode()
{
  for (auto expr : this->expressions)
    expr->GenerateCode();

}

//------------------------------------------------------------------------------

void Module::PostGenerateCode()
{
  // It is required to call this multiple times because code dependencies might
  // some time prevent freeing memory.
  bool repeat = true;
  int repeatCount = 0;
  while (repeat && repeatCount < MAX_POST_CODE_GEN_REPEAT)
  {
    repeat = false; repeatCount++;
    for (auto expr : this->expressions)
      if (expr->CallPostGenerateCode() != Expression::CodeGenerationStage::None)
        repeat = true;
  }
  /*if (repeat) {
    // We repeated a maximum of MAX_POST_CODE_GEN_REPEAT and failed to finish
    // code generation. This indicates a problem in our code that need to be
    // solved.
    THROW_EXCEPTION(SystemException, "Couldn't finish post-code generation!");
  }*/
}

//------------------------------------------------------------------------------

void Module::GenerateRequiredDeclExtFunctions()
{
  for (auto expr : this->expressions)
  {
    //
    auto callFunc = dynamic_cast<CallFunction*>(expr);
    if (callFunc == nullptr)
      continue;

    // Finds where this function is defined.
    // TODO: Modify FindDefineFunction() function such that we are able to pass
    // function arguments.
    auto allDefFuncs = this->program->FindDefineFunction(
        callFunc->GetFunctionName(), callFunc->GetArgumentTypes());
    if (allDefFuncs.size() == 0)
      THROW_EXCEPTION(UndefinedFunctionException,
          "Calling undefined function: " + callFunc->GetFunctionName());
    else if (allDefFuncs.size() > 1)
      // TODO: Should we ignore other matches if there is a match within the
      // same module?
      THROW_EXCEPTION(MultipleMatchesException,
          "Found multiple of the function: " + callFunc->GetFunctionName());

    auto defFunc = allDefFuncs[0];
    if (defFunc->GetModule() == this)
      // The required function is defined within the same module, so we don't
      // need to declare.
      continue;

    auto declFunc = new DeclareExtFunction(defFunc);
    this->expressions.insert(this->expressions.begin(), declFunc);
  }
}
}

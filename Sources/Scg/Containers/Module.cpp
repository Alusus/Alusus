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

// LLVM include files
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Module.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

// Scg include files
#include <Containers/Module.h>
#include <Containers/Program.h>
#include <Instructions/CallFunction.h>
#include <Instructions/DeclareExtFunction.h>
#include <Instructions/DefineFunction.h>
#include <Types/ArrayType.h>
#include <Types/PointerType.h>
#include <Types/StringType.h>
#include <LlvmContainer.h>

namespace Scg
{
Module::Module(const std::string& name) : functionStore(), name(name)
{
  //this->llvmModule = new llvm::Module(name, LlvmContainer::GetContext());
}

Module::~Module()
{
  /*for(auto expr : this->expressions)
    delete expr;*/
  //delete this->llvmModule;
}

const llvm::Module *Module::GetLlvmModule() const
{
	return this->program->GetLlvmModule();
}

llvm::Module *Module::GetLlvmModule()
{
	return this->program->GetLlvmModule();
}

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
    throw EXCEPTION(InvalidTypeException, ("Use of undefined type: " + typeName).c_str());
  return type;
}

bool Module::HasFunction(const std::string &name,
		const ValueTypeSpecArray &arguments) const
{
	auto nonConstThis = const_cast<Module*>(this);
	return nonConstThis->FindDefineFunction(name, arguments) != nullptr ||
			nonConstThis->FindDeclareFunction(name, arguments) != nullptr;
}

DefineFunction *Module::FindDefineFunction(const std::string &name,
    const ValueTypeSpecArray &arguments)
{
  // TODO: We need to create a map for faster retrieval.
  // TODO: We need to raise an error if there is more than one match.
  for (auto expr : this->children)
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

DeclareExtFunction *Module::FindDeclareFunction(const std::string &name,
    const ValueTypeSpecArray &arguments)
{
  // TODO: We need to create a map for faster retrieval.
  // TODO: We need to raise an error if there is more than one match.
  for (auto expr : this->children)
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

std::string Module::GetTempVarName()
{
  std::stringstream str;
  str << "_tmp" << this->tempIndex++;
  return str.str();
}

void Module::PrependExpression(Expression *expr)
{
  this->children.insert(this->children.begin(), expr);
  expr->SetModule(this);
  expr->SetBlock(nullptr);
}

void Module::PrependExpressions(ExpressionArray &expr)
{
  this->children.insert(this->children.begin(), expr.begin(), expr.end());
  for (auto e : expr)
  {
    e->SetModule(this);
    e->SetBlock(nullptr);
  }
}

void Module::AppendExpression(Expression *expr)
{
  this->children.push_back(expr);
  expr->SetModule(this);
  expr->SetBlock(nullptr);
}

void Module::AppendExpressions(ExpressionArray &expr)
{
  this->children.insert(this->children.end(), expr.begin(), expr.end());
  for (auto e : expr)
  {
    e->SetModule(this);
    e->SetBlock(nullptr);
  }
}
}

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
#include <Instructions/CallFunction.h>
#include <Instructions/DeclareExtFunction.h>
#include <Instructions/DefineFunction.h>
#include <Types/ArrayType.h>
#include <Types/PointerType.h>
#include <Types/StringType.h>
#include <LlvmContainer.h>

namespace Scg
{

Module::~Module()
{
  for (Int i = 0; i < this->children.getCount(); ++i) {
    DISOWN_PLAINPTR(this->children.get(i));
  }
}


ValueType* Module::getValueTypeByName(const std::string &typeName) const
{
  ValueType *type = this->tryGetValueTypeByName(typeName);

  if (type == nullptr)
    // TODO: String concatenation might allocate memory which will reduce
    // compilation speed. We should use pre-allocated memory instead.
    throw EXCEPTION(InvalidTypeException, ("Use of undefined type: " + typeName).c_str());

  return type;
}

ValueType* Module::tryGetValueTypeByName(const std::string &typeName) const
{
  ValueType *type;
  // First try the types defined in the module.
  auto it = this->typeMap.find(typeName);

  if (it != this->typeMap.end())
    type = it->second;
  else
    type = ValueType::getPrimitiveType(typeName);

  return type;
}

bool Module::hasFunction(const std::string &name,
                         const ValueTypeSpecArray &arguments) const
{
  auto nonConstThis = const_cast<Module*>(this);
  return nonConstThis->findDefineFunction(name, arguments) != nullptr ||
         nonConstThis->findDeclareFunction(name, arguments) != nullptr;
}

DefineFunction *Module::findDefineFunction(const std::string &name,
    const ValueTypeSpecArray &arguments)
{
  // TODO: We need to create a map for faster retrieval.
  // TODO: We need to raise an error if there is more than one match.
  for (Int i = 0; i < this->children.getCount(); ++i) {
    auto expr = static_cast<AstNode*>(this->children.get(i));
    // TODO: Don't use dynamic_cast.
    auto defFunc = tio_cast<DefineFunction>(expr);

    if (defFunc == nullptr)
      continue;

    if (defFunc->getName().compare(name) == 0 &&
        defFunc->getArguments().areTypesEqualTo(&arguments))
      return defFunc;
  }

  return nullptr;
}

DeclareExtFunction *Module::findDeclareFunction(const std::string &name,
    const ValueTypeSpecArray &arguments)
{
  // TODO: We need to create a map for faster retrieval.
  // TODO: We need to raise an error if there is more than one match.
  for (Int i = 0; i < this->children.getCount(); ++i) {
    auto expr = static_cast<AstNode*>(this->children.get(i));
    // TODO: Don't use dynamic_cast.
    auto declFunc = tio_cast<DeclareExtFunction>(expr);

    if (declFunc == nullptr)
      continue;

    if (declFunc->getName().compare(name) == 0 &&
        declFunc->getArgumentTypes().isEqualTo(&arguments,
            declFunc->getArgumentTypes().size()))
      return declFunc;
  }

  return nullptr;
}

std::string Module::getTempVarName()
{
  std::stringstream str;
  str << "_tmp" << this->tempIndex++;
  return str.str();
}

void Module::prependNode(SharedPtr<AstNode> const &node)
{
  this->children.insert(0, node);
  OWN_SHAREDPTR(node);
}

void Module::appendNode(SharedPtr<AstNode> const &node)
{
  this->children.add(node);
  OWN_SHAREDPTR(node);
}

} // namespace

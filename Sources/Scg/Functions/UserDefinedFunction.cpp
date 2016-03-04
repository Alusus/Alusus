/**
 * @file Scg/Functions/UserDefinedFunction.cpp
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

// Scg files
#include <CodeGenUnit.h>
#include <Containers/Block.h>
#include <Containers/Module.h>
#include <Functions/UserDefinedFunction.h>
#include <Values/Variable.h>

using namespace llvm;

namespace Scg
{

//==============================================================================
// Constructors & Destructor

UserDefinedFunction::UserDefinedFunction(std::string const &name,
                                         SharedPtr<ValueTypeSpec> const &returnTypeSpec,
                                         VariableDefinitionArray const &argDefs,
                                         SharedPtr<Block> const &body)
  : name(name)
  , body(body)
  , returnTypeSpec(returnTypeSpec)
  , argDefs(argDefs)
{
  for (auto def : this->argDefs) {
    // TODO: Is there anyway to avoid casting away the constness?
    this->argTypeSpecs.push_back(def.getTypeSpec().c_cast<ValueTypeSpec>());
  }
  OWN_SHAREDPTR(this->body);
}


UserDefinedFunction::~UserDefinedFunction()
{
  // We don't delete this->returnType, etc., because they should be deleted by
  // the defining instruction.
  // TODO: This goes against the convention I have been following that the
  // class should delete the pointers it receives, but I have to do this way
  // because otherwise the pointers will be deleted more than once if the
  // code is compiled twice, because DefineFunction defines a new UserDefinedFunction
  // instance each time its PreCodeGeneration() function called.

  DISOWN_SHAREDPTR(this->body);
}


//==============================================================================
// Member Functions

llvm::Value *UserDefinedFunction::createLLVMInstruction(llvm::IRBuilder<> *irb,
    const std::vector<llvm::Value*> &args) const
{
  return irb->CreateCall(this->llvmFunction, args);
}


void UserDefinedFunction::createFunction(CodeGenUnit *codeGenUnit)
{
  // Constructs the LLVM types representing the argument and return value types.
  std::vector<Type*> argTypes(this->argDefs.size());

  auto module = this->findOwner<Module>();

  for (auto i = 0; i < argTypes.size(); i++) {
    auto type = this->argDefs[i].getTypeSpec()->toValueType(*module);
    argTypes[i] = type->getLlvmType();
  }

  auto retType =
    (this->returnTypeSpec != nullptr ?
     this->returnTypeSpec->toValueType(*module) :
     module->getValueTypeByName(""))->getLlvmType();

  // Constructs the LLVM function type.
  auto funcType = llvm::FunctionType::get(retType, argTypes,
                                          false);

  // Creates the LLVM function representing this function.
  std::string fullName = module->getName() + std::string("_") + this->name;
  this->llvmFunction = llvm::Function::Create(funcType,
                       llvm::Function::ExternalLinkage, fullName, codeGenUnit->getLlvmModule());

  // Creates the Alusus variables wrapping the arguments of the function.
  auto i = 0;

  for (auto iter = this->llvmFunction->arg_begin(); i != this->argDefs.size();
       ++iter, ++i) {
    iter->setName(this->argDefs[i].getVariableName());
    auto var =
      this->argDefs[i].getTypeSpec()->toValueType(*module)->newVariable(
        this->argDefs[i].getVariableName(), iter);
    getBody()->prependNode(var);
    this->args.push_back(var.get());
  }
}


AstNode::CodeGenerationStage UserDefinedFunction::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  MODULE_CHECK;

  // Create the LLVM function early because it is referenced in other pre-code
  // generation places, e.g. Block.
  // TODO: Should we add a function map to Block class as well so that we can
  // define functions inside blocks?

  // Is the function already defined?
  Module *module = this->findOwner<Module>();
  if (module->getFunction(this->name, this->getArgumentTypeSpecs()) != nullptr) {
    throw EXCEPTION(RedefinitionException,
                    ("Function already defined: " + this->name).c_str());
  }

  // Stores this function in the function store of the module.
  module->addFunction(s_cast<Function>(this->getSharedThis()));

  createFunction(codeGenUnit);

  return CodeGenerationStage::CodeGeneration;
}


AstNode::CodeGenerationStage UserDefinedFunction::generateCode(CodeGenUnit *codeGenUnit)
{
  MODULE_CHECK;

  getBody()->generateCode(codeGenUnit);

  // If this function is of type void, we need to make sure there is a
  // return statement at the end in case the user doesn't add one.
  // TODO: What if the function is non-void and the user forgets to return
  // a value?
  if (this->returnTypeSpec == nullptr || this->returnTypeSpec->isVoid())
    getBody()->getIRBuilder()->CreateRetVoid();

  // A function definition doesn't evaluate to a value.
  return AstNode::generateCode(codeGenUnit);
}

// TODO: Do we not need to define post code generation code?

std::string UserDefinedFunction::toString()
{
  std::stringstream str;
  str << "def " << this->name << " : function" << std::endl;

  if (this->argDefs.size() > 0) {
    str << "(";

    for (auto i = 0; i < this->argDefs.size(); i++)
      str << this->argDefs[0].toString();

    str << ")";
  }

  str << getBody()->toString();
  return str.str();
}

} // namespace

/**
 * @file Spp/LlvmCodeGen/Generator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::Generator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"
#include <regex>

namespace Spp { namespace LlvmCodeGen
{

//==============================================================================
// Initialization Functions

void Generator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateModule,
    &this->generateFunction,
    &this->generateFunctionDecl,
    &this->generateBlock,
    &this->generateStatement,
    &this->generateParamPass,
    &this->generateIdentifier,
    &this->generateStringLiteral
  });
}


void Generator::initBindings()
{
  this->generateModule = &Generator::_generateModule;
  this->generateFunction = &Generator::_generateFunction;
  this->generateFunctionDecl = &Generator::_generateFunctionDecl;
  this->generateBlock = &Generator::_generateBlock;
  this->generateStatement = &Generator::_generateStatement;
  this->generateParamPass = &Generator::_generateParamPass;
  this->generateIdentifier = &Generator::_generateIdentifier;
  this->generateStringLiteral = &Generator::_generateStringLiteral;
  //  this->generateIfStatement = &Generator::_generateIfStatement;
  //  this->generateWhileStatement = &Generator::_generateWhileStatement;
  //  this->generateExpression = &Generator::_generateExpression;
}


//==============================================================================
// Main Operation Functions

Str Generator::generateIr(Core::Data::Ast::Scope *root)
{
  if (root == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("root"), STR("Must not be null."));
  }

  llvm::InitializeNativeTarget();

  SharedPtr<llvm::DataLayout> llvmDataLayout(new llvm::DataLayout(""));
  SharedPtr<llvm::Module> llvmModule(new llvm::Module("AlususProgram", llvm::getGlobalContext()));
  llvmModule->setDataLayout(llvmDataLayout->getStringRepresentation());

  // Generates code for all modules.
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->get(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        this->call<void>(this->generateModule, module, llvmModule.get());
      }
    }
  }

  Str out;
  llvm::raw_string_ostream ostream(out);
  llvm::PassManager passManager;
  passManager.add(llvm::createPrintModulePass(&ostream));
  passManager.run(*(llvmModule.get()));

  return out;
}


//==============================================================================
// Code Generation Functions

void Generator::_generateModule(TiObject *self, Spp::Ast::Module *astModule, llvm::Module *llvmModule)
{
  PREPARE_SELF(generator, Generator);
  for (Int i = 0; i < astModule->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(astModule->get(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Spp::Ast::Module>()) {
        generator->generateModule(static_cast<Spp::Ast::Module*>(obj), llvmModule);
      } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
        generator->generateFunction(static_cast<Spp::Ast::Function*>(obj), llvmModule);
      }
      // TODO: Generate for types and global variables.
    }
  }
}


void Generator::_generateFunction(TiObject *self, Spp::Ast::Function *astFunc, llvm::Module *llvmModule)
{
  PREPARE_SELF(generator, Generator);

  auto cgFunc = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::UserFunction>();
  if (cgFunc == 0) {
    generator->generateFunctionDecl(astFunc, llvmModule);
    cgFunc = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::UserFunction>();
  }
  if (cgFunc == 0) {
    throw EXCEPTION(GenericException, STR("Failed to generate function decleration."));
  }

  auto astArgs = astFunc->getArgTypes().get();
  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0) {
    // Create an llvm block and its irbuilder.
    auto cgBlock = std::make_shared<LlvmCodeGen::Block>();
    cgBlock->setLlvmBlock(llvm::BasicBlock::Create(
      llvm::getGlobalContext(),
      generator->getNewBlockName(),
      cgFunc->getLlvmFunction()
    ));
    cgBlock->setIrBuilder(new llvm::IRBuilder<>(cgBlock->getLlvmBlock()));
    astBlock->setExtra(META_EXTRA_NAME, cgBlock);

    // Create variables for each argument and assign them to cgFunc.
    auto i = 0;
    for (auto iter = cgFunc->getLlvmFunction()->arg_begin(); i != cgFunc->getArgCount(); ++iter, ++i) {
      iter->setName(astArgs->getKey(i).c_str());
      auto llvmAlloca = cgBlock->getIrBuilder()->CreateAlloca(cgFunc->getArg(i).type, 0, astArgs->getKey(i).c_str());
      auto llvmStore = cgBlock->getIrBuilder()->CreateStore(iter, llvmAlloca);
      cgFunc->setArgAllocaInst(i, llvmAlloca);
      cgFunc->setArgStoreInst(i, llvmStore);
    }

    // Generate the function's statements.
    generator->generateBlock(astBlock, cgFunc->getLlvmFunction());
  }
}


void Generator::_generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc, llvm::Module *llvmModule)
{
  PREPARE_SELF(generator, Generator);

  auto genType = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Function>();
  if (genType != 0) return;

  // Construct the list of argument LLVM types.
  // TODO: Support functions that take no args.
  auto astArgs = astFunc->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  std::vector<llvm::Type*> llvmArgTypes(argCount);
  for (Int i = 0; i < argCount; ++i) {
    llvmArgTypes[i] = generator->typeGenerator->getGeneratedLlvmType(astArgs->get(i), llvmModule);
  }

  // Get the return LLVM type.
  llvm::Type *llvmRetType = 0;
  if (astFunc->getRetType() != 0) {
    llvmRetType = generator->typeGenerator->getGeneratedLlvmType(astFunc->getRetType().get(), llvmModule);
  } else {
    llvmRetType = llvm::Type::getVoidTy(llvm::getGlobalContext());
  }

  // Generate the function object.
  auto llvmFuncType = llvm::FunctionType::get(llvmRetType, llvmArgTypes, false);
  Str name = generator->getFunctionName(astFunc);
  auto cgFunc = std::make_shared<LlvmCodeGen::UserFunction>();
  cgFunc->setLlvmFunction(llvm::Function::Create(llvmFuncType, llvm::Function::ExternalLinkage, name, llvmModule));
  for (auto llvmArgType : llvmArgTypes) {
    cgFunc->addArg(llvmArgType);
  }
  astFunc->setExtra(META_EXTRA_NAME, cgFunc);
}


void Generator::_generateBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc)
{
  PREPARE_SELF(generator, Generator);
  for (Int i = 0; i < astBlock->getCount(); ++i) {
    auto astNode = astBlock->get(i);
    Ast::Type *resultType;
    TiObject *resultCg;
    TiObject *lastProcessedRef;
    generator->generateStatement(astNode, llvmFunc, resultType, resultCg, lastProcessedRef);
  }
}


void Generator::_generateStatement(TiObject *self, TiObject *astNode, llvm::Function *llvmFunc,
                                   Ast::Type *&resultType, TiObject *&resultCg, TiObject *&lastProcessedRef)
{
  PREPARE_SELF(generator, Generator);
  if (astNode->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(astNode);
    generator->generateParamPass(paramPass, llvmFunc, resultType, resultCg, lastProcessedRef);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(astNode);
    generator->generateIdentifier(identifier, llvmFunc, resultType, resultCg, lastProcessedRef);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(astNode);
    generator->generateStringLiteral(stringLiteral, llvmFunc, resultType, resultCg, lastProcessedRef);
  }
  // TODO:
}


void Generator::_generateParamPass(TiObject *self, Core::Data::Ast::ParamPass *astNode,
                                   llvm::Function *llvmFunc,
                                   Ast::Type *&resultType, TiObject *&resultCg, TiObject *&lastProcessedRef)
{
  PREPARE_SELF(generator, Generator);
  auto operand = astNode->getOperand().get();
  generator->generateStatement(operand, llvmFunc, resultType, resultCg, lastProcessedRef);
  if (resultType != 0) {
    // TODO: Check for function pointers and arrays.
  } else {
    // TODO: Build param list.
    auto paramCgs = Core::Data::SharedList::create();
    auto paramTypes = Core::Data::SharedList::create();
    auto param = astNode->getParam().get();
    if (param->isDerivedFrom<Core::Data::Ast::ExpressionList>()) {
      auto paramList = static_cast<Core::Data::Ast::ExpressionList*>(param);
      for (Int i = 0; i < paramList->getCount(); ++i) {
        TiObject *paramResultCg;
        Ast::Type *paramType;
        TiObject *paramLastProcessedRef;
        generator->generateStatement(paramList->get(i), llvmFunc, paramType, paramResultCg, paramLastProcessedRef);
        // TODO:
      }
    } else {

    }
    generator->seeker->doForeach(operand, astNode->getOwner(),
      [=](TiObject *obj)->Core::Data::Seeker::SeekVerb
      {
        if (obj->isDerivedFrom<Spp::Ast::Function>()) {
          // TODO: Build funcion declaration.
          // TODO: Check parameter match.
        }
        return Core::Data::Seeker::SeekVerb::MOVE;
      }
    );
    // TODO: Create function call if a match was found.
    // TODO: Assign results.
    if (resultCg == 0) {
      throw EXCEPTION(GenericException, STR("No matching callee found."));
    }
  }
}


void Generator::_generateIdentifier(TiObject *self, Core::Data::Ast::Identifier *astNode,
                                    llvm::Function *llvmFunc,
                                    Ast::Type *&resultType, TiObject *&resultCg, TiObject *&lastProcessedRef)
{
  PREPARE_SELF(generator, Generator);
  resultType = 0;
  resultCg = 0;
  lastProcessedRef = 0;
  generator->seeker->doForeach(astNode, astNode->getOwner(),
    [=](TiObject *obj)->Core::Data::Seeker::SeekVerb
    {
      // TODO: Check if the found obj is a variable definition.
      // TODO: Generate var reference if it's a variable.
      return Core::Data::Seeker::SeekVerb::MOVE;
    });
}


void Generator::_generateStringLiteral(TiObject *self, Core::Data::Ast::StringLiteral *astNode,
                                       llvm::Function *llvmFunc,
                                       Ast::Type *&resultType, TiObject *&resultCg, TiObject *&lastProcessedRef)
{
  // TODO:
}


//==============================================================================
// Helper Functions

Str const& Generator::getFunctionName(Spp::Ast::Function *astFunc)
{
  if (astFunc->getName()->getStr() == STR("")) {
    Str name = this->nodePathResolver->doResolve(astFunc);
    // Replace special characters with _.
    Str formattedName = std::regex_replace(name, std::regex("[^a-zA-Z0-9_]"), STR("_"));
    astFunc->setName(formattedName.c_str());
  }
  return astFunc->getName()->getStr();
}


Str Generator::getNewBlockName()
{
  static Int index = 0;
  return Str("block") + std::to_string(index++);
}

} } // namespace

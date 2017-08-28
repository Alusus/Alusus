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
    &this->prepareBlock,
    &this->generateBlock,
    &this->generateStatements,
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
  this->prepareBlock = &Generator::_prepareBlock;
  this->generateBlock = &Generator::_generateBlock;
  this->generateStatements = &Generator::_generateStatements;
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

Str Generator::generateIr(Core::Data::Ast::Scope *root, Core::Processing::ParserState *state)
{
  if (root == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("root"), STR("Must not be null."));
  }

  llvm::InitializeNativeTarget();

  SharedPtr<llvm::DataLayout> llvmDataLayout(new llvm::DataLayout(""));
  this->llvmModule = std::make_shared<llvm::Module>("AlususProgram", llvm::getGlobalContext());
  this->llvmModule->setDataLayout(llvmDataLayout->getStringRepresentation());
  this->executionContext = std::make_shared<ExecutionContext>(llvmDataLayout->getPointerSizeInBits());

  this->parserState = state;

  // Generates code for all modules.
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->get(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        this->generateModule(module);
      }
    }
  }

  Str out;
  llvm::raw_string_ostream ostream(out);
  llvm::PassManager passManager;
  passManager.add(llvm::createPrintModulePass(&ostream));
  passManager.run(*(this->llvmModule.get()));

  return out;
}


//==============================================================================
// Code Generation Functions

void Generator::_generateModule(TiObject *self, Spp::Ast::Module *astModule)
{
  PREPARE_SELF(generator, Generator);
  for (Int i = 0; i < astModule->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(astModule->get(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Spp::Ast::Module>()) {
        generator->generateModule(static_cast<Spp::Ast::Module*>(obj));
      } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
        generator->generateFunction(static_cast<Spp::Ast::Function*>(obj));
      }
      // TODO: Generate for types and global variables.
    }
  }
}


void Generator::_generateFunction(TiObject *self, Spp::Ast::Function *astFunc)
{
  PREPARE_SELF(generator, Generator);

  auto cgFunc = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::UserFunction>();
  if (cgFunc == 0) {
    generator->generateFunctionDecl(astFunc);
    cgFunc = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::UserFunction>();
  }
  if (cgFunc == 0) {
    throw EXCEPTION(GenericException, STR("Failed to generate function decleration."));
  }

  auto astArgs = astFunc->getArgTypes().get();
  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0) {
    // Prepare the block's llvm objects.
    generator->prepareBlock(astBlock, cgFunc->getLlvmFunction());
    auto cgBlock = astBlock->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();

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
    generator->generateStatements(astBlock, cgBlock->getIrBuilder(), cgFunc->getLlvmFunction());
  }
}


void Generator::_generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc)
{
  PREPARE_SELF(generator, Generator);

  auto genType = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Function>();
  if (genType != 0) return;

  // Construct the list of argument LLVM types.
  // TODO: Support functions that take no args.
  auto astArgs = astFunc->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  std::vector<llvm::Type*> llvmArgTypes;
  llvmArgTypes.reserve(argCount);
  for (Int i = 0; i < argCount; ++i) {
    auto argType = astArgs->get(i);
    if (argType->isDerivedFrom<Ast::ArgPack>()) break;
    llvmArgTypes.push_back(generator->typeGenerator->getGeneratedLlvmType(
      astArgs->get(i), generator->llvmModule.get()
    ));
  }

  // Get the return LLVM type.
  llvm::Type *llvmRetType = 0;
  if (astFunc->getRetType() != 0) {
    llvmRetType = generator->typeGenerator->getGeneratedLlvmType(
      astFunc->getRetType().get(), generator->llvmModule.get()
    );
  } else {
    llvmRetType = llvm::Type::getVoidTy(llvm::getGlobalContext());
  }

  // Generate the function object.
  auto llvmFuncType = llvm::FunctionType::get(llvmRetType, llvmArgTypes, astFunc->isVariadic());
  Str name = generator->getFunctionName(astFunc);
  auto cgFunc = std::make_shared<LlvmCodeGen::UserFunction>();
  cgFunc->setLlvmFunction(
    llvm::Function::Create(llvmFuncType, llvm::Function::ExternalLinkage, name, generator->llvmModule.get())
  );
  for (auto llvmArgType : llvmArgTypes) {
    cgFunc->addArg(llvmArgType);
  }
  astFunc->setExtra(META_EXTRA_NAME, cgFunc);
}


void Generator::_generateBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc)
{
  PREPARE_SELF(generator, Generator);
  generator->prepareBlock(astBlock, llvmFunc);
  auto cgBlock = astBlock->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();
  generator->generateStatements(astBlock, cgBlock->getIrBuilder(), llvmFunc);
}


void Generator::_prepareBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc)
{
  PREPARE_SELF(generator, Generator);

  auto genType = astBlock->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();
  if (genType != 0) return;

  auto cgBlock = std::make_shared<LlvmCodeGen::Block>();
  cgBlock->setLlvmBlock(llvm::BasicBlock::Create(llvm::getGlobalContext(), generator->getNewBlockName(), llvmFunc));
  cgBlock->setIrBuilder(new llvm::IRBuilder<>(cgBlock->getLlvmBlock()));
  astBlock->setExtra(META_EXTRA_NAME, cgBlock);
}


void Generator::_generateStatements(
  TiObject *self, Spp::Ast::Block *astBlock, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc
) {
  PREPARE_SELF(generator, Generator);
  for (Int i = 0; i < astBlock->getCount(); ++i) {
    auto astNode = astBlock->get(i);
    Ast::Type *resultType;
    llvm::Value *llvmResult;
    TiObject *lastProcessedRef;
    generator->generateStatement(astNode, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedRef);
  }
}


void Generator::_generateStatement(
  TiObject *self, TiObject *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
) {
  PREPARE_SELF(generator, Generator);
  if (astNode->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(astNode);
    generator->generateParamPass(paramPass, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedRef);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(astNode);
    generator->generateIdentifier(identifier, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedRef);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(astNode);
    generator->generateStringLiteral(stringLiteral, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedRef);
  }
  // TODO:
}


void Generator::_generateParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
) {
  PREPARE_SELF(generator, Generator);
  auto operand = astNode->getOperand().get();
  generator->generateStatement(operand, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedRef);
  if (resultType != 0) {
    // TODO: Check for function pointers and arrays.
  } else {
    // Prepare parameters list.
    using Core::Basic::TiObject;
    using Core::Basic::PlainList;
    PlainList<llvm::Value, TiObject> paramCgs;
    PlainList<TiObject, TiObject> paramTypes;
    auto param = astNode->getParam().get();
    if (param != 0) {
      if (param->isDerivedFrom<Core::Data::Ast::ExpressionList>()) {
        auto paramList = static_cast<Core::Data::Ast::ExpressionList*>(param);
        for (Int i = 0; i < paramList->getCount(); ++i) {
          llvm::Value *paramResultCg;
          Ast::Type *paramType;
          TiObject *paramLastProcessedRef;
          generator->generateStatement(
            paramList->get(i), llvmIrBuilder, llvmFunc, paramType, paramResultCg, paramLastProcessedRef
          );
          paramCgs.add(paramResultCg);
          paramTypes.add(paramType);
        }
      } else {
        llvm::Value *paramResultCg;
        Ast::Type *paramType;
        TiObject *paramLastProcessedRef;
        generator->generateStatement(param, llvmIrBuilder, llvmFunc, paramType, paramResultCg, paramLastProcessedRef);
        paramCgs.add(paramResultCg);
        paramTypes.add(paramType);
      }
    }
    // Look for a matching function to call.
    Ast::Function *function = 0;
    using CallMatchStatus = Ast::Function::CallMatchStatus;
    CallMatchStatus matchStatus = CallMatchStatus::NONE;
    generator->getSeeker()->doForeach(operand, astNode->getOwner(),
      [=, &paramTypes, &function, &matchStatus](TiObject *obj)->Core::Data::Seeker::SeekVerb
      {
        if (obj->isDerivedFrom<Ast::Function>()) {
          auto f = static_cast<Ast::Function*>(obj);
          CallMatchStatus ms;
          ms = f->matchCall(&paramTypes, generator->executionContext.get(), generator->getRootManager());
          if (ms != CallMatchStatus::NONE && matchStatus == CallMatchStatus::NONE) {
            function = f;
            matchStatus = ms;
          } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::CASTED) {
            function  = f;
            matchStatus = ms;
          } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::EXACT) {
            throw EXCEPTION(GenericException, STR("Multiple matches found for the same funciton call."));
          }
        }
        return Core::Data::Seeker::SeekVerb::MOVE;
      }
    );
    // Did we have a matched function to call?
    if (function == 0) {
      throw EXCEPTION(GenericException, STR("No matching callee found."));
    }
    // Build funcion declaration.
    generator->generateFunctionDecl(function);
    auto cgFunction = function->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Function>();
    // Create function call.
    std::vector<llvm::Value*> args;
    Ast::Function::ArgMatchContext context;
    for (Int i = 0; i < paramCgs.getCount(); ++i) {
      Ast::Type *srcType = static_cast<Ast::Type*>(paramTypes.get(i));
      auto status =
        function->matchNextArg(srcType, context, generator->executionContext.get(), generator->getRootManager());
      ASSERT(status != Ast::Function::CallMatchStatus::NONE);
      if (context.type == 0) {
        args.push_back(paramCgs.get(i));
      } else {
        auto destValue = generator->typeGenerator->createCast(
          srcType, context.type, paramCgs.get(i), llvmIrBuilder, generator->llvmModule.get()
        );
        args.push_back(destValue);
      }
    }
    auto llvmCall = cgFunction->createCallInstruction(llvmIrBuilder, args);
    // Assign results.
    llvmResult = llvmCall;
    resultType = function->traceRetType(generator->getSeeker());
    lastProcessedRef = astNode;
  }
}


void Generator::_generateIdentifier(
  TiObject *self, Core::Data::Ast::Identifier *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
) {
  PREPARE_SELF(generator, Generator);
  resultType = 0;
  llvmResult = 0;
  lastProcessedRef = 0;
  generator->getSeeker()->doForeach(astNode, astNode->getOwner(),
    [=](TiObject *obj)->Core::Data::Seeker::SeekVerb
    {
      // TODO: Check if the found obj is a variable definition.
      // TODO: Generate var reference if it's a variable.
      return Core::Data::Seeker::SeekVerb::MOVE;
    });
}


void Generator::_generateStringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
) {
  PREPARE_SELF(generator, Generator);

  auto value = &astNode->getValue().getStr();

  // Prepare char llvm type.
  if (generator->llvmCharType == 0) {
    auto charTypeRef = generator->rootManager->parseExpression(STR("Int[8]"));
    auto charAstType = generator->getSeeker()->doGet(charTypeRef.get(), generator->rootManager->getRootScope().get());
    generator->llvmCharType = generator->typeGenerator->getGeneratedLlvmType(charAstType, generator->llvmModule.get());
  }

  // Prepare the ast const string type.
  if (generator->astCharPtrType == 0) {
    auto astCharPtrTypeRef = generator->rootManager->parseExpression(STR("ptr[Int[8]]"))
      .s_cast<Core::Data::Ast::ParamPass>();
    astCharPtrTypeRef->setOwner(generator->rootManager->getRootScope().get());
    generator->astCharPtrType = ti_cast<Spp::Ast::Type>(generator->getSeeker()->doGet(
      astCharPtrTypeRef.get(), generator->rootManager->getRootScope().get()
    ));
    if (generator->astCharPtrType == 0) {
      throw EXCEPTION(GenericException, STR("Failed to get string literal AST type."));
    }
  }
  resultType = generator->astCharPtrType;

  // Fetch the llvm const string type.
  llvm::Type *llvmType = 0;
  Ast::Type *astType = 0;
  generator->getConstStringType(value->size() + 1, astType, llvmType);
  if (llvmType == 0) {
    throw EXCEPTION(GenericException, STR("Unexpected error."));
  }

  // Prepare the llvm constant array.
  std::vector<llvm::Constant *> llvmCharArray;
  llvmCharArray.reserve(value->size() + 1);
  for (Word i = 0; i < value->size(); i++) {
    llvmCharArray.push_back(llvm::ConstantInt::get(generator->llvmCharType, value->at(i)));
  }
  llvmCharArray.push_back(llvm::ConstantInt::get(generator->llvmCharType, 0));

  // Create an anonymous global variable.
  auto llvmStrConst = llvm::ConstantArray::get(static_cast<llvm::ArrayType*>(llvmType), llvmCharArray);
  auto llvmVar = new llvm::GlobalVariable(
    *generator->llvmModule, llvmType, true,
    llvm::GlobalValue::PrivateLinkage, llvmStrConst, generator->getAnonymouseVarName().c_str()
  );
  llvmVar->setAlignment(1);

  // Get a pointer to the global variable and return it as the value of the string.
  std::vector<llvm::Constant*> indices;
  llvm::ConstantInt *zero = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, 0));
  indices.push_back(zero);
  indices.push_back(zero);
  llvmResult = llvm::ConstantExpr::getGetElementPtr(llvmVar, indices);

  lastProcessedRef = astNode;
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
  return Str("__block") + std::to_string(this->blockIndex++);
}


Str Generator::getAnonymouseVarName()
{
  return Str("__anonymous") + std::to_string(this->anonymousVarIndex++);
}


void Generator::getConstStringType(Word size, Ast::Type *&astType, llvm::Type *&llvmType)
{
  // Prepare the reference.
  if (this->constStringTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << STR("array[Int[8],") << size << STR("]");
    this->constStringTypeRef = this->rootManager->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    this->constStringTypeRef->setOwner(this->rootManager->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = this->constStringTypeRef
      ->getParam().ti_cast_get<Core::Data::Ast::ExpressionList>()
      ->getShared(1).ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, STR("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  astType = ti_cast<Ast::Type>(
    this->getSeeker()->doGet(this->constStringTypeRef.get(), this->rootManager->getRootScope().get())
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, STR("Failed to get const string AST type."));
  }
  llvmType = this->typeGenerator->getGeneratedLlvmType(astType, this->llvmModule.get());
}

} } // namespace

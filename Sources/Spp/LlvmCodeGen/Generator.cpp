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
    &this->generateUserType,
    &this->prepareBlock,
    &this->generateBlock,
    &this->generateStatements,
    &this->generatePhrase
  });
}


void Generator::initBindings()
{
  this->generateModule = &Generator::_generateModule;
  this->generateFunction = &Generator::_generateFunction;
  this->generateFunctionDecl = &Generator::_generateFunctionDecl;
  this->generateUserType = &Generator::_generateUserType;
  this->prepareBlock = &Generator::_prepareBlock;
  this->generateBlock = &Generator::_generateBlock;
  this->generateStatements = &Generator::_generateStatements;
  this->generatePhrase = &Generator::_generatePhrase;
}


//==============================================================================
// Main Operation Functions

Bool Generator::build(
  Core::Data::Ast::Scope *root, Core::Processing::ParserState *state, Core::Basic::OutStream &out
) {
  Bool result = this->generateIr(root, state);

  llvm::raw_os_ostream ostream(out);
  llvm::PassManager passManager;
  passManager.add(llvm::createPrintModulePass(&ostream));
  passManager.run(*(this->llvmModule.get()));

  return result;
}


Bool Generator::execute(Core::Data::Ast::Scope *root, Core::Processing::ParserState *state, TiObject *entryRef)
{
  if (!this->generateIr(root, state)) return false;

  // Find the entry function.
  Ast::Function *function = 0;
  using CallMatchStatus = Ast::Function::CallMatchStatus;
  CallMatchStatus matchStatus = CallMatchStatus::NONE;
  Int matchCount = 0;
  SharedPtr<Core::Data::Notice> notice;
  this->getSeeker()->doForeach(entryRef, root,
    [=, &function, &matchStatus, &matchCount, &notice]
    (TiObject *obj, Core::Data::Notice *ntc)->Core::Data::Seeker::Verb
    {
      if (ntc != 0) {
        notice = getSharedPtr(ntc);
        return Core::Data::Seeker::Verb::MOVE;
      }

      if (obj != 0 && obj->isDerivedFrom<Ast::Function>()) {
        auto f = static_cast<Ast::Function*>(obj);
        CallMatchStatus ms;
        ms = f->matchCall(0, this->executionContext.get(), this->getRootManager());
        if (ms != CallMatchStatus::NONE && matchStatus == CallMatchStatus::NONE) {
          function = f;
          matchStatus = ms;
          matchCount = 1;
        } else if (ms == CallMatchStatus::CASTED && matchStatus == CallMatchStatus::CASTED) {
          matchCount++;
        } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::CASTED) {
          function  = f;
          matchStatus = ms;
          matchCount = 1;
        } else if (ms == CallMatchStatus::EXACT && matchStatus == CallMatchStatus::EXACT) {
          matchCount++;
        }
      }
      return Core::Data::Seeker::Verb::MOVE;
    }
  );
  // Did we have a matched function to call?
  if (notice != 0 && (matchCount > 1 || function == 0)) {
    state->addNotice(notice);
  }
  if (matchCount > 1) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(entryRef);
    state->addNotice(std::make_shared<MultipleCalleeMatchNotice>(metadata->getSourceLocation()));
    return false;
  } else if (function == 0) {
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(entryRef);
    state->addNotice(std::make_shared<NoCalleeMatchNotice>(metadata->getSourceLocation()));
    return false;
  }

  // Execute the IR
  auto ee = llvm::ExecutionEngine::createJIT(this->llvmModule.get());
  auto func = ee->FindFunctionNamed(this->getFunctionName(function).c_str());
  std::vector<llvm::GenericValue> args(0);
  ee->runFunction(func, args);

  return true;
}


Bool Generator::generateIr(Core::Data::Ast::Scope *root, Core::Processing::ParserState *state)
{
  if (root == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("root"), STR("Must not be null."));
  }

  llvm::InitializeNativeTarget();

  this->llvmDataLayout = std::make_shared<llvm::DataLayout>("");
  this->llvmModule = std::make_shared<llvm::Module>("AlususProgram", llvm::getGlobalContext());
  this->llvmModule->setDataLayout(this->llvmDataLayout->getStringRepresentation());
  this->executionContext = std::make_shared<ExecutionContext>(llvmDataLayout->getPointerSizeInBits());
  this->sourceLocationStack.clear();
  this->parserState = state;

  // Generates code for all modules.
  Bool result = true;
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->get(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        if (!this->generateModule(module)) result = false;
      }
    }
  }

  return result;
}


//==============================================================================
// Code Generation Functions

Bool Generator::_generateModule(TiObject *self, Spp::Ast::Module *astModule)
{
  PREPARE_SELF(generator, Generator);
  Bool result = true;
  for (Int i = 0; i < astModule->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(astModule->get(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Spp::Ast::Module>()) {
        if (!generator->generateModule(static_cast<Spp::Ast::Module*>(obj))) result = false;
      } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
        if (!generator->generateFunction(static_cast<Spp::Ast::Function*>(obj))) result = false;
      } else if (obj->isDerivedFrom<Spp::Ast::UserType>()) {
        if (!generator->generateUserType(static_cast<Spp::Ast::UserType*>(obj))) result = false;
      } else {
        // Generate global variable.
        if (!generator->variableGenerator->generateVarDefinition(def)) {
          result = false;
        }
      }
    }
  }
  return result;
}


Bool Generator::_generateFunction(TiObject *self, Spp::Ast::Function *astFunc)
{
  PREPARE_SELF(generator, Generator);

  auto llvmFuncBox = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Function*>>();
  if (llvmFuncBox == 0 || llvmFuncBox->get() == 0) {
    if (!generator->generateFunctionDecl(astFunc)) return false;
    llvmFuncBox = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Function*>>();
  }
  ASSERT(llvmFuncBox != 0);

  auto astArgs = astFunc->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0) {
    // Prepare the block's llvm objects.
    generator->prepareBlock(astBlock, llvmFuncBox->get());
    auto cgBlock = astBlock->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();

    // Create variables for each argument.
    auto i = 0;
    for (auto iter = llvmFuncBox->get()->arg_begin(); i != argCount; ++iter, ++i) {
      auto astArgType = astArgs->get(i);
      if (astArgType->isDerivedFrom<Ast::ArgPack>()) break;
      auto metadata = ti_cast<Core::Data::Ast::Metadata>(astArgType);
      auto cgVar = metadata->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Variable>();
      if (cgVar == 0 || cgVar->getAstType() == 0) {
        throw EXCEPTION(GenericException, STR("Unexpected error while generating function declaration."));
      }
      auto llvmTypeBox = cgVar->getAstType()->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
      if (llvmTypeBox == 0 || llvmTypeBox->get() == 0) {
        throw EXCEPTION(GenericException, STR("Unexpected error while generating function declaration."));
      }

      iter->setName(astArgs->getKey(i).c_str());
      auto llvmAlloca = cgBlock->getIrBuilder()->CreateAlloca(llvmTypeBox->get(), 0, astArgs->getKey(i).c_str());
      cgBlock->getIrBuilder()->CreateStore(iter, llvmAlloca);

      cgVar->setLlvmAllocaInst(llvmAlloca);
    }

    // Generate the function's statements.
    if (!generator->generateStatements(astBlock, cgBlock->getIrBuilder(), llvmFuncBox->get())) return false;

    // Add a void return, if necessary.
    if (generator->typeGenerator->isVoid(astFunc->getRetType().get())) {
      cgBlock->getIrBuilder()->CreateRetVoid();
    }
  }
  return true;
}


Bool Generator::_generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc)
{
  PREPARE_SELF(generator, Generator);

  auto llvmFuncBox = astFunc->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Function*>>();
  if (llvmFuncBox != 0 && llvmFuncBox->get() != 0) return true;

  // Construct the list of argument LLVM types.
  // TODO: Support functions that take no args.
  auto astArgs = astFunc->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  std::vector<llvm::Type*> llvmArgTypes;
  llvmArgTypes.reserve(argCount);
  for (Int i = 0; i < argCount; ++i) {
    auto argType = astArgs->get(i);
    if (argType->isDerivedFrom<Ast::ArgPack>()) break;
    llvm::Type *llvmType;
    Ast::Type *astType;
    if (!generator->typeGenerator->getGeneratedLlvmType(argType, llvmType, &astType)) {
      return false;
    }
    llvmArgTypes.push_back(llvmType);

    SharedPtr<Variable> cgVar = std::make_shared<Variable>();
    cgVar->setAstType(astType);
    auto metadata = ti_cast<Core::Data::Ast::Metadata>(argType);
    metadata->setExtra(META_EXTRA_NAME, cgVar);
  }

  // Get the return LLVM type.
  llvm::Type *llvmRetType = 0;
  if (astFunc->getRetType() != 0) {
    if (!generator->typeGenerator->getGeneratedLlvmType(
      astFunc->getRetType().get(), llvmRetType
    )) return false;
  } else {
    llvmRetType = llvm::Type::getVoidTy(llvm::getGlobalContext());
  }

  // Generate the function object.
  auto llvmFuncType = llvm::FunctionType::get(llvmRetType, llvmArgTypes, astFunc->isVariadic());
  Str name = generator->getFunctionName(astFunc);
  auto llvmFunc = llvm::Function::Create(
    llvmFuncType, llvm::Function::ExternalLinkage, name, generator->llvmModule.get()
  );

  // TODO: Do we need these attributes?
  // if (astFunc->getBody() == 0) {
  //   llvmFunc->addFnAttr(llvm::Attribute::NoCapture);
  //   llvmFunc->addFnAttr(llvm::Attribute::NoUnwind);
  // }

  astFunc->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Function*>::create(llvmFunc));
  return true;
}


Bool Generator::_generateUserType(TiObject *self, Spp::Ast::UserType *astType)
{
  PREPARE_SELF(generator, Generator);

  llvm::Type *llvmType;
  if (!generator->typeGenerator->getGeneratedLlvmType(astType, llvmType)) {
    return false;
  }
  ASSERT(llvmType != 0);
  llvm::StructType *llvmStructType = static_cast<llvm::StructType*>(llvmType);

  // Add struct members.
  auto body = astType->getBody().get();
  if (body == 0) {
    throw EXCEPTION(GenericException, STR("User type missing body block."));
  }
  Bool result = true;
  std::vector<llvm::Type*> structMembers;
  for (Int i = 0; i < body->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(body->get(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (Ast::isVarDefinition(obj)) {
        // Generate member variable.
        if (!generator->variableGenerator->generateVarDefinition(def)) {
          result = false;
          continue;
        }
        auto metadata = ti_cast<Core::Data::Ast::Metadata>(obj);
        ASSERT(metadata != 0);
        auto cgVar = metadata->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Variable>();
        ASSERT(cgVar != 0);
        cgVar->setLlvmStructIndex(structMembers.size());
        llvm::Type *memberLlvmType;
        auto memberAstType = cgVar->getAstType();
        if (!generator->typeGenerator->getGeneratedLlvmType(memberAstType, memberLlvmType)) {
          result = false;
          continue;
        }
        structMembers.push_back(memberLlvmType);
      }
      // TODO: Generate member functions.
      // TODO: Generate subtypes.
    }
  }
  llvmStructType->setBody(structMembers);
  return result;
}


Bool Generator::_generateBlock(TiObject *self, Spp::Ast::Block *astBlock, llvm::Function *llvmFunc)
{
  PREPARE_SELF(generator, Generator);
  generator->prepareBlock(astBlock, llvmFunc);
  auto cgBlock = astBlock->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();
  return generator->generateStatements(astBlock, cgBlock->getIrBuilder(), llvmFunc);
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


Bool Generator::_generateStatements(
  TiObject *self, Spp::Ast::Block *astBlock, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc
) {
  PREPARE_SELF(generator, Generator);
  Bool result = true;
  for (Int i = 0; i < astBlock->getCount(); ++i) {
    auto astNode = astBlock->get(i);
    Ast::Type *resultType;
    llvm::Value *llvmResult;
    TiObject *lastProcessedNode;
    if (!generator->generatePhrase(astNode, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode)) {
      result = false;
    }
  }
  return result;
}


Bool Generator::_generatePhrase(
  TiObject *self, TiObject *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(generator, Generator);
  if (astNode->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition*>(astNode);
    auto target = def->getTarget().get();
    if (target->isDerivedFrom<Spp::Ast::Module>()) {
      generator->parserState->addNotice(std::make_shared<InvalidOperationNotice>(def->getSourceLocation()));
      return false;
    } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
      // TODO: Generate function.
      generator->parserState->addNotice(std::make_shared<UnsupportedOperationNotice>(def->getSourceLocation()));
      return false;
    } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
      // TODO: Generate type.
      generator->parserState->addNotice(std::make_shared<UnsupportedOperationNotice>(def->getSourceLocation()));
      return false;
    } else {
      // Generate local variable.
      resultType = 0;
      llvmResult = 0;
      lastProcessedNode = 0;
      return generator->variableGenerator->generateVarDefinition(def);
    }
  } else if (astNode->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(astNode);
    return generator->expressionGenerator->generateParamPass(
      paramPass, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(astNode);
    return generator->variableGenerator->generateVarAccess(
      identifier, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  } else if (astNode->isDerivedFrom<Core::Data::Ast::AssignmentOperator>()) {
    auto assignmentOp = static_cast<Core::Data::Ast::AssignmentOperator*>(astNode);
    return generator->expressionGenerator->generateAssignment(
      assignmentOp, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  } else if (astNode->isDerivedFrom<Core::Data::Ast::InfixOperator>()) {
    auto infixOp = static_cast<Core::Data::Ast::InfixOperator*>(astNode);
    return generator->expressionGenerator->generateInfixOp(
      infixOp, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  } else if (astNode->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(astNode);
    return generator->literalGenerator->generateStringLiteral(
      stringLiteral, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  } else if (astNode->isDerivedFrom<Core::Data::Ast::IntegerLiteral>()) {
    auto integerLiteral = static_cast<Core::Data::Ast::IntegerLiteral*>(astNode);
    return generator->literalGenerator->generateIntegerLiteral(
      integerLiteral, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  } else if (astNode->isDerivedFrom<Core::Data::Ast::FloatLiteral>()) {
    auto floatLiteral = static_cast<Core::Data::Ast::FloatLiteral*>(astNode);
    return generator->literalGenerator->generateFloatLiteral(
      floatLiteral, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bracket>()) {
    auto bracket = static_cast<Core::Data::Ast::Bracket*>(astNode);
    if (bracket->getType() == Core::Data::Ast::BracketType::ROUND) {
      return generator->generatePhrase(
        bracket->getOperand().get(), llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
      );
    } else {
      generator->parserState->addNotice(std::make_shared<InvalidOperationNotice>(bracket->getSourceLocation()));
      return false;
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::IfStatement>()) {
    auto ifStatement = static_cast<Spp::Ast::IfStatement*>(astNode);
    return generator->commandGenerator->generateIfStatement(ifStatement, llvmIrBuilder, llvmFunc);
  } else if (astNode->isDerivedFrom<Spp::Ast::WhileStatement>()) {
    auto whileStatement = static_cast<Spp::Ast::WhileStatement*>(astNode);
    return generator->commandGenerator->generateWhileStatement(whileStatement, llvmIrBuilder, llvmFunc);
  } else if (astNode->isDerivedFrom<Spp::Ast::ReturnStatement>()) {
    auto returnStatement = static_cast<Spp::Ast::ReturnStatement*>(astNode);
    return generator->commandGenerator->generateReturn(
      returnStatement, llvmIrBuilder, llvmFunc, resultType, llvmResult, lastProcessedNode
    );
  }
  generator->parserState->addNotice(
    std::make_shared<UnsupportedOperationNotice>(Core::Data::Ast::getSourceLocation(astNode))
  );
  return false;
}


//==============================================================================
// Helper Functions

Str const& Generator::getFunctionName(Spp::Ast::Function *astFunc)
{
  if (astFunc->getName().getStr() == STR("")) {
    Str name = this->nodePathResolver->doResolve(astFunc);
    // Replace special characters with _.
    Str formattedName = std::regex_replace(name, std::regex("[^a-zA-Z0-9_]"), STR("_"));
    astFunc->setName(formattedName.c_str());
  }
  return astFunc->getName().getStr();
}


Str Generator::getNewBlockName()
{
  return Str("__block") + std::to_string(this->blockIndex++);
}

} } // namespace

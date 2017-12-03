/**
 * @file Spp/LlvmCodeGen/CommandGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::CommandGenerator.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp { namespace LlvmCodeGen
{

//==============================================================================
// Initialization Functions

void CommandGenerator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateReturn,
    &this->generateIfStatement,
    &this->generateWhileStatement
  });
}


void CommandGenerator::initBindings()
{
  this->generateReturn = &CommandGenerator::_generateReturn;
  this->generateIfStatement = &CommandGenerator::_generateIfStatement;
  this->generateWhileStatement = &CommandGenerator::_generateWhileStatement;
}


//==============================================================================
// Code Generation Functions

Bool CommandGenerator::_generateReturn(
  TiObject *self, Spp::Ast::ReturnStatement *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  // Get this function's return type.
  Ast::Function *function = Core::Data::findOwner<Ast::Function>(astNode);
  if (function == 0) {
    throw EXCEPTION(GenericException, STR("Return statement does not belong to a function."));
  }
  Ast::Type *retType = function->traceRetType(cmdGenerator->generator->getSeeker());

  auto operand = astNode->getOperand().get();
  if (operand != 0) {
    // Generate the expression to return.
    Ast::Type *operandType;
    llvm::Value *operandLlvmResult;
    TiObject *operandLastProcessedRef;
    if (!cmdGenerator->generator->generatePhrase(
      operand, llvmIrBuilder, llvmFunc, operandType, operandLlvmResult, operandLastProcessedRef
    )) return false;
    // TODO: Cast the returned value, if needed.
    if (!operandType->isImplicitlyCastableTo(
      retType,
      cmdGenerator->generator->getExecutionContext().get(),
      cmdGenerator->generator->getRootManager()
    )) {
      cmdGenerator->generator->getNoticeStore()->add(
        std::make_shared<InvalidReturnValueNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    llvm::Value *llvmReturnValue;
    if (!cmdGenerator->generator->getTypeGenerator()->generateCast(
      llvmIrBuilder, operandType, retType, operandLlvmResult, llvmReturnValue)
    ) {
      // This should not happen since non-castable calls should be filtered out earlier.
      throw EXCEPTION(GenericException, STR("Invalid cast was unexpectedly found."));
    }
    // Generate the return statement.
    llvmIrBuilder->CreateRet(llvmReturnValue);
  } else {
    // Make sure return type is void.
    if (!retType->isA<Ast::VoidType>()) {
      cmdGenerator->generator->getNoticeStore()->add(
        std::make_shared<InvalidReturnValueNotice>(astNode->findSourceLocation())
      );
      return false;
    }
    // Generate a void return statement.
    llvmIrBuilder->CreateRetVoid();
  }

  lastProcessedNode = astNode;
  llvmResult = 0;
  resultType = 0;
  return true;
}


Bool CommandGenerator::_generateIfStatement(
  TiObject *self, Spp::Ast::IfStatement *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  TiObject *lastProcessedNode;

  // Generate condition.
  Ast::Type *conditionAstType;
  llvm::Value *conditionLlvmValue;
  if (!cmdGenerator->generator->generatePhrase(
    astNode->getCondition().get(), llvmIrBuilder, llvmFunc, conditionAstType, conditionLlvmValue, lastProcessedNode
  )) return false;

  // Generate ifBody.
  LlvmCodeGen::Block *ifCgBlock;
  LlvmCodeGen::Block tmpIfCgBlock;
  auto ifBody = astNode->getIfBody().get();
  if (ifBody->isDerivedFrom<Ast::Block>()) {
    // The if body is already a block, so just build it.
    if (!cmdGenerator->generator->generateBlock(static_cast<Ast::Block*>(ifBody), llvmFunc)) {
      return false;
    }
    ifCgBlock = static_cast<Ast::Block*>(ifBody)->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();
    if (ifCgBlock == 0) {
      throw EXCEPTION(GenericException, STR("Unexpected error while generating if body."));
    }
  } else {
    // The if body is a single statement, so create a block to contain it.
    tmpIfCgBlock.setLlvmBlock(
      llvm::BasicBlock::Create(llvm::getGlobalContext(), cmdGenerator->generator->getNewBlockName(), llvmFunc)
    );
    tmpIfCgBlock.setIrBuilder(new llvm::IRBuilder<>(tmpIfCgBlock.getLlvmBlock()));
    ifCgBlock = &tmpIfCgBlock;
    Ast::Type *resultType;
    llvm::Value *llvmResult;
    if (!cmdGenerator->generator->generatePhrase(
      ifBody, tmpIfCgBlock.getIrBuilder(), llvmFunc, resultType, llvmResult, lastProcessedNode
    )) {
      return false;
    }
  }

  // Generate elseBody, if needed.
  LlvmCodeGen::Block *elseCgBlock = 0;
  LlvmCodeGen::Block tmpElseCgBlock;
  auto elseBody = astNode->getElseBody().get();
  if (elseBody != 0) {
    if (elseBody->isDerivedFrom<Ast::Block>()) {
      // The else body is already a block, so just build it.
      if (!cmdGenerator->generator->generateBlock(static_cast<Ast::Block*>(elseBody), llvmFunc)) {
        return false;
      }
      elseCgBlock = static_cast<Ast::Block*>(elseBody)->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();
      if (elseCgBlock == 0) {
        throw EXCEPTION(GenericException, STR("Unexpected error while generating else body."));
      }
    } else {
      // The if body is a single statement, so create a block to contain it.
      tmpElseCgBlock.setLlvmBlock(
        llvm::BasicBlock::Create(llvm::getGlobalContext(), cmdGenerator->generator->getNewBlockName(), llvmFunc)
      );
      tmpElseCgBlock.setIrBuilder(new llvm::IRBuilder<>(tmpElseCgBlock.getLlvmBlock()));
      elseCgBlock = &tmpElseCgBlock;
      Ast::Type *resultType;
      llvm::Value *llvmResult;
      if (!cmdGenerator->generator->generatePhrase(
        elseBody, tmpElseCgBlock.getIrBuilder(), llvmFunc, resultType, llvmResult, lastProcessedNode
      )) {
        return false;
      }
    }
  }

  // Create a merge block and jump to it from if and else bodies.
  auto mergeLlvmBlock = llvm::BasicBlock::Create(
    llvm::getGlobalContext(), cmdGenerator->generator->getNewBlockName(), llvmFunc
  );
  ifCgBlock->getIrBuilder()->CreateBr(mergeLlvmBlock);
  if (elseCgBlock != 0) {
    elseCgBlock->getIrBuilder()->CreateBr(mergeLlvmBlock);
  }

  // Create the if statement.
  llvmIrBuilder->CreateCondBr(
    conditionLlvmValue,
    ifCgBlock->getLlvmBlock(),
    elseCgBlock != 0 ? elseCgBlock->getLlvmBlock() : mergeLlvmBlock
  );

  // Set insert point to the merge body.
  llvmIrBuilder->SetInsertPoint(mergeLlvmBlock);

  return true;
}


Bool CommandGenerator::_generateWhileStatement(
  TiObject *self, Spp::Ast::WhileStatement *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc
) {
  PREPARE_SELF(cmdGenerator, CommandGenerator);

  TiObject *lastProcessedNode;

  // Create empty condition block.
  LlvmCodeGen::Block tmpCondCgBlock;
  tmpCondCgBlock.setLlvmBlock(
    llvm::BasicBlock::Create(llvm::getGlobalContext(), cmdGenerator->generator->getNewBlockName(), llvmFunc)
  );
  tmpCondCgBlock.setIrBuilder(new llvm::IRBuilder<>(tmpCondCgBlock.getLlvmBlock()));

  // Jump to condition block.
  llvmIrBuilder->CreateBr(tmpCondCgBlock.getLlvmBlock());

  // Generate body.
  LlvmCodeGen::Block *bodyCgBlock;
  LlvmCodeGen::Block tmpBodyCgBlock;
  auto body = astNode->getBody().get();
  if (body->isDerivedFrom<Ast::Block>()) {
    // The if body is already a block, so just build it.
    if (!cmdGenerator->generator->generateBlock(static_cast<Ast::Block*>(body), llvmFunc)) {
      return false;
    }
    bodyCgBlock = static_cast<Ast::Block*>(body)->getExtra(META_EXTRA_NAME).ti_cast_get<LlvmCodeGen::Block>();
    if (bodyCgBlock == 0) {
      throw EXCEPTION(GenericException, STR("Unexpected error while generating while body."));
    }
  } else {
    // The if body is a single statement, so create a block to contain it.
    tmpBodyCgBlock.setLlvmBlock(
      llvm::BasicBlock::Create(llvm::getGlobalContext(), cmdGenerator->generator->getNewBlockName(), llvmFunc)
    );
    tmpBodyCgBlock.setIrBuilder(new llvm::IRBuilder<>(tmpBodyCgBlock.getLlvmBlock()));
    bodyCgBlock = &tmpBodyCgBlock;
    Ast::Type *resultType;
    llvm::Value *llvmResult;
    if (!cmdGenerator->generator->generatePhrase(
      body, tmpBodyCgBlock.getIrBuilder(), llvmFunc, resultType, llvmResult, lastProcessedNode
    )) {
      return false;
    }
  }

  // Jump from body to condition block.
  bodyCgBlock->getIrBuilder()->CreateBr(tmpCondCgBlock.getLlvmBlock());

  // Create exit block.
  auto exitLlvmBlock = llvm::BasicBlock::Create(
    llvm::getGlobalContext(), cmdGenerator->generator->getNewBlockName(), llvmFunc
  );

  // Generate the condition.
  Ast::Type *conditionAstType;
  llvm::Value *conditionLlvmValue;
  if (!cmdGenerator->generator->generatePhrase(
    astNode->getCondition().get(), tmpCondCgBlock.getIrBuilder(), llvmFunc,
    conditionAstType, conditionLlvmValue, lastProcessedNode
  )) return false;

  // Create condition branch.
  tmpCondCgBlock.getIrBuilder()->CreateCondBr(conditionLlvmValue, bodyCgBlock->getLlvmBlock(), exitLlvmBlock);

  // Set insert point.
  llvmIrBuilder->SetInsertPoint(exitLlvmBlock);

  return true;
}

} } // namespace

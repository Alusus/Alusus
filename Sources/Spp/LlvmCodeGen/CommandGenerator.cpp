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
    &this->generateReturn
  });
}


void CommandGenerator::initBindings()
{
  this->generateReturn = &CommandGenerator::_generateReturn;
}


//==============================================================================
// Code Generation Functions

Bool CommandGenerator::_generateReturn(
  TiObject *self, Spp::Ast::ReturnStatement *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
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
      cmdGenerator->generator->getSourceLocationStack()->push_back(astNode->getSourceLocation());
      cmdGenerator->generator->getParserState()->addNotice(
        std::make_shared<InvalidReturnValueNotice>(cmdGenerator->generator->getSourceLocationStack())
      );
      cmdGenerator->generator->getSourceLocationStack()->pop_back();
      return false;
    }
    llvm::Value *llvmReturnValue;
    if (!cmdGenerator->generator->getTypeGenerator()->createCast(
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
      cmdGenerator->generator->getSourceLocationStack()->push_back(astNode->getSourceLocation());
      cmdGenerator->generator->getParserState()->addNotice(
        std::make_shared<InvalidReturnValueNotice>(cmdGenerator->generator->getSourceLocationStack())
      );
      cmdGenerator->generator->getSourceLocationStack()->pop_back();
      return false;
    }
    // Generate a void return statement.
    llvmIrBuilder->CreateRetVoid();
  }

  lastProcessedRef = astNode;
  llvmResult = 0;
  resultType = 0;
  return true;
}

} } // namespace

/**
 * @file Spp/LlvmCodeGen/LiteralGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::LiteralGenerator.
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

void LiteralGenerator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateStringLiteral
  });
}


void LiteralGenerator::initBindings()
{
  this->generateStringLiteral = &LiteralGenerator::_generateStringLiteral;
}


//==============================================================================
// Code Generation Functions

Bool LiteralGenerator::_generateStringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedRef
) {
  PREPARE_SELF(literalGenerator, LiteralGenerator);

  auto value = &astNode->getValue().getStr();

  // Prepare char llvm type.
  if (literalGenerator->llvmCharType == 0) {
    auto charTypeRef = literalGenerator->rootManager->parseExpression(STR("Int[8]"));
    auto charAstType = literalGenerator->getSeeker()->doGet(
      charTypeRef.get(), literalGenerator->rootManager->getRootScope().get()
    );
    if (!literalGenerator->typeGenerator->getGeneratedLlvmType(charAstType, literalGenerator->llvmCharType)) {
      throw EXCEPTION(GenericException, STR("Failed to generate char type'"));
    }
  }

  // Prepare the ast const string type.
  if (literalGenerator->astCharPtrType == 0) {
    auto astCharPtrTypeRef = literalGenerator->rootManager->parseExpression(STR("ptr[Int[8]]"))
      .s_cast<Core::Data::Ast::ParamPass>();
    astCharPtrTypeRef->setOwner(literalGenerator->rootManager->getRootScope().get());
    literalGenerator->astCharPtrType = ti_cast<Spp::Ast::Type>(literalGenerator->getSeeker()->doGet(
      astCharPtrTypeRef.get(), literalGenerator->rootManager->getRootScope().get()
    ));
    if (literalGenerator->astCharPtrType == 0) {
      throw EXCEPTION(GenericException, STR("Failed to get string literal AST type."));
    }
  }
  resultType = literalGenerator->astCharPtrType;

  // Fetch the llvm const string type.
  llvm::Type *llvmType = 0;
  Ast::Type *astType = 0;
  literalGenerator->getConstStringType(value->size() + 1, astType, llvmType);
  if (llvmType == 0) {
    throw EXCEPTION(GenericException, STR("Unexpected error."));
  }

  // Prepare the llvm constant array.
  std::vector<llvm::Constant *> llvmCharArray;
  llvmCharArray.reserve(value->size() + 1);
  for (Word i = 0; i < value->size(); i++) {
    llvmCharArray.push_back(llvm::ConstantInt::get(literalGenerator->llvmCharType, value->at(i)));
  }
  llvmCharArray.push_back(llvm::ConstantInt::get(literalGenerator->llvmCharType, 0));

  // Create an anonymous global variable.
  auto llvmStrConst = llvm::ConstantArray::get(static_cast<llvm::ArrayType*>(llvmType), llvmCharArray);
  auto llvmVar = new llvm::GlobalVariable(
    *literalGenerator->llvmModule, llvmType, true,
    llvm::GlobalValue::PrivateLinkage, llvmStrConst, literalGenerator->getAnonymouseVarName().c_str()
  );
  llvmVar->setAlignment(1);

  // Get a pointer to the global variable and return it as the value of the string.
  std::vector<llvm::Constant*> indices;
  llvm::ConstantInt *zero = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, 0));
  indices.push_back(zero);
  indices.push_back(zero);
  llvmResult = llvm::ConstantExpr::getGetElementPtr(llvmVar, indices);

  lastProcessedRef = astNode;
  return true;
}


//==============================================================================
// Helper Functions

Str LiteralGenerator::getAnonymouseVarName()
{
  return Str("__anonymous") + std::to_string(this->anonymousVarIndex++);
}


void LiteralGenerator::getConstStringType(Word size, Ast::Type *&astType, llvm::Type *&llvmType)
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
  if (!this->typeGenerator->getGeneratedLlvmType(astType, llvmType)) {
    throw EXCEPTION(GenericException, STR("Failed to generate const string LLVM type."));
  }
}

} } // namespace

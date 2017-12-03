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
    &this->generateStringLiteral,
    &this->generateIntegerLiteral,
    &this->generateFloatLiteral
  });
}


void LiteralGenerator::initBindings()
{
  this->generateStringLiteral = &LiteralGenerator::_generateStringLiteral;
  this->generateIntegerLiteral = &LiteralGenerator::_generateIntegerLiteral;
  this->generateFloatLiteral = &LiteralGenerator::_generateFloatLiteral;
}


//==============================================================================
// Code Generation Functions

Bool LiteralGenerator::_generateStringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(literalGenerator, LiteralGenerator);

  auto value = &astNode->getValue().getStr();

  // Prepare char llvm type.
  if (literalGenerator->llvmCharType == 0) {
    auto charTypeRef = literalGenerator->generator->getRootManager()->parseExpression(STR("Int[8]"));
    auto charAstType = literalGenerator->generator->getSeeker()->doGet(
      charTypeRef.get(), literalGenerator->generator->getRootManager()->getRootScope().get()
    );
    if (!literalGenerator->generator->getTypeGenerator()->getGeneratedLlvmType(
      charAstType, literalGenerator->llvmCharType)
    ) {
      throw EXCEPTION(GenericException, STR("Failed to generate char type."));
    }
  }

  // Prepare the ast const string type.
  if (literalGenerator->astCharPtrType == 0) {
    auto astCharPtrTypeRef = literalGenerator->generator->getRootManager()->parseExpression(STR("ptr[Int[8]]"))
      .s_cast<Core::Data::Ast::ParamPass>();
    astCharPtrTypeRef->setOwner(literalGenerator->generator->getRootManager()->getRootScope().get());
    literalGenerator->astCharPtrType = ti_cast<Spp::Ast::Type>(literalGenerator->generator->getSeeker()->doGet(
      astCharPtrTypeRef.get(), literalGenerator->generator->getRootManager()->getRootScope().get()
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
    *(literalGenerator->generator->getLlvmModule().get()), llvmType, true,
    llvm::GlobalValue::PrivateLinkage, llvmStrConst, literalGenerator->getAnonymouseVarName().c_str()
  );
  llvmVar->setAlignment(1);

  // Get a pointer to the global variable and return it as the value of the string.
  std::vector<llvm::Constant*> indices;
  llvm::ConstantInt *zero = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, 0));
  indices.push_back(zero);
  indices.push_back(zero);
  llvmResult = llvm::ConstantExpr::getGetElementPtr(llvmVar, indices);

  lastProcessedNode = astNode;
  return true;
}


Bool LiteralGenerator::_generateIntegerLiteral(
  TiObject *self, Core::Data::Ast::IntegerLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(literalGenerator, LiteralGenerator);

  auto src = astNode->getValue().get();

  // TODO: Consider non-English letters prefixes and postfixes in the literal.
  // TODO: Implement unsigned integers.

  // Parse the given value.
  Int base = 10;
  if (compareStr(src, STR("0b"), 2) == 0 || compareStr(src, STR("0B"), 2) == 0) {
    base = 2;
    src += 2;
  } else if (compareStr(src, STR("0o"), 2) == 0 || compareStr(src, STR("0O"), 2) == 0) {
    base = 8;
    src += 2;
  } else if (compareStr(src, STR("0h"), 2) == 0 || compareStr(src, STR("0H"), 2) == 0) {
    base = 16;
    src += 2;
  }
  Long value = 0;
  while (
    (*src >= CHR('0') && *src <= CHR('9')) ||
    (*src >= CHR('a') && *src <= CHR('f')) ||
    (*src >= CHR('A') && *src <= CHR('F'))
  ) {
    Int digit = 0;
    if (*src >= CHR('0') && *src <= CHR('9')) digit = *src - CHR('0');
    else if (*src >= CHR('a') && *src <= CHR('f')) digit = *src - CHR('a') + 10;
    else if (*src >= CHR('A') && *src <= CHR('F')) digit = *src - CHR('A') + 10;
    ASSERT(digit < base);
    value *= base;
    value += digit;
    ++src;
  }

  // Is it a signed number?
  // Bool signedNum = true;
  if (*src == CHR('u') || *src == CHR('U')) {
    // signedNum = false;
    ++src;
  }

  // Determine integer size.
  Int size = 32;
  if (*src == CHR('i') || *src == CHR('I')) {
    ++src;
    size = std::stoi(src);
  }

  // Generate the literal.
  llvm::Type *llvmType = 0;
  Ast::Type *astType = 0;
  auto sourceLocation = astNode->findSourceLocation().get();
  literalGenerator->generator->getNoticeStore()->pushPrefixSourceLocation(sourceLocation);
  Bool result = literalGenerator->getIntegerType(size, astType, llvmType);
  literalGenerator->generator->getNoticeStore()->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!result) return false;
  llvmResult = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(size, value, true));
  resultType = astType;
  lastProcessedNode = astNode;
  return true;
}


Bool LiteralGenerator::_generateFloatLiteral(
  TiObject *self, Core::Data::Ast::FloatLiteral *astNode, llvm::IRBuilder<> *llvmIrBuilder, llvm::Function *llvmFunc,
  Ast::Type *&resultType, llvm::Value *&llvmResult, TiObject *&lastProcessedNode
) {
  PREPARE_SELF(literalGenerator, LiteralGenerator);

  auto src = astNode->getValue().get();

  // TODO: Consider non-English letters prefixes and postfixes in the literal.
  // TODO: Consider the different float sizes.

  // Parse the given value.
  std::size_t numSize;
  Double value = std::stof(src, &numSize);
  src += numSize;

  // Determine float size.
  Int size = 32;
  if (*src == CHR('f') || *src == CHR('F')) {
    ++src;
    size = std::stoi(src);
  }

  // Generate the literal.
  llvm::Type *llvmType = 0;
  Ast::Type *astType = 0;
  auto sourceLocation = astNode->findSourceLocation().get();
  literalGenerator->generator->getNoticeStore()->pushPrefixSourceLocation(sourceLocation);
  Bool result = literalGenerator->getFloatType(size, astType, llvmType);
  literalGenerator->generator->getNoticeStore()->popPrefixSourceLocation(
    Core::Data::getSourceLocationRecordCount(sourceLocation)
  );
  if (!result) return false;
  if (size == 32) {
    llvmResult = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat((Float)value));
  } else {
    llvmResult = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(value));
  }
  resultType = astType;
  lastProcessedNode = astNode;
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
    this->constStringTypeRef = this->generator->getRootManager()->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    this->constStringTypeRef->setOwner(this->generator->getRootManager()->getRootScope().get());
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
    this->generator->getSeeker()->doGet(
      this->constStringTypeRef.get(), this->generator->getRootManager()->getRootScope().get()
    )
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, STR("Failed to get const string AST type."));
  }
  if (!this->generator->getTypeGenerator()->getGeneratedLlvmType(astType, llvmType)) {
    throw EXCEPTION(GenericException, STR("Failed to generate const string LLVM type."));
  }
}


Bool LiteralGenerator::getIntegerType(Word size, Ast::Type *&astType, llvm::Type *&llvmType)
{
  // Prepare the reference.
  if (this->integerTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << STR("Int[") << size << STR("]");
    this->integerTypeRef = this->generator->getRootManager()->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    this->integerTypeRef->setOwner(this->generator->getRootManager()->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto intLiteral = this->integerTypeRef
      ->getParam().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (!intLiteral) {
      throw EXCEPTION(GenericException, STR("Unexpected internal error."));
    }
    intLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  astType = ti_cast<Ast::Type>(
    this->generator->getSeeker()->doGet(
      this->integerTypeRef.get(), this->generator->getRootManager()->getRootScope().get()
    )
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, STR("Failed to get integer AST type."));
  }
  return this->generator->getTypeGenerator()->getGeneratedLlvmType(astType, llvmType);
}


Bool LiteralGenerator::getFloatType(Word size, Ast::Type *&astType, llvm::Type *&llvmType)
{
  // Prepare the reference.
  if (this->floatTypeRef == 0) {
    // Create a new reference.
    StrStream stream;
    stream << STR("Float[") << size << STR("]");
    this->floatTypeRef = this->generator->getRootManager()->parseExpression(stream.str().c_str())
      .s_cast<Core::Data::Ast::ParamPass>();
    this->floatTypeRef->setOwner(this->generator->getRootManager()->getRootScope().get());
  } else {
    // Recycle the existing reference.
    auto floatLiteral = this->floatTypeRef
      ->getParam().ti_cast_get<Core::Data::Ast::IntegerLiteral>();
    if (floatLiteral == 0) {
      throw EXCEPTION(GenericException, STR("Unexpected internal error."));
    }
    floatLiteral->setValue(std::to_string(size).c_str());
  }
  // Generate the llvm type.
  astType = ti_cast<Ast::Type>(
    this->generator->getSeeker()->doGet(
      this->floatTypeRef.get(), this->generator->getRootManager()->getRootScope().get()
    )
  );
  if (astType == 0) {
    throw EXCEPTION(GenericException, STR("Failed to get integer AST type."));
  }
  return this->generator->getTypeGenerator()->getGeneratedLlvmType(astType, llvmType);
}

} } // namespace

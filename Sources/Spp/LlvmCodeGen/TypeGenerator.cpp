/**
 * @file Spp/LlvmCodeGen/TypeGenerator.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::TypeGenerator.
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

void TypeGenerator::initBindingCaches()
{
  Core::Basic::initBindingCaches(this, {
    &this->generateType,
    &this->generateIntegerType,
    &this->generateFloatType,
    &this->generatePointerType
    // &this->generateArrayType,
    // &this->generateStructType,
  });
}


void TypeGenerator::initBindings()
{
  this->generateType = &TypeGenerator::_generateType;
  this->generateIntegerType = &TypeGenerator::_generateIntegerType;
  this->generateFloatType = &TypeGenerator::_generateFloatType;
  this->generatePointerType = &TypeGenerator::_generatePointerType;
  // this->generateArrayType = &TypeGenerator::_generateArrayType;
  // this->generateStructType = &TypeGenerator::_generateStructType;
}


//==============================================================================
// Main Operation Functions

Spp::Ast::Type* TypeGenerator::getGeneratedType(TiObject *ref, llvm::Module *llvmModule)
{
  Spp::Ast::Type *type = ti_cast<Spp::Ast::Type>(ref);
  if (type == 0) {
    this->seeker->doForeach(ref, ref,
      [=,&type](TiObject *obj)->Core::Data::Seeker::SeekVerb
      {
        type = ti_cast<Spp::Ast::Type>(obj);
        if (type != 0) {
          return Core::Data::Seeker::SeekVerb::STOP;
        }
        // TODO: Support template defaults.
        // TODO: Handle aliases.
        return Core::Data::Seeker::SeekVerb::MOVE;
      });
  }
  if (type == 0) {
    throw EXCEPTION(GenericException, STR("AST Type is not found."));
  }
  this->generateType(type, llvmModule);
  return type;
}


llvm::Type* TypeGenerator::getGeneratedLlvmType(TiObject *ref, llvm::Module *llvmModule)
{
  auto type = this->getGeneratedType(ref, llvmModule);
  auto llvmTypeBox = type->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
  if (llvmTypeBox == 0 || llvmTypeBox->get() == 0) {
    throw EXCEPTION(GenericException, STR("AST Type is missing the generated Type object."));
  }
  return llvmTypeBox->get();
}


//==============================================================================
// Code Generation Functions

void TypeGenerator::_generateType(TiObject *self, Spp::Ast::Type *astType, llvm::Module *llvmModule)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto llvmTypeBox = astType->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
  if (llvmTypeBox != 0) return;

  if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    typeGenerator->generateIntegerType(static_cast<Spp::Ast::IntegerType*>(astType), llvmModule);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    typeGenerator->generateFloatType(static_cast<Spp::Ast::FloatType*>(astType), llvmModule);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    typeGenerator->generatePointerType(static_cast<Spp::Ast::PointerType*>(astType), llvmModule);
  // } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
  //   typeGenerator->generateStructType(static_cast<Spp::Ast::ArrayType*>(astType), llvmModule);
  // } else if (astType->isDerivedFrom<Spp::Ast::StructType>()) {
  //   typeGenerator->generateStructType(static_cast<Spp::Ast::StructType*>(astType), llvmModule);
  } else {
    throw EXCEPTION(GenericException, STR("Unknown data type"));
  }
}


void TypeGenerator::_generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType, llvm::Module *llvmModule)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->seeker);
  auto llvmType = llvm::Type::getIntNTy(llvm::getGlobalContext(), bitCount);
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
}


void TypeGenerator::_generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, llvm::Module *llvmModule)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->seeker);
  llvm::Type *llvmType;
  switch (bitCount) {
    case 32:
      llvmType = llvm::Type::getFloatTy(llvm::getGlobalContext());
      break;
    case 64:
      llvmType = llvm::Type::getDoubleTy(llvm::getGlobalContext());
      break;
    case 128:
      llvmType = llvm::Type::getFP128Ty(llvm::getGlobalContext());
      break;
    default:
      throw EXCEPTION(GenericException, STR("Unsupported float bit count. Bit count should be either 32, 64, or 128."));
  }
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
}


void TypeGenerator::_generatePointerType(TiObject *self, Spp::Ast::PointerType *astType, llvm::Module *llvmModule)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->seeker);
  auto contentLlvmType = typeGenerator->getGeneratedLlvmType(contentAstType, llvmModule);
  auto llvmType = contentLlvmType->getPointerTo();
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
}

} } // namespace

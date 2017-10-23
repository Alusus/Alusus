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
    &this->generateVoidType,
    &this->generateIntegerType,
    &this->generateFloatType,
    &this->generatePointerType,
    &this->generateArrayType,
    // &this->generateStructType,
    &this->createCast
  });
}


void TypeGenerator::initBindings()
{
  this->generateType = &TypeGenerator::_generateType;
  this->generateVoidType = &TypeGenerator::_generateVoidType;
  this->generateIntegerType = &TypeGenerator::_generateIntegerType;
  this->generateFloatType = &TypeGenerator::_generateFloatType;
  this->generatePointerType = &TypeGenerator::_generatePointerType;
  this->generateArrayType = &TypeGenerator::_generateArrayType;
  // this->generateStructType = &TypeGenerator::_generateStructType;
  this->createCast = &TypeGenerator::_createCast;
}


//==============================================================================
// Main Operation Functions

Bool TypeGenerator::getGeneratedType(TiObject *ref, Spp::Ast::Type *&type)
{
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(ref);
  if (metadata == 0) {
    throw EXCEPTION(GenericException, STR("Reference does not contain metadata."));
  }

  type = this->traceAstType(ref);
  if (type == 0) return false;

  this->sourceLocationStack->push_back(metadata->getSourceLocation());
  Bool result = this->generateType(type);
  this->sourceLocationStack->pop_back();

  return result;
}


Bool TypeGenerator::getGeneratedLlvmType(TiObject *ref, llvm::Type *&type)
{
  Spp::Ast::Type *astType;
  if (!this->getGeneratedType(ref, astType)) {
    return false;
  }
  auto llvmTypeBox = astType->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
  if (llvmTypeBox == 0 || llvmTypeBox->get() == 0) {
    throw EXCEPTION(GenericException, STR("AST Type is missing the generated Type object."));
  }
  type = llvmTypeBox->get();
  return true;
}


Bool TypeGenerator::isVoid(TiObject *ref)
{
  if (ref == 0) return true;
  auto type = this->traceAstType(ref);
  if (!type) return false;
  return type->isA<Spp::Ast::VoidType>();
}


Spp::Ast::Type* TypeGenerator::traceAstType(TiObject *ref)
{
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(ref);
  if (metadata == 0) {
    throw EXCEPTION(GenericException, STR("Reference does not contain metadata."));
  }

  SharedPtr<Core::Data::Notice> notice;
  auto type = ti_cast<Spp::Ast::Type>(ref);
  if (type == 0) {
    this->getSeeker()->doForeach(ref, ref,
      [=, &type, &notice](TiObject *obj, Core::Data::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          notice = getSharedPtr(ntc);
          return Core::Data::Seeker::Verb::MOVE;
        }

        type = ti_cast<Spp::Ast::Type>(obj);
        if (type != 0) {
          return Core::Data::Seeker::Verb::STOP;
        }
        // TODO: Support template defaults.
        // TODO: Handle aliases.
        return Core::Data::Seeker::Verb::MOVE;
      }
    );
  }
  if (type == 0) {
    if (notice != 0) {
      notice->prependSourceLocationStack(this->sourceLocationStack);
      this->parserState->addNotice(notice);
    }
    this->sourceLocationStack->push_back(metadata->getSourceLocation());
    this->parserState->addNotice(std::make_shared<InvalidTypeNotice>(this->sourceLocationStack));
    this->sourceLocationStack->pop_back();
    return 0;
  }

  return type;
}


//==============================================================================
// Code Generation Functions

Bool TypeGenerator::_generateType(TiObject *self, Spp::Ast::Type *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto llvmTypeBox = astType->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
  if (llvmTypeBox != 0) return true;

  if (astType->isDerivedFrom<Spp::Ast::VoidType>()) {
    return typeGenerator->generateVoidType(static_cast<Spp::Ast::VoidType*>(astType));
  } else if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    return typeGenerator->generateIntegerType(static_cast<Spp::Ast::IntegerType*>(astType));
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    return typeGenerator->generateFloatType(static_cast<Spp::Ast::FloatType*>(astType));
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    return typeGenerator->generatePointerType(static_cast<Spp::Ast::PointerType*>(astType));
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    return typeGenerator->generateArrayType(static_cast<Spp::Ast::ArrayType*>(astType));
  // } else if (astType->isDerivedFrom<Spp::Ast::StructType>()) {
  //   return typeGenerator->generateStructType(static_cast<Spp::Ast::StructType*>(astType));
  } else {
    typeGenerator->parserState->addNotice(std::make_shared<InvalidTypeNotice>(typeGenerator->sourceLocationStack));
    return false;
  }
}


Bool TypeGenerator::_generateVoidType(TiObject *self, Spp::Ast::VoidType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto llvmType = llvm::Type::getVoidTy(llvm::getGlobalContext());
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->rootManager);
  auto llvmType = llvm::Type::getIntNTy(llvm::getGlobalContext(), bitCount);
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generateFloatType(TiObject *self, Spp::Ast::FloatType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->rootManager);
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
      typeGenerator->parserState->addNotice(
        std::make_shared<InvalidFloatBitCountNotice>(typeGenerator->sourceLocationStack)
      );
      return false;
  }
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generatePointerType(TiObject *self, Spp::Ast::PointerType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->rootManager);
  llvm::Type *contentLlvmType;
  if (!typeGenerator->getGeneratedLlvmType(contentAstType, contentLlvmType)) return false;
  auto llvmType = contentLlvmType->getPointerTo();
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->rootManager);
  auto contentSize = astType->getSize(typeGenerator->rootManager);
  llvm::Type *contentLlvmType;
  if (!typeGenerator->getGeneratedLlvmType(contentAstType, contentLlvmType)) return false;
  auto llvmType = llvm::ArrayType::get(contentLlvmType, contentSize);
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_createCast(
  TiObject *self, llvm::IRBuilder<> *llvmIrBuilder, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
  llvm::Value *llvmValue, llvm::Value *&llvmCastedValue
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  if (srcType == targetType) {
    // Same type, return value as is.
    llvmCastedValue = llvmValue;
    return true;
  } else if (srcType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Casting from integer.
    auto srcIntegerType = static_cast<Spp::Ast::IntegerType*>(srcType);
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from integer to another integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      llvm::Type *targetIntegerLlvmType;
      if (!typeGenerator->getGeneratedLlvmType(targetIntegerType, targetIntegerLlvmType)) return false;
      llvmCastedValue = llvmIrBuilder->CreateIntCast(llvmValue, targetIntegerLlvmType, true);
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from integer to float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      llvm::Type *targetFloatLlvmType;
      if (!typeGenerator->getGeneratedLlvmType(targetFloatType, targetFloatLlvmType)) return false;
      llvmCastedValue = llvmIrBuilder->CreateSIToFP(llvmValue, targetFloatLlvmType);
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast from integer to pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      Word targetBitCount = typeGenerator->llvmDataLayout->getPointerSizeInBits();
      Word srcBitCount = srcIntegerType->getBitCount(typeGenerator->rootManager);
      llvm::Type *targetPointerLlvmType;
      if (!typeGenerator->getGeneratedLlvmType(targetPointerType, targetPointerLlvmType)) return false;
      if (srcBitCount == targetBitCount) {
        llvmCastedValue = llvmIrBuilder->CreateBitCast(llvmValue, targetPointerLlvmType);
        return true;
      }
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Casting from float.
    auto srcFloatType = static_cast<Spp::Ast::FloatType*>(srcType);
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from float to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      llvm::Type *targetIntegerLlvmType;
      if (!typeGenerator->getGeneratedLlvmType(targetIntegerType, targetIntegerLlvmType)) return false;
      llvmCastedValue = llvmIrBuilder->CreateFPToSI(llvmValue, targetIntegerLlvmType);
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from float to another float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      Word srcBitCount = srcFloatType->getBitCount(typeGenerator->rootManager);
      Word targetBitCount = targetFloatType->getBitCount(typeGenerator->rootManager);
      llvm::Type *targetFloatLlvmType;
      if (!typeGenerator->getGeneratedLlvmType(targetFloatType, targetFloatLlvmType)) return false;
      if (srcBitCount > targetBitCount) {
        llvmCastedValue = llvmIrBuilder->CreateFPTrunc(llvmValue, targetFloatLlvmType);
        return true;
      } else {
        llvmCastedValue = llvmIrBuilder->CreateFPExt(llvmValue, targetFloatLlvmType);
        return true;
      }
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Casting from pointer.
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast pointer to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      Word srcBitCount = typeGenerator->llvmDataLayout->getPointerSizeInBits();
      Word targetBitCount = targetIntegerType->getBitCount(typeGenerator->rootManager);
      if (srcBitCount == targetBitCount) {
        llvm::Type *targetIntegerLlvmType;
        if (!typeGenerator->getGeneratedLlvmType(targetIntegerType, targetIntegerLlvmType)) return false;
        llvmCastedValue = llvmIrBuilder->CreateBitCast(llvmValue, targetIntegerLlvmType);
        return true;
      }
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast pointer to another pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      llvm::Type *targetPointerLlvmType;
      if (!typeGenerator->getGeneratedLlvmType(targetPointerType, targetPointerLlvmType)) return false;
      llvmCastedValue = llvmIrBuilder->CreateBitCast(llvmValue, targetPointerLlvmType);
      return true;
    }
  }
  typeGenerator->parserState->addNotice(std::make_shared<InvalidCastNotice>(typeGenerator->sourceLocationStack));
  return false;
}

} } // namespace

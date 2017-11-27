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
    &this->generateUserType,
    &this->generateCast,
    &this->generateDefaultValue
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
  this->generateUserType = &TypeGenerator::_generateUserType;
  this->generateCast = &TypeGenerator::_generateCast;
  this->generateDefaultValue = &TypeGenerator::_generateDefaultValue;
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

  if (metadata->getSourceLocation().filename != 0) {
    this->generator->getSourceLocationStack()->push_back(metadata->getSourceLocation());
  }
  Bool result = this->generateType(type);
  if (metadata->getSourceLocation().filename != 0) {
    this->generator->getSourceLocationStack()->pop_back();
  }

  return result;
}


Bool TypeGenerator::getGeneratedLlvmType(TiObject *ref, llvm::Type *&llvmTypeResult, Ast::Type **astTypeResult)
{
  Spp::Ast::Type *astType;
  if (!this->getGeneratedType(ref, astType)) {
    return false;
  }
  auto llvmTypeBox = astType->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
  if (llvmTypeBox == 0 || llvmTypeBox->get() == 0) {
    throw EXCEPTION(GenericException, STR("AST Type is missing the generated Type object."));
  }
  llvmTypeResult = llvmTypeBox->get();
  if (astTypeResult != 0) {
    *astTypeResult = astType;
  }
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
    this->generator->getSeeker()->doForeach(ref, ref,
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
      notice->prependSourceLocationStack(this->generator->getSourceLocationStack());
      this->generator->getParserState()->addNotice(notice);
    }
    if (metadata->getSourceLocation().filename != 0) {
      this->generator->getSourceLocationStack()->push_back(metadata->getSourceLocation());
      this->generator->getParserState()->addNotice(
        std::make_shared<InvalidTypeNotice>(this->generator->getSourceLocationStack())
      );
      this->generator->getSourceLocationStack()->pop_back();
    } else {
      this->generator->getParserState()->addNotice(
        std::make_shared<InvalidTypeNotice>(this->generator->getSourceLocationStack())
      );
    }
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
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    return typeGenerator->generateUserType(static_cast<Spp::Ast::UserType*>(astType));
  } else {
    typeGenerator->generator->getParserState()->addNotice(
      std::make_shared<InvalidTypeNotice>(typeGenerator->generator->getSourceLocationStack())
    );
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
  auto bitCount = astType->getBitCount(typeGenerator->generator->getRootManager());
  // TODO: Support 128 bits?
  if (bitCount != 1 && bitCount != 8 && bitCount != 16 && bitCount != 32 && bitCount != 64) {
    typeGenerator->generator->getParserState()->addNotice(
      std::make_shared<InvalidIntegerBitCountNotice>(typeGenerator->generator->getSourceLocationStack())
    );
    return false;
  }
  auto llvmType = llvm::Type::getIntNTy(llvm::getGlobalContext(), bitCount);
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generateFloatType(TiObject *self, Spp::Ast::FloatType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->generator->getRootManager());
  llvm::Type *llvmType;
  switch (bitCount) {
    case 32:
      llvmType = llvm::Type::getFloatTy(llvm::getGlobalContext());
      break;
    case 64:
      llvmType = llvm::Type::getDoubleTy(llvm::getGlobalContext());
      break;
    // TODO: Support 128 bits?
    // case 128:
    //   llvmType = llvm::Type::getFP128Ty(llvm::getGlobalContext());
    //   break;
    default:
      typeGenerator->generator->getParserState()->addNotice(
        std::make_shared<InvalidFloatBitCountNotice>(typeGenerator->generator->getSourceLocationStack())
      );
      return false;
  }
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generatePointerType(TiObject *self, Spp::Ast::PointerType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->generator->getRootManager());
  llvm::Type *contentLlvmType;
  if (!typeGenerator->getGeneratedLlvmType(contentAstType, contentLlvmType)) return false;
  auto llvmType = contentLlvmType->getPointerTo();
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->generator->getRootManager());
  auto contentSize = astType->getSize(typeGenerator->generator->getRootManager());
  llvm::Type *contentLlvmType;
  if (!typeGenerator->getGeneratedLlvmType(contentAstType, contentLlvmType)) return false;
  auto llvmType = llvm::ArrayType::get(contentLlvmType, contentSize);
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generateUserType(TiObject *self, Spp::Ast::UserType *astType)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  Str name = std::regex_replace(
    typeGenerator->generator->getNodePathResolver()->doResolve(astType), std::regex("[^a-zA-Z0-9_]"), STR("_")
  );
  auto llvmType = llvm::StructType::create(llvm::getGlobalContext(), name.c_str());
  astType->setExtra(META_EXTRA_NAME, Core::Basic::Box<llvm::Type*>::create(llvmType));
  return true;
}


Bool TypeGenerator::_generateCast(
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
      Word targetBitCount = typeGenerator->generator->getLlvmDataLayout()->getPointerSizeInBits();
      Word srcBitCount = srcIntegerType->getBitCount(typeGenerator->generator->getRootManager());
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
      Word srcBitCount = srcFloatType->getBitCount(typeGenerator->generator->getRootManager());
      Word targetBitCount = targetFloatType->getBitCount(typeGenerator->generator->getRootManager());
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
      Word srcBitCount = typeGenerator->generator->getLlvmDataLayout()->getPointerSizeInBits();
      Word targetBitCount = targetIntegerType->getBitCount(typeGenerator->generator->getRootManager());
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
  typeGenerator->generator->getParserState()->addNotice(
    std::make_shared<InvalidCastNotice>(typeGenerator->generator->getSourceLocationStack())
  );
  return false;
}


Bool TypeGenerator::_generateDefaultValue(TiObject *self, Spp::Ast::Type *astType, llvm::Constant *&result)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto llvmTypeBox = astType->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
  if (llvmTypeBox == 0) {
    if (!typeGenerator->generateType(astType)) return false;
    llvmTypeBox = astType->getExtra(META_EXTRA_NAME).ti_cast_get<Core::Basic::Box<llvm::Type*>>();
    ASSERT(llvmTypeBox != 0);
  }

  if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Generate integer 0
    auto integerType = static_cast<Spp::Ast::IntegerType*>(astType);
    auto bitCount = integerType->getBitCount(typeGenerator->generator->getRootManager());
    result = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(bitCount, 0, true));
    return true;
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Generate float 0
    auto floatType = static_cast<Spp::Ast::FloatType*>(astType);
    auto bitCount = floatType->getBitCount(typeGenerator->generator->getRootManager());
    if (bitCount == 32) {
      result = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat((Float)0));
    } else {
      result = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat((Double)0));
    }
    return true;
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Generate pointer null
    auto llvmPointerType = static_cast<llvm::PointerType*>(llvmTypeBox->get());
    result = llvm::ConstantPointerNull::get(llvmPointerType);
    return true;
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    throw EXCEPTION(GenericException, STR("Array default values are not implemented yet."));
  // } else if (astType->isDerivedFrom<Spp::Ast::StructType>()) {
  //   throw EXCEPTION(GenericException, STR("Struct default values are not implemented yet."));
  } else {
    throw EXCEPTION(GenericException, STR("Invlid type for generation of default value."));
  }
}

} } // namespace

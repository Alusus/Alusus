/**
 * @file Spp/CodeGen/TypeGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"
#include <regex>

namespace Spp { namespace CodeGen
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
    &this->generateDefaultValue,
    &this->getTypeAllocationSize
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
  this->getTypeAllocationSize = &TypeGenerator::_getTypeAllocationSize;
}


//==============================================================================
// Main Operation Functions

Bool TypeGenerator::getGeneratedType(
  TiObject *ref, TargetGeneration *tg, TiObject *&targetTypeResult, Ast::Type **astTypeResult
) {
  Spp::Ast::Type *astType;
  if (!this->_getGeneratedType(ref, tg, astType)) return false;
  targetTypeResult = getCodeGenData<TiObject>(astType);
  if (astTypeResult != 0) {
    *astTypeResult = astType;
  }
  return true;
}


Bool TypeGenerator::getGeneratedVoidType(
  TargetGeneration *tg, TiObject *&tgTypeResult, Ast::VoidType **astTypeResult
) {
  auto voidType = this->astHelper->getVoidType();
  Ast::Type *dummy;
  if (!this->_getGeneratedType(voidType, tg, dummy)) return false;
  tgTypeResult = getCodeGenData<TiObject>(voidType);
  if (astTypeResult != 0) {
    *astTypeResult = voidType;
  }
  return true;
}


Bool TypeGenerator::_getGeneratedType(TiObject *ref, TargetGeneration *tg, Spp::Ast::Type *&type)
{
  auto metadata = ti_cast<Core::Data::Ast::Metadata>(ref);
  if (metadata == 0) {
    throw EXCEPTION(GenericException, STR("Reference does not contain metadata."));
  }

  type = this->astHelper->traceType(ref);
  if (type == 0) return false;

  Core::Data::SourceLocation *sourceLocation = 0;
  if (metadata->findSourceLocation() != 0) {
    sourceLocation = metadata->findSourceLocation().get();
    this->noticeStore->pushPrefixSourceLocation(sourceLocation);
  }
  Bool result = this->generateType(type, tg);
  if (sourceLocation != 0) {
    this->noticeStore->popPrefixSourceLocation(
      Core::Data::getSourceLocationRecordCount(sourceLocation)
    );
  }

  return result;
}


//==============================================================================
// Code Generation Functions

Bool TypeGenerator::_generateType(TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto cgType = tryGetCodeGenData<TiObject>(astType);
  if (cgType != 0) return true;

  if (astType->isDerivedFrom<Spp::Ast::VoidType>()) {
    return typeGenerator->generateVoidType(static_cast<Spp::Ast::VoidType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    return typeGenerator->generateIntegerType(static_cast<Spp::Ast::IntegerType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    return typeGenerator->generateFloatType(static_cast<Spp::Ast::FloatType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    return typeGenerator->generatePointerType(static_cast<Spp::Ast::PointerType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    return typeGenerator->generateArrayType(static_cast<Spp::Ast::ArrayType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    return typeGenerator->generateUserType(static_cast<Spp::Ast::UserType*>(astType), tg);
  } else {
    typeGenerator->noticeStore->add(std::make_shared<Spp::Ast::InvalidTypeNotice>());
    return false;
  }
}


Bool TypeGenerator::_generateVoidType(TiObject *self, Spp::Ast::VoidType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  TioSharedPtr tgType;
  if (!tg->generateVoidType(tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->astHelper);
  // TODO: Support 128 bits?
  if (bitCount != 1 && bitCount != 8 && bitCount != 16 && bitCount != 32 && bitCount != 64) {
    typeGenerator->noticeStore->add(std::make_shared<InvalidIntegerBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!tg->generateIntType(bitCount, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->astHelper);
  // TODO: Support 128 bits?
  if (bitCount != 32 && bitCount != 64) {
    typeGenerator->noticeStore->add(std::make_shared<InvalidFloatBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!tg->generateFloatType(bitCount, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generatePointerType(TiObject *self, Spp::Ast::PointerType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (!typeGenerator->generateType(contentAstType, tg)) return false;
  TiObject *contentTgType = getCodeGenData<TiObject>(contentAstType);
  TioSharedPtr tgType;
  if (!tg->generatePointerType(contentTgType, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (!typeGenerator->generateType(contentAstType, tg)) return false;
  TiObject *contentTgType = getCodeGenData<TiObject>(contentAstType);
  auto size = astType->getSize(typeGenerator->astHelper);
  TioSharedPtr tgType;
  if (!tg->generateArrayType(contentTgType, size, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateUserType(TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  Str name = std::regex_replace(
    typeGenerator->astHelper->resolveNodePath(astType), std::regex("[^a-zA-Z0-9_]"), STR("_")
  );
  TioSharedPtr tgType;
  if (!tg->generateStructTypeDecl(name.c_str(), tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateCast(
    TiObject *self, TargetGeneration *tg, TiObject *tgContext, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
    TiObject *tgValue, TioSharedPtr &tgCastedValue
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  if (srcType == targetType) {
    // Same type, return value as is.
    tgCastedValue = getSharedPtr(tgValue);
    return true;
  } else if (srcType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Casting from integer.
    auto srcIntegerType = static_cast<Spp::Ast::IntegerType*>(srcType);
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from integer to another integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      auto srcTgType = getCodeGenData<TiObject>(srcIntegerType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastIntToInt(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from integer to float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      auto srcTgType = getCodeGenData<TiObject>(srcIntegerType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastIntToFloat(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast from integer to pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      Word srcBitCount = srcIntegerType->getBitCount(typeGenerator->astHelper);
      if (tg->getExecutionContext()->getPointerBitCount() == srcBitCount) {
        auto srcTgType = getCodeGenData<TiObject>(srcIntegerType);
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetPointerType, tg, targetTgType, 0)) return false;
        if (!tg->generateCastIntToPointer(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
        return true;
      }
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Casting from float.
    auto srcFloatType = static_cast<Spp::Ast::FloatType*>(srcType);
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from float to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      auto srcTgType = getCodeGenData<TiObject>(srcFloatType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastFloatToInt(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from float to another float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      auto srcTgType = getCodeGenData<TiObject>(srcFloatType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastFloatToFloat(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Casting from pointer.
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast pointer to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      Word targetBitCount = targetIntegerType->getBitCount(typeGenerator->astHelper);
      if (tg->getExecutionContext()->getPointerBitCount() == targetBitCount) {
        auto srcTgType = getCodeGenData<TiObject>(srcType);
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetIntegerType, tg, targetTgType, 0)) return false;
        if (!tg->generateCastPointerToInt(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
        return true;
      }
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast pointer to another pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      auto srcTgType = getCodeGenData<TiObject>(srcType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetPointerType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastPointerToPointer(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) {
        return false;
      }
      return true;
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::ReferenceType>()) {
    // Casting from reference.
    auto srcReferenceType = static_cast<Spp::Ast::ReferenceType*>(srcType);
    auto srcContentType = srcReferenceType->getContentType(typeGenerator->astHelper);
    auto tgContentType = getCodeGenData<TiObject>(srcContentType);
    TioSharedPtr tgDerefVal;
    if (!tg->generateDereference(tgContext, tgContentType, tgValue, tgDerefVal)) return false;
    return typeGenerator->generateCast(
      tg, tgContext, srcContentType, targetType, tgDerefVal.get(), tgCastedValue
    );
  }

  typeGenerator->noticeStore->add(std::make_shared<InvalidCastNotice>());
  return false;
}


Bool TypeGenerator::_generateDefaultValue(
  TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, TiObject *tgContext, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto tgType = tryGetCodeGenData<TiObject>(astType);
  if (tgType == 0) {
    if (!typeGenerator->generateType(astType, tg)) return false;
    tgType = getCodeGenData<TiObject>(astType);
  }

  if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Generate integer 0
    auto integerType = static_cast<Spp::Ast::IntegerType*>(astType);
    auto bitCount = integerType->getBitCount(typeGenerator->astHelper);
    return tg->generateIntLiteral(tgContext, bitCount, 0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Generate float 0
    auto floatType = static_cast<Spp::Ast::FloatType*>(astType);
    auto bitCount = floatType->getBitCount(typeGenerator->astHelper);
    return tg->generateFloatLiteral(tgContext, bitCount, (Double)0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Generate pointer null
    return tg->generateNullPtrLiteral(tgContext, tgType, result);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    throw EXCEPTION(GenericException, STR("Array default values are not implemented yet."));
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    throw EXCEPTION(GenericException, STR("Struct default values are not implemented yet."));
  } else {
    throw EXCEPTION(GenericException, STR("Invlid type for generation of default value."));
  }
}


Bool TypeGenerator::_getTypeAllocationSize(TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, Word &result)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  TiObject *tgType;
  if (!typeGenerator->getGeneratedType(astType, tg, tgType, 0)) return false;
  result = tg->getTypeAllocationSize(tgType);
  return true;
}

} } // namespace

/**
 * @file Spp/CodeGen/TypeGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
  Basic::initBindingCaches(this, {
    &this->generateType,
    &this->generateVoidType,
    &this->generateIntegerType,
    &this->generateFloatType,
    &this->generatePointerType,
    &this->generateArrayType,
    &this->generateUserType,
    &this->generateUserTypeMemberVars,
    &this->generateUserTypeAutoConstructor,
    &this->generateUserTypeAutoDestructor,
    &this->generateFunctionType,
    &this->generateCast,
    &this->generateDefaultValue,
    &this->generateDefaultArrayValue,
    &this->generateDefaultUserTypeValue,
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
  this->generateUserTypeMemberVars = &TypeGenerator::_generateUserTypeMemberVars;
  this->generateUserTypeAutoConstructor = &TypeGenerator::_generateUserTypeAutoConstructor;
  this->generateUserTypeAutoDestructor = &TypeGenerator::_generateUserTypeAutoDestructor;
  this->generateFunctionType = &TypeGenerator::_generateFunctionType;
  this->generateCast = &TypeGenerator::_generateCast;
  this->generateDefaultValue = &TypeGenerator::_generateDefaultValue;
  this->generateDefaultArrayValue = &TypeGenerator::_generateDefaultArrayValue;
  this->generateDefaultUserTypeValue = &TypeGenerator::_generateDefaultUserTypeValue;
  this->getTypeAllocationSize = &TypeGenerator::_getTypeAllocationSize;
}


//==============================================================================
// Main Operation Functions

Bool TypeGenerator::getGeneratedType(
  TiObject *ref, Generation *g, TargetGeneration *tg, TiObject *&targetTypeResult, Ast::Type **astTypeResult
) {
  Spp::Ast::Type *astType;
  if (!this->_getGeneratedType(ref, g, tg, astType)) return false;
  targetTypeResult = getCodeGenData<TiObject>(astType);
  if (astTypeResult != 0) {
    *astTypeResult = astType;
  }
  return true;
}


Bool TypeGenerator::getGeneratedVoidType(
  Generation *g, TargetGeneration *tg, TiObject *&tgTypeResult, Ast::VoidType **astTypeResult
) {
  auto voidType = this->astHelper->getVoidType();
  Ast::Type *dummy;
  if (!this->_getGeneratedType(voidType, g, tg, dummy)) return false;
  tgTypeResult = getCodeGenData<TiObject>(voidType);
  if (astTypeResult != 0) {
    *astTypeResult = voidType;
  }
  return true;
}


Bool TypeGenerator::_getGeneratedType(TiObject *ref, Generation *g, TargetGeneration *tg, Spp::Ast::Type *&type)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(ref);
  if (metadata == 0) {
    throw EXCEPTION(GenericException, S("Reference does not contain metadata."));
  }

  Bool shouldPushSl = ref->isDerivedFrom<Spp::Ast::Type>() ? false : true;

  type = this->astHelper->traceType(ref);
  if (type == 0) return false;

  Core::Data::SourceLocation *sourceLocation = 0;
  if (shouldPushSl && metadata->findSourceLocation() != 0) {
    sourceLocation = metadata->findSourceLocation().get();
    this->noticeStore->pushPrefixSourceLocation(sourceLocation);
  }
  Bool result = this->generateType(type, g, tg);
  if (shouldPushSl && sourceLocation != 0) {
    this->noticeStore->popPrefixSourceLocation(
      Core::Data::getSourceLocationRecordCount(sourceLocation)
    );
  }

  return result;
}


//==============================================================================
// Code Generation Functions

Bool TypeGenerator::_generateType(TiObject *self, Spp::Ast::Type *astType, Generation *g, TargetGeneration *tg)
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
    return typeGenerator->generatePointerType(static_cast<Spp::Ast::PointerType*>(astType), g, tg);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    return typeGenerator->generateArrayType(static_cast<Spp::Ast::ArrayType*>(astType), g, tg);
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    return typeGenerator->generateUserType(static_cast<Spp::Ast::UserType*>(astType), g, tg);
  } else if (astType->isDerivedFrom<Spp::Ast::FunctionType>()) {
    return typeGenerator->generateFunctionType(static_cast<Spp::Ast::FunctionType*>(astType), g, tg);
  } else {
    typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidTypeNotice>());
    return false;
  }
}


Bool TypeGenerator::_generateVoidType(TiObject *self, Spp::Ast::VoidType *astType, TargetGeneration *tg)
{
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
    typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidIntegerBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!tg->generateIntType(bitCount, astType->isSigned(), tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->astHelper);
  // TODO: Support 128 bits?
  if (bitCount != 32 && bitCount != 64) {
    typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidFloatBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!tg->generateFloatType(bitCount, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generatePointerType(TiObject *self, Spp::Ast::PointerType *astType, Generation *g, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (typeGenerator->astHelper->isVoid(contentAstType)) {
    contentAstType = typeGenerator->astHelper->getCharType();
  }
  if (!typeGenerator->generateType(contentAstType, g, tg)) return false;
  TiObject *contentTgType = getCodeGenData<TiObject>(contentAstType);
  TioSharedPtr tgType;
  if (!tg->generatePointerType(contentTgType, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (!typeGenerator->generateType(contentAstType, g, tg)) return false;
  TiObject *contentTgType = getCodeGenData<TiObject>(contentAstType);
  auto size = astType->getSize(typeGenerator->astHelper);
  TioSharedPtr tgType;
  if (!tg->generateArrayType(contentTgType, size, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateUserType(TiObject *self, Spp::Ast::UserType *astType, Generation *g, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  Str name = std::regex_replace(
    typeGenerator->astHelper->resolveNodePath(astType), std::regex("[^a-zA-Z0-9_]"), S("_")
  );
  TioSharedPtr tgType;
  if (!tg->generateStructTypeDecl(name.c_str(), tgType)) return false;
  setCodeGenData(astType, tgType);
  if (!typeGenerator->generateUserTypeMemberVars(astType, g, tg)) return false;
  if (!typeGenerator->generateUserTypeAutoConstructor(astType, g, tg)) return false;
  if (!typeGenerator->generateUserTypeAutoDestructor(astType, g, tg)) return false;
  return true;
}


Bool TypeGenerator::_generateUserTypeMemberVars(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, TargetGeneration *tg
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  TiObject *tgType = tryGetCodeGenData<TiObject>(astType);
  ASSERT(tgType != 0);

  // Prepare struct members.
  auto body = astType->getBody().get();
  if (body == 0) {
    throw EXCEPTION(GenericException, S("User type missing body block."));
  }
  auto prevInProgress = tryGetCodeGenData<TiBool>(body);
  if (prevInProgress != 0) {
    if (prevInProgress->get()) {
      typeGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::CircularUserTypeDefinitionsNotice>(astType->findSourceLocation())
      );
      return false;
    } else {
      return true;
    }
  }
  auto inProgress = TiBool::create(true);
  setCodeGenData(body, inProgress);

  // Generate the structure.
  Bool result = true;
  PlainList<TiObject> members;
  PlainMap<TiObject> tgMemberTypes;
  SharedList<TiObject> tgMembers;
  for (Int i = 0; i < body->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(body->getElement(i));
    if (def != 0) {
      if (typeGenerator->astHelper->getDefinitionDomain(def) != Ast::DefinitionDomain::GLOBAL) {
        auto obj = def->getTarget().get();
        if (typeGenerator->astHelper->isAstReference(obj)) {
          TiObject *tgType;
          Ast::Type *astMemberType;
          if (!typeGenerator->getGeneratedType(obj, g, tg, tgType, &astMemberType)) {
            result = false;
            continue;
          }
          Ast::setAstType(obj, astMemberType);
          tgMemberTypes.add(def->getName().get(), tgType);
          members.add(obj);
        } else if (obj->isDerivedFrom<Core::Data::Ast::Bridge>()) {
          if (!typeGenerator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(obj))) {
            result = false;
          }
          continue;
        }
      }
    }
  }
  if (!result) return false;

  if (!tg->generateStructTypeBody(tgType, &tgMemberTypes, &tgMembers)) return false;
  if (tgMemberTypes.getElementCount() != tgMembers.getCount()) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating struct body."));
  }
  for (Int i = 0; i < tgMemberTypes.getElementCount(); ++i) {
    setCodeGenData(members.get(i), tgMembers.get(i));
  }
  inProgress->set(false);

  return true;
}


Bool TypeGenerator::_generateUserTypeAutoConstructor(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, TargetGeneration *tg
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  // Validate that the type has custom initialization.
  if (!astType->hasCustomInitialization(typeGenerator->astHelper, tg->getExecutionContext())) return true;

  // Generate pointer type for the user type.
  auto astPtrType = typeGenerator->astHelper->getPointerTypeFor(astType);
  TiObject *tgPtrType;
  if (!typeGenerator->getGeneratedType(astPtrType, g, tg, tgPtrType, 0)) return false;

  // Generate function type.
  TioSharedPtr tgVoidType;
  if (!tg->generateVoidType(tgVoidType)) {
    throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
  }
  PlainMap<TiObject> argTypes;
  argTypes.add(S("this"), tgPtrType);
  TioSharedPtr tgFuncType;
  if (!tg->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
    throw EXCEPTION(GenericException, S("Failed to generate function type for type constructor."));
  }

  // Prepare the constructor function.
  auto funcName = typeGenerator->astHelper->resolveNodePath(astType) + S(".__autoConstruct__");
  TioSharedPtr tgFunc;
  if (!tg->generateFunctionDecl(funcName.c_str(), tgFuncType.get(), tgFunc)) {
    throw EXCEPTION(GenericException, S("Failed to generate function declaration for type constructor."));
  }
  TioSharedPtr tgContext;
  SharedList<TiObject> args;
  if (!tg->prepareFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext)) {
    throw EXCEPTION(GenericException, S("Failed to prepare function body for type constructor."));
  }

  // Prepare dependencies.
  DestructionStack destructionStack;
  GenDeps deps(tg, tgContext.get(), &destructionStack, 0, 0);
  deps.tgSelf = args.getElement(0);
  deps.astSelfType = astType;

  // Main loop.
  auto body = astType->getBody().get();
  for (Int i = 0; i < body->getCount(); ++i) {
    auto obj = body->getElement(i);
    auto def = ti_cast<Core::Data::Ast::Definition>(obj);
    if (def != 0) {
      auto target = def->getTarget().get();
      if (
        typeGenerator->getAstHelper()->isAstReference(target) &&
        typeGenerator->getAstHelper()->getDefinitionDomain(def) == Ast::DefinitionDomain::OBJECT
      ) {
        // Initialize member variable.
        if (!g->generateMemberVarInitialization(target, deps)) return false;
      }
    } else {
      // Generate regular statement.
      TerminalStatement terminal;
      if (!g->generateStatement(obj, deps, terminal)) return false;
    }
  }

  // Finish the function.
  if (!tg->finishFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext.get())) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for type constructor."));
  }

  // Assign the function to the type.
  setAutoCtor(astType, tgFunc);

  return true;
}


Bool TypeGenerator::_generateUserTypeAutoDestructor(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, TargetGeneration *tg
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  // Validate that the type has custom initialization.
  if (!astType->hasCustomDestruction(typeGenerator->astHelper, tg->getExecutionContext())) return true;

  // Generate pointer type for the user type.
  auto astPtrType = typeGenerator->astHelper->getPointerTypeFor(astType);
  TiObject *tgPtrType;
  if (!typeGenerator->getGeneratedType(astPtrType, g, tg, tgPtrType, 0)) return false;

  // Generate function type.
  TioSharedPtr tgVoidType;
  if (!tg->generateVoidType(tgVoidType)) {
    throw EXCEPTION(GenericException, S("Failed to generate LLVM void type."));
  }
  PlainMap<TiObject> argTypes;
  argTypes.add(S("this"), tgPtrType);
  TioSharedPtr tgFuncType;
  if (!tg->generateFunctionType(&argTypes, tgVoidType.get(), false, tgFuncType)) {
    throw EXCEPTION(GenericException, S("Failed to generate function type for type destructor."));
  }

  // Prepare the destructor function.
  auto funcName = typeGenerator->astHelper->resolveNodePath(astType) + S(".__autoDestruct__");
  TioSharedPtr tgFunc;
  if (!tg->generateFunctionDecl(funcName.c_str(), tgFuncType.get(), tgFunc)) {
    throw EXCEPTION(GenericException, S("Failed to generate function declaration for type destructor."));
  }
  TioSharedPtr tgContext;
  SharedList<TiObject> args;
  if (!tg->prepareFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext)) {
    throw EXCEPTION(GenericException, S("Failed to prepare function body for type destructor."));
  }

  // Prepare dependencies.
  DestructionStack destructionStack;
  GenDeps deps(tg, tgContext.get(), &destructionStack, 0, 0);
  deps.tgSelf = args.getElement(0);
  deps.astSelfType = astType;

  // Main loop.
  auto body = astType->getBody().get();
  for (Int i = 0; i < body->getCount(); ++i) {
    auto obj = body->getElement(i);
    auto def = ti_cast<Core::Data::Ast::Definition>(obj);
    if (def != 0) {
      auto target = def->getTarget().get();
      if (
        typeGenerator->getAstHelper()->isAstReference(target) &&
        typeGenerator->getAstHelper()->getDefinitionDomain(def) == Ast::DefinitionDomain::OBJECT
      ) {
        // Destruct member variable.
        if (!g->generateMemberVarDestruction(target, deps)) return false;
      }
    }
  }

  // Finish the function.
  if (!tg->finishFunctionBody(tgFunc.get(), tgFuncType.get(), &args, tgContext.get())) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for type destructor."));
  }

  // Assign the function to the type.
  setAutoDtor(astType, tgFunc);

  return true;
}


Bool TypeGenerator::_generateFunctionType(
  TiObject *self, Spp::Ast::FunctionType *astType, Generation *g, TargetGeneration *tg
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  // Construct the list of argument TG types.
  auto astArgs = astType->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  PlainMap<TiObject> tgArgs;
  for (Int i = 0; i < argCount; ++i) {
    auto argType = astArgs->getElement(i);
    if (argType->isDerivedFrom<Ast::ArgPack>()) break;
    TiObject *tgType;
    Ast::Type *astType;
    if (!typeGenerator->getGeneratedType(argType, g, tg, tgType, &astType)) {
      return false;
    }
    if (astType->hasCustomInitialization(typeGenerator->astHelper, tg->getExecutionContext())) {
      // This type has custom initialization, so we'll pass it by reference instead of by value.
      auto astPtrType = typeGenerator->astHelper->getPointerTypeFor(astType);
      if (!typeGenerator->getGeneratedType(astPtrType, g, tg, tgType, 0)) {
        return false;
      }
    }
    tgArgs.add(astArgs->getKey(i).c_str(), tgType);
    Ast::setAstType(argType, astType);
  }

  // Get the return TG type.
  TiObject *tgRetType = 0;
  if (astType->getRetType() != 0) {
    Ast::Type *astRetType;
    if (!typeGenerator->getGeneratedType(astType->getRetType().get(), g, tg, tgRetType, &astRetType)) {
      return false;
    }
    Ast::setAstType(astType->getRetType().get(), astRetType);
    if (astRetType->hasCustomInitialization(typeGenerator->astHelper, tg->getExecutionContext())) {
      // This type has custom initialization, so we'll pass it by reference instead of by value.
      auto astRetPtrType = typeGenerator->astHelper->getPointerTypeFor(astRetType);
      if (!typeGenerator->getGeneratedType(astRetPtrType, g, tg, tgRetType, 0)) {
        return false;
      }
      tgArgs.insert(0, S("#ret"), tgRetType);
      if (!typeGenerator->getGeneratedVoidType(g, tg, tgRetType, 0)) return false;
    }
  } else {
    if (!typeGenerator->getGeneratedVoidType(g, tg, tgRetType, 0)) return false;
  }

  // Generate the type.
  TioSharedPtr tgFuncType;
  if (!tg->generateFunctionType(&tgArgs, tgRetType, astType->isVariadic(), tgFuncType)) return false;
  setCodeGenData(astType, tgFuncType);
  return true;
}


Bool TypeGenerator::_generateCast(
    TiObject *self, Generation *g, GenDeps const &deps, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
    TiObject *tgValue, TioSharedPtr &tgCastedValue
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  if (srcType->isEqual(targetType, typeGenerator->astHelper, deps.tg->getExecutionContext())) {
    // Same type, return value as is.
    tgCastedValue = getSharedPtr(tgValue);
    return true;
  } else if (srcType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Casting from integer.
    auto srcIntegerType = static_cast<Spp::Ast::IntegerType*>(srcType);
    TiObject *srcTgType;
    if (!typeGenerator->getGeneratedType(srcIntegerType, g, deps.tg, srcTgType, 0)) return false;
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from integer to another integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, g, deps.tg, targetTgType, 0)) return false;
      if (!deps.tg->generateCastIntToInt(deps.tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from integer to float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, g, deps.tg, targetTgType, 0)) return false;
      if (!deps.tg->generateCastIntToFloat(deps.tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast from integer to pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      if (
        srcIntegerType->isNullLiteral() ||
        srcIntegerType->getBitCount(typeGenerator->astHelper) == deps.tg->getExecutionContext()->getPointerBitCount()
      ) {
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetPointerType, g, deps.tg, targetTgType, 0)) return false;
        if (!deps.tg->generateCastIntToPointer(deps.tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
        return true;
      }
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Casting from float.
    auto srcFloatType = static_cast<Spp::Ast::FloatType*>(srcType);
    TiObject *srcTgType;
    if (!typeGenerator->getGeneratedType(srcFloatType, g, deps.tg, srcTgType, 0)) return false;
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from float to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, g, deps.tg, targetTgType, 0)) return false;
      if (!deps.tg->generateCastFloatToInt(deps.tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from float to another float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, g, deps.tg, targetTgType, 0)) return false;
      if (!deps.tg->generateCastFloatToFloat(deps.tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Casting from pointer.
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast pointer to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      Word targetBitCount = targetIntegerType->getBitCount(typeGenerator->astHelper);
      if (deps.tg->getExecutionContext()->getPointerBitCount() == targetBitCount) {
        TiObject *srcTgType;
        if (!typeGenerator->getGeneratedType(srcType, g, deps.tg, srcTgType, 0)) return false;
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetIntegerType, g, deps.tg, targetTgType, 0)) return false;
        if (!deps.tg->generateCastPointerToInt(deps.tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
        return true;
      }
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast pointer to another pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      TiObject *srcTgType;
      if (!typeGenerator->getGeneratedType(srcType, g, deps.tg, srcTgType, 0)) return false;
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetPointerType, g, deps.tg, targetTgType, 0)) return false;
      if (!deps.tg->generateCastPointerToPointer(deps.tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) {
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
    if (!deps.tg->generateDereference(deps.tgContext, tgContentType, tgValue, tgDerefVal)) return false;
    return typeGenerator->generateCast(
      g, deps, srcContentType, targetType, tgDerefVal.get(), tgCastedValue
    );
  }

  typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidCastNotice>());
  return false;
}


Bool TypeGenerator::_generateDefaultValue(
  TiObject *self, Spp::Ast::Type *astType, Generation *g, GenDeps const &deps, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Generate integer 0
    auto tgType = tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, g, deps.tg)) return false;
      tgType = getCodeGenData<TiObject>(astType);
    }

    auto integerType = static_cast<Spp::Ast::IntegerType*>(astType);
    auto bitCount = integerType->getBitCount(typeGenerator->astHelper);
    return deps.tg->generateIntLiteral(deps.tgContext, bitCount, integerType->isSigned(), 0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Generate float 0
    auto tgType = tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, g, deps.tg)) return false;
      tgType = getCodeGenData<TiObject>(astType);
    }

    auto floatType = static_cast<Spp::Ast::FloatType*>(astType);
    auto bitCount = floatType->getBitCount(typeGenerator->astHelper);
    return deps.tg->generateFloatLiteral(deps.tgContext, bitCount, (Double)0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Generate pointer null
    auto tgType = tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, g, deps.tg)) return false;
      tgType = getCodeGenData<TiObject>(astType);
    }

    return deps.tg->generateNullPtrLiteral(deps.tgContext, tgType, result);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    // Generate zeroed out array.
    return typeGenerator->generateDefaultArrayValue(static_cast<Ast::ArrayType*>(astType), g, deps, result);
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    // Generate zeroed out structure.
    return typeGenerator->generateDefaultUserTypeValue(static_cast<Ast::UserType*>(astType), g, deps, result);
  } else {
    throw EXCEPTION(GenericException, S("Invlid type for generation of default value."));
  }
}


Bool TypeGenerator::_generateDefaultArrayValue(
  TiObject *self, Spp::Ast::ArrayType *astType, Generation *g, GenDeps const &deps, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto tgType = tryGetCodeGenData<TiObject>(astType);
  if (tgType == 0) {
    if (!typeGenerator->generateType(astType, g, deps.tg)) return false;
    tgType = getCodeGenData<TiObject>(astType);
  }

  auto elementAstType = astType->getContentType(typeGenerator->astHelper);
  auto elementTgType = tryGetCodeGenData<TiObject>(elementAstType);
  if (elementTgType == 0) {
    if (!typeGenerator->generateType(elementAstType, g, deps.tg)) return false;
    elementTgType = getCodeGenData<TiObject>(elementAstType);
  }

  TioSharedPtr elementVal;
  if (!typeGenerator->generateDefaultValue(elementAstType, g, deps, elementVal)) return false;

  auto size = astType->getSize(typeGenerator->astHelper);
  SharedList<TiObject> memberVals;
  for (Word i = 0; i < size; ++i) {
    memberVals.add(elementVal);
  }

  return deps.tg->generateArrayLiteral(deps.tgContext, tgType, &memberVals, result);
}


Bool TypeGenerator::_generateDefaultUserTypeValue(
  TiObject *self, Spp::Ast::UserType *astType, Generation *g, GenDeps const &deps, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto tgType = tryGetCodeGenData<TiObject>(astType);
  if (tgType == 0) {
    if (!typeGenerator->generateType(astType, g, deps.tg)) return false;
    tgType = getCodeGenData<TiObject>(astType);
  }

  auto body = astType->getBody().get();
  SharedList<TiObject> memberVals;
  PlainMap<TiObject> memberTypes;
  for (Int i = 0; i < body->getElementCount(); ++i) {
    auto def = ti_cast<Core::Data::Ast::Definition>(body->getElement(i));
    if (def != 0 && typeGenerator->getAstHelper()->getDefinitionDomain(def) == Ast::DefinitionDomain::OBJECT) {
      auto obj = def->getTarget().get();
      if (typeGenerator->getAstHelper()->isAstReference(obj)) {
        TiObject *tgMemberType;
        Ast::Type *astMemberType;
        if (!typeGenerator->getGeneratedType(obj, g, deps.tg, tgMemberType, &astMemberType)) return false;
        memberTypes.addElement(def->getName().get(), tgMemberType);
        TioSharedPtr memberVal;
        if (!typeGenerator->generateDefaultValue(astMemberType, g, deps, memberVal)) return false;
        memberVals.add(memberVal);
      }
    }
  }
  return deps.tg->generateStructLiteral(deps.tgContext, tgType, &memberTypes, &memberVals, result);
}


Bool TypeGenerator::_getTypeAllocationSize(
  TiObject *self, Spp::Ast::Type *astType, Generation *g, TargetGeneration *tg, Word &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  TiObject *tgType;
  if (!typeGenerator->getGeneratedType(astType, g, tg, tgType, 0)) return false;
  result = tg->getTypeAllocationSize(tgType);
  return true;
}

} } // namespace

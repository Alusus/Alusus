/**
 * @file Spp/CodeGen/Generator.cpp
 * Contains the implementation of class Spp::CodeGen::Generator.
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

void Generator::initBindings()
{
  auto generation = ti_cast<Generation>(this);

  generation->generateModules = &Generator::_generateModules;
  generation->generateModule = &Generator::_generateModule;
  generation->generateFunction = &Generator::_generateFunction;
  generation->generateFunctionDecl = &Generator::_generateFunctionDecl;
  generation->generateUserTypeBody = &Generator::_generateUserTypeBody;
  generation->generateVarDef = &Generator::_generateVarDef;
  generation->generateStatements = &Generator::_generateStatements;
  generation->generateStatement = &Generator::_generateStatement;
  generation->generateExpression = &Generator::_generateExpression;
  generation->generateCast = &Generator::_generateCast;
  generation->getGeneratedType = &Generator::_getGeneratedType;
  generation->getTypeAllocationSize = &Generator::_getTypeAllocationSize;
}


//==============================================================================
// Main Operation Functions

Bool Generator::generate(
  Core::Data::Ast::Scope *root, Core::Processing::ParserState *state, TargetGeneration *tg
) {
  VALIDATE_NOT_NULL(root, state, tg);

  this->noticeStore = state->getNoticeStore();
  this->noticeStore->clearPrefixSourceLocationStack();

  this->astHelper->prepare(this->noticeStore);
  this->typeGenerator->setNoticeStore(this->noticeStore);
  this->commandGenerator->setNoticeStore(this->noticeStore);
  this->expressionGenerator->setNoticeStore(this->noticeStore);

  auto generation = ti_cast<Generation>(this);
  return generation->generateModules(root, tg);
}


//==============================================================================
// Code Generation Functions

Bool Generator::_generateModules(TiObject *self, Core::Data::Ast::Scope *root, TargetGeneration *tg)
{
  PREPARE_SELF(generation, Generation);

  Bool result = true;
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->get(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        if (!generation->generateModule(module, tg)) result = false;
      }
    }
  }

  return result;
}


Bool Generator::_generateModule(TiObject *self, Spp::Ast::Module *astModule, TargetGeneration *tg)
{
  PREPARE_SELF(generation, Generation);
  Bool result = true;
  for (Int i = 0; i < astModule->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(astModule->get(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Spp::Ast::Module>()) {
        if (!generation->generateModule(static_cast<Spp::Ast::Module*>(obj), tg)) result = false;
      } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
        if (!generation->generateFunction(static_cast<Spp::Ast::Function*>(obj), tg)) result = false;
      } else if (obj->isDerivedFrom<Spp::Ast::UserType>()) {
        if (!generation->generateUserTypeBody(static_cast<Spp::Ast::UserType*>(obj), tg)) result = false;
      } else {
        // Generate global variable.
        if (!generation->generateVarDef(def, tg)) {
          result = false;
        }
      }
    }
  }
  return result;
}


Bool Generator::_generateFunction(TiObject *self, Spp::Ast::Function *astFunc, TargetGeneration *tg)
{
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(generator);

  auto tgFunc = tryGetCodeGenData<TiObject>(astFunc);
  if (tgFunc == 0) {
    if (!generation->generateFunctionDecl(astFunc, tg)) return false;
    tgFunc = getCodeGenData<TiObject>(astFunc);
  }

  auto astArgs = astFunc->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0) {
    // Prepare list of args.
    Core::Basic::PlainMap<TiObject, TiObject> tgArgs;
    for (Int i = 0; i < argCount; ++i) {
      auto argType = astArgs->get(i);
      if (argType->isDerivedFrom<Ast::ArgPack>()) break;
      auto argAstType = Ast::getAstType(argType);
      tgArgs.add(astArgs->getKey(i).c_str(), getCodeGenData<TiObject>(argAstType));
    }

    // Prepare return type.
    TiObject *tgRetType = 0;
    if (astFunc->getRetType() != 0) {
      auto astRetType = Ast::getAstType(astFunc->getRetType().get());
      tgRetType = getCodeGenData<TiObject>(astRetType);
    } else {
      if (!generator->typeGenerator->getGeneratedVoidType(tg, tgRetType, 0)) return false;
    }

    // Prepare the funciton body.
    Core::Basic::SharedList<TiObject, TiObject> tgVars;
    TioSharedPtr tgContext;
    if (!tg->prepareFunctionBody(tgFunc, &tgArgs, tgRetType, astFunc->isVariadic(), &tgVars, tgContext)) return false;
    if (tgArgs.getCount() != tgVars.getCount()) {
      throw EXCEPTION(GenericException, STR("Unexpected error in target generator during function body preparation."));
    }

    // Store the generated data.
    setCodeGenData(astBlock, tgContext);
    for (Int i = 0; i < tgVars.getCount(); ++i) {
      auto argType = astArgs->get(i);
      setCodeGenData(argType, tgVars.get(i));
    }

    // Generate the function's statements.
    auto retVal = generation->generateStatements(astBlock, tg, tgContext.get());

    // Finalize the body.
    if (!tg->finishFunctionBody(tgFunc, &tgArgs, tgRetType, astFunc->isVariadic(), &tgVars, tgContext.get())) {
      return false;
    }

    return retVal;
  }
  return true;
}


Bool Generator::_generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc, TargetGeneration *tg)
{
  PREPARE_SELF(generator, Generator);

  auto tgFunc = tryGetCodeGenData<TiObject>(astFunc);
  if (tgFunc != 0) return true;

  // Construct the list of argument TG types.
  // TODO: Support functions that take no args.
  auto astArgs = astFunc->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  Core::Basic::PlainMap<TiObject, TiObject> tgArgs;
  for (Int i = 0; i < argCount; ++i) {
    auto argType = astArgs->get(i);
    if (argType->isDerivedFrom<Ast::ArgPack>()) break;
    TiObject *tgType;
    Ast::Type *astType;
    if (!generator->typeGenerator->getGeneratedType(argType, tg, tgType, &astType)) {
      return false;
    }
    tgArgs.add(astArgs->getKey(i).c_str(), tgType);
    Ast::setAstType(argType, astType);
  }

  // Get the return LLVM type.
  TiObject *tgRetType = 0;
  if (astFunc->getRetType() != 0) {
    Ast::Type *astRetType;
    if (!generator->typeGenerator->getGeneratedType(astFunc->getRetType().get(), tg, tgRetType, &astRetType)) {
      return false;
    }
    Ast::setAstType(astFunc->getRetType().get(), astRetType);
  } else {
    if (!generator->typeGenerator->getGeneratedVoidType(tg, tgRetType, 0)) return false;
  }

  // Generate the function object.
  Str name = generator->astHelper->getFunctionName(astFunc);
  TioSharedPtr tgFuncResult;
  if (!tg->generateFunctionDecl(name.c_str(), &tgArgs, tgRetType, astFunc->isVariadic(), tgFuncResult)) return false;
  setCodeGenData(astFunc, tgFuncResult);

  // TODO: Do we need these attributes?
  // if (astFunc->getBody() == 0) {
  //   llvmFunc->addFnAttr(llvm::Attribute::NoCapture);
  //   llvmFunc->addFnAttr(llvm::Attribute::NoUnwind);
  // }

  return true;
}


Bool Generator::_generateUserTypeBody(TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(generator, Generator);

  TiObject *tgType;
  if (!generator->typeGenerator->getGeneratedType(astType, tg, tgType, 0)) return false;
  ASSERT(tgType != 0);

  // Prepare struct members.
  auto body = astType->getBody().get();
  if (body == 0) {
    throw EXCEPTION(GenericException, STR("User type missing body block."));
  }
  Bool result = true;
  Core::Basic::PlainList<TiObject, TiObject> members;
  Core::Basic::PlainMap<TiObject, TiObject> tgMemberTypes;
  Core::Basic::SharedList<TiObject, TiObject> tgMembers;
  for (Int i = 0; i < body->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(body->get(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (generator->getAstHelper()->isVarDefinition(obj)) {
        TiObject *tgType;
        Ast::Type *astMemberType;
        if (!generator->typeGenerator->getGeneratedType(obj, tg, tgType, &astMemberType)) {
          result = false;
          continue;
        }
        Ast::setAstType(obj, astMemberType);
        tgMemberTypes.add(def->getName().get(), tgType);
        members.add(obj);
      }
      // TODO: Generate member functions.
      // TODO: Generate subtypes.
    }
  }
  if (!result) return false;

  if (!tg->generateStructTypeBody(tgType, &tgMemberTypes, &tgMembers)) return false;
  if (tgMemberTypes.getElementCount() != tgMembers.getCount()) {
    throw EXCEPTION(GenericException, STR("Unexpected error while generating struct body."));
  }
  for (Int i = 0; i < tgMemberTypes.getElementCount(); ++i) {
    setCodeGenData(members.get(i), tgMembers.get(i));
  }

  return true;
}


Bool Generator::_generateVarDef(TiObject *self, Core::Data::Ast::Definition *definition, TargetGeneration *tg)
{
  PREPARE_SELF(generator, Generator);

  TiObject *astVar = definition->getTarget().get();
  TiObject *tgVar = tryGetCodeGenData<TiObject>(astVar);

  if (tgVar == 0) {
    // Generate the type of the variable.
    Ast::Type *astType;
    TiObject *tgType;
    if (!generator->typeGenerator->getGeneratedType(astVar, tg, tgType, &astType)) {
      return false;
    }

    Ast::setAstType(astVar, astType);

    auto astBlock = Core::Data::findOwner<Ast::Block>(definition);
    if (ti_cast<Ast::Module>(astBlock) != 0) {
      // Generate a global variable.
      // Generate global name.
      Str name = std::regex_replace(
        generator->getAstHelper()->resolveNodePath(definition), std::regex("[^a-zA-Z0-9_]"), STR("_")
      );
      // Generate the default value.
      TioSharedPtr tgDefaultValue;
      if (!generator->typeGenerator->generateDefaultValue(astType, tg, 0, tgDefaultValue)) return false;
      // Create the llvm global var.
      TioSharedPtr tgGlobalVar;
      if (!tg->generateGlobalVariable(tgType, name.c_str(), tgDefaultValue.get(), tgGlobalVar)) return false;
      setCodeGenData(astVar, tgGlobalVar);
    } else {
      if (ti_cast<Ast::Type>(astBlock->getOwner()) != 0) {
        // This should never happen.
        throw EXCEPTION(GenericException, STR("Unexpected error while generating variable."));
      } else {
        // Generate a local variable.
        // At this point we should already have a TG context.
        auto tgContext = getCodeGenData<TiObject>(astBlock);

        // TODO: Should we use default values with local variables?
        // TioSharedPtr tgDefaultValue;
        // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
        //   if (!generator->typeGenerator->generateDefaultValue(astType, tg, tgContext, tgDefaultValue)) return false;
        // }

        // Create the llvm global var.
        TioSharedPtr tgLocalVar;
        if (!tg->generateLocalVariable(
          tgContext, tgType, definition->getName().get(), 0, tgLocalVar
        )) return false;
        setCodeGenData(astVar, tgLocalVar);
      }
    }
  }

  return true;
}


Bool Generator::_generateStatements(
  TiObject *self, Spp::Ast::Block *astBlock, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(generation, Generation);
  Bool result = true;
  for (Int i = 0; i < astBlock->getCount(); ++i) {
    auto astNode = astBlock->get(i);
    if (!generation->generateStatement(astNode, tg, tgContext)) result = false;
  }
  return result;
}


Bool Generator::_generateStatement(
  TiObject *self, TiObject *astNode, TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(generator);

  if (astNode->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition*>(astNode);
    auto target = def->getTarget().get();
    if (target->isDerivedFrom<Spp::Ast::Module>()) {
      generator->noticeStore->add(std::make_shared<InvalidOperationNotice>(def->findSourceLocation()));
      return false;
    } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
      // TODO: Generate function.
      generator->noticeStore->add(std::make_shared<UnsupportedOperationNotice>(def->findSourceLocation()));
      return false;
    } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
      // TODO: Generate type.
      generator->noticeStore->add(std::make_shared<UnsupportedOperationNotice>(def->findSourceLocation()));
      return false;
    } else {
      // Generate local variable.
      return generation->generateVarDef(def, tg);
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::IfStatement>()) {
    auto ifStatement = static_cast<Spp::Ast::IfStatement*>(astNode);
    return generator->commandGenerator->generateIfStatement(ifStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::WhileStatement>()) {
    auto whileStatement = static_cast<Spp::Ast::WhileStatement*>(astNode);
    return generator->commandGenerator->generateWhileStatement(whileStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::ReturnStatement>()) {
    auto returnStatement = static_cast<Spp::Ast::ReturnStatement*>(astNode);
    return generator->commandGenerator->generateReturn(returnStatement, generation, tg, tgContext);
  } else {
    GenResult result;
    return generation->generateExpression(astNode, tg, tgContext, result);
  }
}


Bool Generator::_generateExpression(
  TiObject *self, TiObject *astNode, TargetGeneration *tg, TiObject *tgContext, GenResult &result
) {
  PREPARE_SELF(generator, Generator);
  return generator->expressionGenerator->generate(astNode, ti_cast<Generation>(self), tg, tgContext, result);
}


Bool Generator::_generateCast(
  TiObject *self, TargetGeneration *tg, TiObject *tgContext, Spp::Ast::Type *srcType, Spp::Ast::Type *destType,
  TiObject *tgValue, TioSharedPtr &tgCastedValue
) {
  PREPARE_SELF(generator, Generator);
  return generator->typeGenerator->generateCast(tg, tgContext, srcType, destType, tgValue, tgCastedValue);
}


Bool Generator::_getGeneratedType(
  TiObject *self, TiObject *ref, TargetGeneration *tg, TiObject *&targetTypeResult, Ast::Type **astTypeResult
) {
  PREPARE_SELF(generator, Generator);
  return generator->typeGenerator->getGeneratedType(ref, tg, targetTypeResult, astTypeResult);
}


Bool Generator::_getTypeAllocationSize(TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, Word &result)
{
  PREPARE_SELF(generator, Generator);
  return generator->typeGenerator->getTypeAllocationSize(astType, tg, result);
}

} } // namespace

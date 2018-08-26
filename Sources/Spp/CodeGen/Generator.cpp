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
    auto def = ti_cast<Data::Ast::Definition>(root->getElement(i));
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
  PREPARE_SELF(generator, Generator);
  Bool result = true;
  for (Int i = 0; i < astModule->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(astModule->getElement(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (obj->isDerivedFrom<Spp::Ast::Module>()) {
        if (!generation->generateModule(static_cast<Spp::Ast::Module*>(obj), tg)) result = false;
      } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
        if (!generation->generateFunction(static_cast<Spp::Ast::Function*>(obj), tg)) result = false;
      } else if (obj->isDerivedFrom<Spp::Ast::UserType>()) {
        if (!generation->generateUserTypeBody(static_cast<Spp::Ast::UserType*>(obj), tg)) result = false;
      } else if (generator->getAstHelper()->isAstReference(obj)) {
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

  auto astFuncType = astFunc->getType().get();
  auto tgFuncType = tryGetCodeGenData<TiObject>(astFuncType);
  ASSERT(tgFuncType != 0);

  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0) {
    auto astArgs = astFuncType->getArgTypes().get();

    // Prepare the funciton body.
    SharedList<TiObject> tgVars;
    TioSharedPtr tgContext;
    if (!tg->prepareFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext)) return false;

    // Store the generated data.
    setCodeGenData(astBlock, tgContext);
    for (Int i = 0; i < tgVars.getCount(); ++i) {
      auto argType = astArgs->getElement(i);
      setCodeGenData(argType, tgVars.get(i));
    }

    // Generate the function's statements.
    auto retVal = generation->generateStatements(astBlock, tg, tgContext.get());

    // Finalize the body.
    if (!tg->finishFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext.get())) {
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

  // Generate function type.
  TiObject *tgFunctionType;
  if (!generator->typeGenerator->getGeneratedType(astFunc->getType().get(), tg, tgFunctionType, 0)) {
    return false;
  }

  // Generate the function object.
  Str name = generator->astHelper->getFunctionName(astFunc);
  TioSharedPtr tgFuncResult;
  if (!tg->generateFunctionDecl(name.c_str(), tgFunctionType, tgFuncResult)) return false;
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
    throw EXCEPTION(GenericException, S("User type missing body block."));
  }
  auto prevInProgress = tryGetCodeGenData<TiBool>(body);
  if (prevInProgress != 0) {
    if (prevInProgress->get()) {
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::CircularUserTypeDefinitionsNotice>(astType->findSourceLocation())
      );
      return false;
    } else {
      return true;
    }
  }
  auto inProgress = TiBool::create(true);
  setCodeGenData(body, inProgress);

  Bool result = true;
  PlainList<TiObject> members;
  PlainMap<TiObject> tgMemberTypes;
  SharedList<TiObject> tgMembers;
  for (Int i = 0; i < body->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(body->getElement(i));
    if (def != 0) {
      auto obj = def->getTarget().get();
      if (generator->getAstHelper()->isAstReference(obj)) {
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
    throw EXCEPTION(GenericException, S("Unexpected error while generating struct body."));
  }
  for (Int i = 0; i < tgMemberTypes.getElementCount(); ++i) {
    setCodeGenData(members.get(i), tgMembers.get(i));
  }
  inProgress->set(false);

  return true;
}


Bool Generator::_generateVarDef(TiObject *self, Core::Data::Ast::Definition *definition, TargetGeneration *tg)
{
  PREPARE_SELF(generator, Generator);

  TiObject *astVar = definition->getTarget().get();
  TiObject *tgVar = tryGetCodeGenData<TiObject>(astVar);

  if (tgVar == 0) {
    // Have we previously tried to build this var?
    if (didCodeGenFail(astVar)) return false;

    // Generate the type of the variable.
    Ast::Type *astType;
    TiObject *tgType;
    if (!generator->typeGenerator->getGeneratedType(astVar, tg, tgType, &astType)) {
      setCodeGenFailed(astVar, true);
      return false;
    }

    // Also generate the reference type of this type.
    Ast::Type *astRefType = generator->astHelper->getPointerTypeFor(astType);
    if (astRefType == 0) {
      throw EXCEPTION(GenericException, S("Could not find reference type for the given var type."));
    }
    TiObject *tgRefType;
    if (!generator->typeGenerator->getGeneratedType(astRefType, tg, tgRefType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate pointer type for the given var type."));
    }

    if (astType->isDerivedFrom<Ast::UserType>()) {
      PREPARE_SELF(generation, Generation);
      if (!generation->generateUserTypeBody(static_cast<Ast::UserType*>(astType), tg)) {
        setCodeGenFailed(astVar, true);
        return false;
      }
    }

    Ast::setAstType(astVar, astType);

    auto astBlock = Core::Data::findOwner<Ast::Block>(definition);
    if (ti_cast<Ast::Module>(astBlock) != 0) {
      // Generate a global variable.
      // Generate global name.
      Str name = std::regex_replace(
        generator->getAstHelper()->resolveNodePath(definition), std::regex("[^a-zA-Z0-9_]"), S("_")
      );
      // Generate the default value.
      TioSharedPtr tgDefaultValue;
      if (!generator->typeGenerator->generateDefaultValue(astType, tg, 0, tgDefaultValue)) {
        setCodeGenFailed(astVar, true);
        return false;
      }
      // Create the llvm global var.
      TioSharedPtr tgGlobalVar;
      if (!tg->generateGlobalVariable(tgType, name.c_str(), tgDefaultValue.get(), tgGlobalVar)) {
        setCodeGenFailed(astVar, true);
        return false;
      }
      setCodeGenData(astVar, tgGlobalVar);
    } else {
      if (ti_cast<Ast::Type>(astBlock->getOwner()) != 0) {
        // This should never happen.
        throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));
      } else {
        // Generate a local variable.

        // To avoid stack overflows we need to allocate at the function level rather than any inner block.
        while (astBlock != 0 && ti_cast<Ast::Function>(astBlock->getOwner()) == 0) {
          astBlock = Core::Data::findOwner<Ast::Block>(astBlock->getOwner());
        }
        if (astBlock == 0) {
          throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));
        }

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
        )) {
          setCodeGenFailed(astVar, true);
          return false;
        }
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
    auto astNode = astBlock->getElement(i);
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
      generator->noticeStore->add(std::make_shared<Spp::Notices::InvalidOperationNotice>(def->findSourceLocation()));
      return false;
    } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
      // TODO: Generate function.
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
      );
      return false;
    } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
      // TODO: Generate type.
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
      );
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
  } else if (astNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
    auto forStatement = static_cast<Spp::Ast::ForStatement*>(astNode);
    return generator->commandGenerator->generateForStatement(forStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::ContinueStatement>()) {
    auto continueStatement = static_cast<Spp::Ast::ContinueStatement*>(astNode);
    return generator->commandGenerator->generateContinueStatement(continueStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::BreakStatement>()) {
    auto breakStatement = static_cast<Spp::Ast::BreakStatement*>(astNode);
    return generator->commandGenerator->generateBreakStatement(breakStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::ReturnStatement>()) {
    auto returnStatement = static_cast<Spp::Ast::ReturnStatement*>(astNode);
    return generator->commandGenerator->generateReturnStatement(returnStatement, generation, tg, tgContext);
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

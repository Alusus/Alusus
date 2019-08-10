/**
 * @file Spp/CodeGen/Generator.cpp
 * Contains the implementation of class Spp::CodeGen::Generator.
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

void Generator::initBindings()
{
  auto generation = ti_cast<Generation>(this);

  generation->generateModules = &Generator::_generateModules;
  generation->generateModule = &Generator::_generateModule;
  generation->generateFunction = &Generator::_generateFunction;
  generation->generateFunctionDecl = &Generator::_generateFunctionDecl;
  generation->generateUserTypeBody = &Generator::_generateUserTypeBody;
  generation->generateVarDef = &Generator::_generateVarDef;
  generation->generateTempVar = &Generator::_generateTempVar;
  generation->generateVarInitialization = &Generator::_generateVarInitialization;
  generation->generateStatements = &Generator::_generateStatements;
  generation->generateStatement = &Generator::_generateStatement;
  generation->generateExpression = &Generator::_generateExpression;
  generation->generateCast = &Generator::_generateCast;
  generation->getGeneratedType = &Generator::_getGeneratedType;
  generation->getTypeAllocationSize = &Generator::_getTypeAllocationSize;
}


//==============================================================================
// Main Operation Functions

void Generator::prepareBuild(Core::Notices::Store *noticeStore, Bool offlineExecution)
{
  VALIDATE_NOT_NULL(noticeStore);

  this->noticeStore = noticeStore;
  this->noticeStore->clearPrefixSourceLocationStack();

  this->astHelper->prepare(this->noticeStore);
  this->typeGenerator->setNoticeStore(this->noticeStore);
  this->commandGenerator->setNoticeStore(this->noticeStore);
  this->expressionGenerator->setNoticeStore(this->noticeStore);

  this->offlineExecution = offlineExecution;
  this->expressionGenerator->setOfflineExecution(offlineExecution);
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
    auto obj = astModule->getElement(i);
    auto def = ti_cast<Data::Ast::Definition>(obj);
    if (def != 0) {
      auto target = def->getTarget().get();
      if (target->isDerivedFrom<Spp::Ast::Module>()) {
        if (!generation->generateModule(static_cast<Spp::Ast::Module*>(target), tg)) result = false;
      } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
        if (!generation->generateFunction(static_cast<Spp::Ast::Function*>(target), tg)) result = false;
      } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
        if (!generation->generateUserTypeBody(static_cast<Spp::Ast::UserType*>(target), tg)) result = false;
      } else if (generator->getAstHelper()->isAstReference(target)) {
        // Generate global variable.
        if (!generation->generateVarDef(def, tg)) {
          result = false;
        }
      }
    } else if (obj->isDerivedFrom<Core::Data::Ast::Bridge>()) {
      if (!generator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(obj))) result = false;
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

  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0 && tryGetCodeGenData<TiObject>(astBlock) == 0) {
    auto astFuncType = astFunc->getType().get();
    auto tgFuncType = tryGetCodeGenData<TiObject>(astFuncType);
    ASSERT(tgFuncType != 0);

    auto astArgs = astFuncType->getArgTypes().get();
    auto astRetTypeRef = astFuncType->getRetType().get();
    Ast::Type *astRetType = astFuncType->traceRetType(generator->astHelper);

    // Prepare the funciton body.
    SharedList<TiObject> tgVars;
    TioSharedPtr tgContext;
    if (!tg->prepareFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext)) return false;

    // Store the generated ret value reference, if needed.
    if (astRetType->hasCustomInitialization(generator->astHelper, tg->getExecutionContext())) {
      setCodeGenData(astRetTypeRef, tgVars.get(0));
      tgVars.remove(0);
    }

    // Store the generated data.
    setCodeGenData(astBlock, tgContext);
    for (Int i = 0; i < tgVars.getCount(); ++i) {
      auto argType = astArgs->getElement(i);
      auto argAstType = Ast::getAstType(argType);
      auto argTgType = getCodeGenData<TiObject>(argAstType);
      TioSharedPtr argTgVar;
      if (!tg->generateLocalVariable(tgContext.get(), argTgType, astArgs->getElementKey(i).c_str(), 0, argTgVar)) {
        return false;
      }
      setCodeGenData(argType, argTgVar);
      Ast::Type *argSourceAstType;
      if (argAstType->hasCustomInitialization(generator->astHelper, tg->getExecutionContext())) {
        argSourceAstType = generator->astHelper->getReferenceTypeFor(argAstType);
      } else {
        argSourceAstType = argAstType;
      }
      SharedList<TiObject> initTgVals;
      PlainList<Ast::Type> initAstTypes;
      initTgVals.clear();
      initTgVals.add(tgVars.get(i));
      initAstTypes.clear();
      initAstTypes.add(argSourceAstType);
      TioSharedPtr argTgVarRef;
      if (!tg->generateVarReference(tgContext.get(), argTgType, argTgVar.get(), argTgVarRef)) {
        return false;
      }
      if (!generation->generateVarInitialization(
        argAstType, argTgVarRef.get(), ti_cast<Core::Data::Node>(argType), &initAstTypes, &initTgVals, tg, tgContext.get()
      )) {
        return false;
      }
    }

    // Generate the function's statements.
    TerminalStatement terminal;
    auto retVal = generation->generateStatements(astBlock, tg, tgContext.get(), terminal);

    // Does this function need to return a value?
    if (!generator->astHelper->isVoid(astRetType) && terminal != TerminalStatement::YES) {
      // A block could have been terminated by a block or continue statement instead of a return, but that's fine
      // since top level breaks and returns will raise an error anyway.
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::MissingReturnStatementNotice>(astFunc->findSourceLocation())
      );
      return false;
    }

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
  PREPARE_SELF(generation, Generation);

  TiObject *tgType;
  if (!generator->typeGenerator->getGeneratedType(astType, tg, tgType, 0)) return false;
  ASSERT(tgType != 0);

  // Prepare struct members.
  auto body = astType->getBody().get();
  if (body == 0) {
    throw EXCEPTION(GenericException, S("User type missing body block."));
  }

  // Generate member functions and sub types.
  Bool result = true;
  for (Int i = 0; i < body->getCount(); ++i) {
    auto obj = body->getElement(i);
    if (obj != 0) {
      if (obj->isDerivedFrom<Core::Data::Ast::Bridge>()) {
        if (!generator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(obj))) result = false;
        continue;
      }
      // TODO: Generate member functions.
      // TODO: Generate subtypes.
    }
  }
  if (!result) return false;

  // Generate static members.
  for (Int i = 0; i < body->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(body->getElement(i));
    if (def != 0) {
      if (generator->astHelper->getDefinitionDomain(def) == Ast::DefinitionDomain::GLOBAL) {
        auto obj = def->getTarget().get();
        if (generator->getAstHelper()->isAstReference(obj)) {
          if (!generation->generateVarDef(def, tg)) result = false;
        } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
          if (!generation->generateFunction(static_cast<Spp::Ast::Function*>(obj), tg)) result = false;
        }
      }
    }
  }

  return true;
}


Bool Generator::_generateVarDef(TiObject *self, Core::Data::Ast::Definition *definition, TargetGeneration *tg)
{
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

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

    Ast::setAstType(astVar, astType);

    if (generator->getAstHelper()->getDefinitionDomain(definition) == Ast::DefinitionDomain::GLOBAL) {
      // Generate a global or a static variable.
      Str name = generator->getAstHelper()->resolveNodePath(definition);
      TioSharedPtr tgGlobalVar;
      if (generator->offlineExecution) {
        // Generate the default value.
        TioSharedPtr tgDefaultValue;
        if (!generator->typeGenerator->generateDefaultValue(astType, tg, 0, tgDefaultValue)) {
          setCodeGenFailed(astVar, true);
          return false;
        }
        // Create the llvm global var.
        if (!tg->generateGlobalVariable(tgType, name.c_str(), tgDefaultValue.get(), tgGlobalVar)) {
          setCodeGenFailed(astVar, true);
          return false;
        }
      } else {
        // Create the llvm global var.
        if (!tg->generateGlobalVariable(tgType, name.c_str(), 0, tgGlobalVar)) {
          setCodeGenFailed(astVar, true);
          return false;
        }
        // Add an entry for the variable in the repo.
        Word size;
        if (!generator->typeGenerator->getTypeAllocationSize(astType, tg, size)) {
          setCodeGenFailed(astVar, true);
          return false;
        }
        generator->globalItemRepo->addItem(name.c_str(), size);
      }

      // TODO: Initialize the variable if it's not an already initialized global.
      // TODO: Add to destructor list if necessary.

      setCodeGenData(astVar, tgGlobalVar);
    } else {
      auto astBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(definition);
      if (ti_cast<Ast::Type>(astBlock->getOwner()) != 0) {
        // This should never happen.
        throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));
      } else {
        // Generate a local variable.

        // To avoid stack overflows we need to allocate at the function level rather than any inner block.
        auto astFuncBlock = astBlock;
        while (astFuncBlock != 0 && ti_cast<Ast::Function>(astFuncBlock->getOwner()) == 0) {
          astFuncBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(astFuncBlock->getOwner());
        }
        if (astFuncBlock == 0) {
          throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));
        }

        // At this point we should already have a TG context.
        auto tgFuncContext = getCodeGenData<TiObject>(astFuncBlock);

        // Create the target local var.
        TioSharedPtr tgLocalVar;
        if (!tg->generateLocalVariable(tgFuncContext, tgType, definition->getName().get(), 0, tgLocalVar)) {
          setCodeGenFailed(astVar, true);
          return false;
        }

        // Initialize the variable.
        // TODO: Should we use default values with local variables?
        // TioSharedPtr tgDefaultValue;
        // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
        //   if (!generator->typeGenerator->generateDefaultValue(astType, tg, tgContext, tgDefaultValue)) return false;
        // }
        auto tgContext = getCodeGenData<TiObject>(astBlock);
        SharedList<TiObject> initTgVals;
        PlainList<Ast::Type> initAstTypes;
        TioSharedPtr tgLocalVarRef;
        if (!tg->generateVarReference(tgContext, tgType, tgLocalVar.get(), tgLocalVarRef)) {
          return false;
        }
        if (!generation->generateVarInitialization(
          astType, tgLocalVarRef.get(), definition, &initAstTypes, &initTgVals, tg, tgContext
        )) return false;

        // TODO: Add to destructor list if necessary.

        setCodeGenData(astVar, tgLocalVar);
      }
    }
  }

  return true;
}


Bool Generator::_generateTempVar(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::Type *astType, TargetGeneration *tg
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  TiObject *tgVar = tryGetCodeGenData<TiObject>(astNode);

  if (tgVar == 0) {
    // Generate the type of the variable.
    TiObject *tgType;
    if (!generator->typeGenerator->getGeneratedType(astType, tg, tgType, 0)) return false;

    // Also generate the reference type of this type.
    Ast::Type *astRefType = generator->astHelper->getPointerTypeFor(astType);
    if (astRefType == 0) {
      throw EXCEPTION(GenericException, S("Could not find reference type for the given var type."));
    }
    TiObject *tgRefType;
    if (!generator->typeGenerator->getGeneratedType(astRefType, tg, tgRefType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate pointer type for the given var type."));
    }

    Ast::setAstType(astNode, astType);

    Core::Data::Ast::Definition tempDef;
    tempDef.setOwner(astNode->getOwner());

    if (generator->getAstHelper()->getDefinitionDomain(&tempDef) == Ast::DefinitionDomain::GLOBAL) {
      // Generate a global or a static variable.
      Str name = generator->getTempVarName();
      TioSharedPtr tgGlobalVar;
      if (generator->offlineExecution) {
        // Generate the default value.
        TioSharedPtr tgDefaultValue;
        if (!generator->typeGenerator->generateDefaultValue(astType, tg, 0, tgDefaultValue)) return false;
        // Create the llvm global var.
        if (!tg->generateGlobalVariable(tgType, name.c_str(), tgDefaultValue.get(), tgGlobalVar)) return false;
      } else {
        // Create the llvm global var.
        if (!tg->generateGlobalVariable(tgType, name.c_str(), 0, tgGlobalVar)) return false;
        // Add an entry for the variable in the repo.
        Word size;
        if (!generator->typeGenerator->getTypeAllocationSize(astType, tg, size)) return false;
        generator->globalItemRepo->addItem(name.c_str(), size);
      }

      // TODO: Initialize the variable.
      // TODO: Add to destructor list if necessary.

      setCodeGenData(astNode, tgGlobalVar);
    } else {
      auto astBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(astNode);
      if (ti_cast<Ast::Type>(astBlock->getOwner()) != 0) {
        // This should never happen.
        throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));
      } else {
        // Generate a local variable.

        // To avoid stack overflows we need to allocate at the function level rather than any inner block.
        auto astFuncBlock = astBlock;
        while (astFuncBlock != 0 && ti_cast<Ast::Function>(astFuncBlock->getOwner()) == 0) {
          astFuncBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(astFuncBlock->getOwner());
        }
        if (astFuncBlock == 0) throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));

        // At this point we should already have a TG context.
        auto tgFuncContext = getCodeGenData<TiObject>(astFuncBlock);

        // Create the llvm local var.
        TioSharedPtr tgLocalVar;
        Str name = generator->getTempVarName();
        if (!tg->generateLocalVariable(tgFuncContext, tgType, name.c_str(), 0, tgLocalVar)) return false;

        // Initialize the variable.
        // TODO: Should we use default values with local variables?
        // TioSharedPtr tgDefaultValue;
        // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
        //   if (!generator->typeGenerator->generateDefaultValue(astType, tg, tgContext, tgDefaultValue)) return false;
        // }
        auto tgContext = getCodeGenData<TiObject>(astBlock);
        SharedList<TiObject> initTgVals;
        PlainList<Ast::Type> initAstTypes;
        TioSharedPtr tgLocalVarRef;
        if (!tg->generateVarReference(tgContext, tgType, tgLocalVar.get(), tgLocalVarRef)) {
          return false;
        }
        if (!generation->generateVarInitialization(
          astType, tgLocalVar.get(), astNode, &initAstTypes, &initTgVals, tg, tgContext
        )) return false;

        // TODO: Add to destructor list if necessary.

        setCodeGenData(astNode, tgLocalVar);
      }
    }
  }

  return true;
}


Bool Generator::_generateVarInitialization(
  TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *paramsAstNode,
  PlainList<Ast::Type> *paramAstTypes, SharedList<TiObject> *paramTgValues,
  TargetGeneration *tg, TiObject *tgContext
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);
  // TODO: Call empty constructors, if any.
  if (paramAstTypes->getCount() > 0) {
    // Initialize from the given value.
    // TODO: Do we have constructors for the given source?
    // Else just copy the value.
    ASSERT(paramAstTypes->getCount() == 1);
    ASSERT(paramTgValues->getCount() == 1);

    // Cast the value to var type.
    if (!paramAstTypes->getElement(0)->isImplicitlyCastableTo(
      varAstType, generator->astHelper, tg->getExecutionContext()
    )) {
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidReturnValueNotice>(Core::Data::Ast::findSourceLocation(paramsAstNode))
      );
      return false;
    }
    TioSharedPtr tgCastedValue;
    if (!generation->generateCast(
      tg, tgContext, paramAstTypes->getElement(0), varAstType, paramTgValues->getElement(0), tgCastedValue)
    ) {
      // This should not happen since non-castable calls should be filtered out earlier.
      throw EXCEPTION(GenericException, S("Invalid cast was unexpectedly found."));
    }

    // Copy the value into the var.
    auto varTgType = getCodeGenData<TiObject>(varAstType);
    TioSharedPtr assignTgRes;
    if (!tg->generateAssign(tgContext, varTgType, tgCastedValue.get(), tgVarRef, assignTgRes)) {
      return false;
    }
  }
  return true;
}


Bool Generator::_generateStatements(
  TiObject *self, Core::Data::Ast::Scope *astBlock, TargetGeneration *tg, TiObject *tgContext,
  TerminalStatement &terminal
) {
  PREPARE_SELF(generation, Generation);
  Bool result = true;
  terminal = TerminalStatement::UNKNOWN;
  for (Int i = 0; i < astBlock->getCount(); ++i) {
    auto astNode = astBlock->getElement(i);
    if (terminal == TerminalStatement::YES) {
      // Unreachable code.
      PREPARE_SELF(generator, Generator);
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::UnreachableCodeNotice>(Core::Data::Ast::findSourceLocation(astNode))
      );
      return false;
    }
    if (!generation->generateStatement(astNode, tg, tgContext, terminal)) result = false;
  }
  return result;
}


Bool Generator::_generateStatement(
  TiObject *self, TiObject *astNode, TargetGeneration *tg, TiObject *tgContext, TerminalStatement &terminal
) {
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(generator);

  terminal = TerminalStatement::NO;

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
    } else if (generator->getAstHelper()->isAstReference(target)) {
      // Generate local variable.
      return generation->generateVarDef(def, tg);
    } else {
      // TODO: Make sure the definition is a literal.
      return true;
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::IfStatement>()) {
    auto ifStatement = static_cast<Spp::Ast::IfStatement*>(astNode);
    return generator->commandGenerator->generateIfStatement(ifStatement, generation, tg, tgContext, terminal);
  } else if (astNode->isDerivedFrom<Spp::Ast::WhileStatement>()) {
    auto whileStatement = static_cast<Spp::Ast::WhileStatement*>(astNode);
    return generator->commandGenerator->generateWhileStatement(whileStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
    auto forStatement = static_cast<Spp::Ast::ForStatement*>(astNode);
    return generator->commandGenerator->generateForStatement(forStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::ContinueStatement>()) {
    terminal = TerminalStatement::YES;
    auto continueStatement = static_cast<Spp::Ast::ContinueStatement*>(astNode);
    return generator->commandGenerator->generateContinueStatement(continueStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::BreakStatement>()) {
    terminal = TerminalStatement::YES;
    auto breakStatement = static_cast<Spp::Ast::BreakStatement*>(astNode);
    return generator->commandGenerator->generateBreakStatement(breakStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Spp::Ast::ReturnStatement>()) {
    terminal = TerminalStatement::YES;
    auto returnStatement = static_cast<Spp::Ast::ReturnStatement*>(astNode);
    return generator->commandGenerator->generateReturnStatement(returnStatement, generation, tg, tgContext);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bridge>()) {
    return generator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(astNode));
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


//==============================================================================
// Helper Functions

Str Generator::getTempVarName()
{
  return Str("#temp") + std::to_string(this->tempVarIndex++);
}

} } // namespace

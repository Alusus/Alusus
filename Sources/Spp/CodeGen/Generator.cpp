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
    TerminalStatement terminal;
    auto retVal = generation->generateStatements(astBlock, tg, tgContext.get(), terminal);

    // Does this function need to return a value?
    auto astRetType = astFuncType->traceRetType(generator->astHelper);
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
      setCodeGenData(astVar, tgGlobalVar);
    } else {
      auto astBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(definition);
      if (ti_cast<Ast::Type>(astBlock->getOwner()) != 0) {
        // This should never happen.
        throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));
      } else {
        // Generate a local variable.

        // To avoid stack overflows we need to allocate at the function level rather than any inner block.
        while (astBlock != 0 && ti_cast<Ast::Function>(astBlock->getOwner()) == 0) {
          astBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(astBlock->getOwner());
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
        if (!tg->generateLocalVariable(tgContext, tgType, definition->getName().get(), 0, tgLocalVar)) {
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

} } // namespace

/**
 * @file Spp/CodeGen/Generator.cpp
 * Contains the implementation of class Spp::CodeGen::Generator.
 *
 * @copyright Copyright (C) 2025 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::CodeGen
{

//==============================================================================
// Initialization Functions

void Generator::initBindings()
{
  auto generation = ti_cast<Generation>(this);

  generation->generateModules = &Generator::_generateModules;
  generation->generateModule = &Generator::_generateModule;
  generation->generateModuleInit = &Generator::_generateModuleInit;
  generation->generateFunction = &Generator::_generateFunction;
  generation->generateFunctionDecl = &Generator::_generateFunctionDecl;
  generation->prepareNullaryProcedure = &Generator::_prepareNullaryProcedure;
  generation->finalizeNullaryProcedure = &Generator::_finalizeNullaryProcedure;
  generation->generateUserTypeBody = &Generator::_generateUserTypeBody;
  generation->generateVarDef = &Generator::_generateVarDef;
  generation->generateTempVar = &Generator::_generateTempVar;
  generation->generateVarInitialization = &Generator::_generateVarInitialization;
  generation->generateMemberVarInitialization = &Generator::_generateMemberVarInitialization;
  generation->generateVarDestruction = &Generator::_generateVarDestruction;
  generation->generateMemberVarDestruction = &Generator::_generateMemberVarDestruction;
  generation->registerDestructor = &Generator::_registerDestructor;
  generation->generateVarGroupDestruction = &Generator::_generateVarGroupDestruction;
  generation->generateStatementBlock = &Generator::_generateStatementBlock;
  generation->generateStatement = &Generator::_generateStatement;
  generation->generateExpression = &Generator::_generateExpression;
  generation->generateCast = &Generator::_generateCast;
  generation->generateFunctionCall = &Generator::_generateFunctionCall;
  generation->getGeneratedType = &Generator::_getGeneratedType;
  generation->getTypeAllocationSize = &Generator::_getTypeAllocationSize;
  generation->addThisDefinition = &Generator::_addThisDefinition;
  generation->buildDependencies = &Generator::_buildDependencies;
}


//==============================================================================
// Code Generation Functions

Bool Generator::_generateModules(TiObject *self, Core::Data::Ast::Scope *root, Session *session)
{
  PREPARE_SELF(generation, Generation);

  Bool result = true;
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->getElement(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        if (!generation->generateModule(module, session)) result = false;
      }
    }
  }

  return result;
}


Bool Generator::_generateModule(TiObject *self, Spp::Ast::Module *astModule, Session *session)
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
        if (!generation->generateModule(static_cast<Spp::Ast::Module*>(target), session)) result = false;
      } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
        if (!generation->generateFunction(static_cast<Spp::Ast::Function*>(target), session)) result = false;
      } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
        if (!generation->generateUserTypeBody(static_cast<Spp::Ast::UserType*>(target), session)) result = false;
        // TODO: Generate member functions and sub-types.
      } else if (generator->getAstHelper()->isInMemVariable(target)) {
        // Generate global variable.
        if (!generation->generateVarDef(def, session)) {
          result = false;
        }
      }
    } else if (obj->isDerivedFrom<Core::Data::Ast::Bridge>()) {
      if (!generator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(obj))) result = false;
    }
  }
  return result;
}


Bool Generator::_generateModuleInit(TiObject *self, Spp::Ast::Module *astModule, Session *session)
{
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);
  auto startIndex = session->getEda()->getInitStatementsGenIndex(astModule);
  Bool retVal = true;
  Int i;
  for (i = startIndex; i < astModule->getCount(); ++i) {
    auto obj = astModule->getElement(i);
    if (obj->isDerivedFrom<Core::Data::Ast::Definition>()) {
      auto definition = static_cast<Core::Data::Ast::Definition*>(obj);
      if (definition->getTarget().ti_cast_get<Spp::Ast::Module>() == 0) continue;
    }
    TerminalStatement terminal;
    if (!generation->generateStatement(obj, session, terminal)) retVal = false;
  }
  session->getEda()->setInitStatementsGenIndex(astModule, i);
  return retVal;
}


Bool Generator::_generateFunction(TiObject *self, Spp::Ast::Function *astFunc, Session *session)
{
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(generator);

  auto tgFunc = session->getEda()->tryGetCodeGenData<TiObject>(astFunc);
  if (tgFunc == 0) {
    if (!generation->generateFunctionDecl(astFunc, session)) return false;
    tgFunc = session->getEda()->getCodeGenData<TiObject>(astFunc);
  }

  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0 && session->getEda()->tryGetCodeGenData<TiObject>(astBlock) == 0) {
    LOG(
      Spp::LogLevel::CODEGEN, S("Generating function body: ") << generator->astHelper->getFunctionName(astFunc)
    );

    SharedPtr<TiInt> buildId = newSrdObj<TiInt>(session->getBuildId());
    session->getEda()->setBuildId(astFunc, buildId);

    if (generator->astProcessor != 0) {
      if (!generator->astProcessor->processFunctionBody(astFunc)) {
        session->getEda()->removeBuildId(astFunc);
        return false;
      }
    }

    auto astFuncType = astFunc->getType().get();
    auto tgFuncType = session->getEda()->tryGetCodeGenData<TiObject>(astFuncType);
    ASSERT(tgFuncType != 0);

    auto astArgs = astFuncType->getArgTypes().get();
    auto astRetTypeRef = astFuncType->getRetType().get();
    Ast::Type *astRetType = astFuncType->traceRetType(generator->astHelper);

    // Prepare the function body.
    SharedList<TiObject> tgVars;
    TioSharedPtr tgContext;
    if (!session->getTg()->prepareFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext)) return false;

    auto destructionStack = newSrdObj<DestructionStack>();
    auto dependencyInfo = newSrdObj<DependencyInfo>();
    Session childSession(session, tgContext.get(), tgContext.get(), destructionStack, dependencyInfo);

    // Store the generated ret value reference, if needed.
    if (astRetType->getInitializationMethod(generator->astHelper) != Ast::TypeInitMethod::NONE) {
      session->getEda()->setCodeGenData(astRetTypeRef, tgVars.get(0));
      tgVars.remove(0);
    }

    // Store the generated data.
    session->getEda()->setCodeGenData(astBlock, tgContext);
    for (Int i = 0; i < tgVars.getCount(); ++i) {
      auto argType = astArgs->getElement(i);
      if (argType->isDerivedFrom<Ast::ArgPack>()) {
        // If this is the var arg pack, then it should already be initialized for us.
        session->getEda()->setCodeGenData(argType, tgVars.get(i));
        Ast::setAstType(argType, generator->astHelper->getCharType());
        break;
      }
      auto argAstType = Ast::getAstType(argType);
      auto argTgType = session->getEda()->getCodeGenData<TiObject>(argAstType);
      TioSharedPtr argTgVar;
      if (!session->getTg()->generateLocalVariable(
        tgContext.get(), argTgType, astArgs->getElementKey(i), 0, argTgVar
      )) {
        session->getEda()->removeBuildId(astFunc);
        return false;
      }
      session->getEda()->setCodeGenData(argType, argTgVar);
      Ast::Type *argSourceAstType;
      if (argAstType->getInitializationMethod(generator->astHelper) != Ast::TypeInitMethod::NONE) {
        argSourceAstType = generator->astHelper->getReferenceTypeFor(argAstType, Ast::ReferenceMode::IMPLICIT);
      } else {
        argSourceAstType = argAstType;
      }
      SharedList<TiObject> initTgVals;
      PlainList<TiObject> initAstTypes;
      PlainList<TiObject> initAstNodes;
      initTgVals.clear();
      initTgVals.add(tgVars.get(i));
      initAstTypes.clear();
      initAstTypes.add(argSourceAstType);
      initAstNodes.clear();
      initAstNodes.add(argType);
      TioSharedPtr argTgVarRef;
      if (!session->getTg()->generateVarReference(tgContext.get(), argTgType, argTgVar.get(), argTgVarRef)) {
        session->getEda()->removeBuildId(astFunc);
        return false;
      }
      if (!generation->generateVarInitialization(
        argAstType, argTgVarRef.get(), ti_cast<Core::Data::Node>(argType),
        &initAstNodes, &initAstTypes, &initTgVals, &childSession
      )) {
        session->getEda()->removeBuildId(astFunc);
        return false;
      }
      generation->registerDestructor(
        ti_cast<Core::Data::Node>(argType), argAstType, argTgVar, childSession.getDestructionStack().get()
      );
    }

    // Generate the function's statements.
    TerminalStatement terminal;
    auto retVal = generation->generateStatementBlock(astBlock, &childSession, terminal);

    // Does this function need to return a value?
    if (!generator->astHelper->isVoid(astRetType) && terminal != TerminalStatement::YES) {
      // A block could have been terminated by a block or continue statement instead of a return, but that's fine
      // since top level breaks and returns will raise an error anyway.
      generator->rootManager->getNoticeStore()->add(
        newSrdObj<Spp::Notices::MissingReturnStatementNotice>(astFunc->findSourceLocation())
      );
      session->getEda()->removeBuildId(astFunc);
      retVal = false;
    }

    // If there wasn't a return statement then we should destruct the variables.
    if (terminal != TerminalStatement::YES) {
      // Destruct function args first, then return.
      if (!generation->generateVarGroupDestruction(&childSession, 0)) retVal = false;
    }

    // Finalize the body.
    if (!session->getTg()->finishFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext.get())) {
      session->getEda()->removeBuildId(astFunc);
      retVal = false;
    }

    // Build dependencies.
    if (!generation->buildDependencies(&childSession)) retVal = false;

    session->getEda()->removeBuildId(astFunc);

    return retVal;
  }
  return true;
}


Bool Generator::_generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc, Session *session)
{
  PREPARE_SELF(generator, Generator);

  auto tgFunc = session->getEda()->tryGetCodeGenData<TiObject>(astFunc);
  if (tgFunc != 0) return true;

  // Generate function type.
  TiObject *tgFunctionType;
  if (!generator->typeGenerator->getGeneratedType(
    astFunc->getType().get(), ti_cast<Generation>(self), session, tgFunctionType, 0
  )) {
    return false;
  }

  // Generate the function object.
  Str name = generator->astHelper->getFunctionName(astFunc);
  TioSharedPtr tgFuncResult;
  if (!session->getTg()->generateFunctionDecl(name, tgFunctionType, tgFuncResult)) return false;
  session->getEda()->setCodeGenData(astFunc, tgFuncResult);

  // TODO: Do we need these attributes?
  // if (astFunc->getBody() == 0) {
  //   llvmFunc->addFnAttr(llvm::Attribute::NoCapture);
  //   llvmFunc->addFnAttr(llvm::Attribute::NoUnwind);
  // }

  return true;
}


void Generator::_prepareNullaryProcedure(
  TiObject *self, Char const *funcName, Session *session, TioSharedPtr &tgFunc, TioSharedPtr &tgContext
) {
  auto tgFuncType = session->getTg()->getNullaryProcedureType();
  if (tgFuncType == 0) {
    throw EXCEPTION(GenericException, S("Failed to get nullary procedure type."));
  };
  if (!session->getTg()->generateFunctionDecl(funcName, tgFuncType, tgFunc)) {
    throw EXCEPTION(GenericException, S("Failed to generate function declaration for nullary procedure."));
  }
  SharedList<TiObject> args;
  if (!session->getTg()->prepareFunctionBody(tgFunc.get(), tgFuncType, &args, tgContext)) {
    throw EXCEPTION(GenericException, S("Failed to prepare function body for root scope execution."));
  }
}


void Generator::_finalizeNullaryProcedure(TiObject *self, Session *session, TiObject *tgFunc, TiObject *tgContext)
{
  auto tgFuncType = session->getTg()->getNullaryProcedureType();
  if (tgFuncType == 0) {
    throw EXCEPTION(GenericException, S("Failed to get nullary procedure type."));
  };
  SharedList<TiObject> args;
  if (!session->getTg()->finishFunctionBody(tgFunc, tgFuncType, &args, tgContext)) {
    throw EXCEPTION(GenericException, S("Failed to finalize function body for nullary procedure."));
  }
}


Bool Generator::_generateUserTypeBody(TiObject *self, Spp::Ast::UserType *astType, Session *session)
{
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  TiObject *tgType;
  if (!generator->typeGenerator->getGeneratedType(astType, generation, session, tgType, 0)) return false;
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

  return true;
}


Bool Generator::_generateVarDef(TiObject *self, Core::Data::Ast::Definition *definition, Session *session)
{
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  TiObject *astVar = definition->getTarget().get();
  TiObject *tgVar = session->getEda()->tryGetCodeGenData<TiObject>(astVar);

  if (tgVar == 0) {
    // Have we previously tried to build this var?
    if (session->getEda()->didCodeGenFail(astVar)) return false;

    // Get initialization params, if any.
    TiObject *astTypeRef = astVar;
    TiObject *astParams = 0;
    auto astParamPass = ti_cast<Core::Data::Ast::ParamPass>(astVar);
    if (astParamPass != 0 && astParamPass->getType() == Core::Data::Ast::BracketType::ROUND) {
      astTypeRef = astParamPass->getOperand().get();
      astParams = astParamPass->getParam().get();
    }

    // Generate the type of the variable.
    Ast::Type *astType;
    TiObject *tgType;
    if (!generator->typeGenerator->getGeneratedType(astTypeRef, generation, session, tgType, &astType)) {
      session->getEda()->setCodeGenFailed(astVar, true);
      return false;
    }

    // Also generate the reference type of this type.
    Ast::Type *astRefType = generator->astHelper->getPointerTypeFor(astType);
    if (astRefType == 0) {
      throw EXCEPTION(GenericException, S("Could not find reference type for the given var type."));
    }
    TiObject *tgRefType;
    if (!generator->typeGenerator->getGeneratedType(astRefType, generation, session, tgRefType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate pointer type for the given var type."));
    }

    Ast::setAstType(astVar, astType);

    if (generator->getAstHelper()->getVariableDomain(definition) == Ast::DefinitionDomain::GLOBAL) {
      auto state = generator->getGlobalVarState(session, definition);
      if ((state & (GlobalVarState::INITIALIZING | GlobalVarState::TERMINATING)) != 0) {
        throw EXCEPTION(GenericException, S("Global var being initialized/terminated before being code generated."));
      }

      // Generate a global or a static variable.
      Str name = generator->getGlobalVarMangledName(definition);
      TioSharedPtr tgGlobalVar;

      // Generate the default value.
      TioSharedPtr tgDefaultValue;
      if (session->isOfflineExecution()) {
        if (!generator->typeGenerator->generateDefaultValue(astType, generation, session, tgDefaultValue)) {
          session->getEda()->setCodeGenFailed(astVar, true);
          return false;
        }
      }
      // Create the llvm global var.
      if (!session->getTg()->generateGlobalVariable(tgType, name, tgDefaultValue.get(), tgGlobalVar)) {
        session->getEda()->setCodeGenFailed(astVar, true);
        return false;
      }
      session->getEda()->setCodeGenData(astVar, tgGlobalVar);

      if (!session->isOfflineExecution()) {
        Int itemIndex = generator->globalItemRepo->findItem(name);
        if (itemIndex == -1) {
          // Add an entry for the variable in the repo.
          Word size;
          if (!generator->typeGenerator->getTypeAllocationSize(astType, generation, session, size)) {
            session->getEda()->setCodeGenFailed(astVar, true);
            return false;
          }
          generator->globalItemRepo->addItem(name, size);
        }
      }

      // Determine whether the variable initialization is high priority.
      // TODO: Switch to using an integer priority value instead of the boolean priority.
      auto highPriority = generator->getAstHelper()->doesModifierExistOnDef(definition, "priority");

      if (astParams != 0 || astType->getInitializationMethod(generator->astHelper) != Ast::TypeInitMethod::NONE) {
        if ((state & GlobalVarState::INITIALIZED) == 0) {
          session->getGlobalVarInitializationDeps()->add(static_cast<Core::Data::Node*>(astVar), highPriority);
        }
      }

      if (astType->getDestructionMethod(generator->astHelper) != Ast::TypeInitMethod::NONE) {
        if ((state & GlobalVarState::TERMINATED) == 0) {
          session->getGlobalVarDestructionDeps()->add(static_cast<Core::Data::Node*>(astVar), highPriority);
        }
      }
    } else {
      // Generate a local variable.

      // At this point we should already have a TG context.
      if (session->getTgAllocContext() == 0) {
        throw EXCEPTION(GenericException, S("Missing TG allocation context."));
      }

      // Create the target local var.
      TioSharedPtr tgLocalVar;
      if (!session->getTg()->generateLocalVariable(
        session->getTgAllocContext(), tgType, definition->getName().get(), 0, tgLocalVar)
      ) {
        session->getEda()->setCodeGenFailed(astVar, true);
        return false;
      }
      session->getEda()->setCodeGenData(astVar, tgLocalVar);

      // Initialize the variable.
      // TODO: Should we use default values with local variables?
      // TioSharedPtr tgDefaultValue;
      // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
      //   if (!generator->typeGenerator->generateDefaultValue(
      //     astType, generation, session, tgDefaultValue
      //   )) return false;
      // }
      TioSharedPtr tgLocalVarRef;
      if (!session->getTg()->generateVarReference(session->getTgContext(), tgType, tgLocalVar.get(), tgLocalVarRef)) {
        return false;
      }

      session->getDestructionStack()->pushScope();

      SharedList<TiObject> initTgVals;
      PlainList<TiObject> initAstTypes;
      PlainList<TiObject> initAstNodes;
      if (astParams != 0) {
        if (!generator->expressionGenerator->generateParams(
          astParams, generation, session, &initAstNodes, &initAstTypes, &initTgVals
        )) {
          session->getDestructionStack()->popScope();
          return false;
        }
      }
      if (!generation->generateVarInitialization(
        astType, tgLocalVarRef.get(), definition, &initAstNodes, &initAstTypes, &initTgVals, session
      )) {
        session->getDestructionStack()->popScope();
        return false;
      }

      if (!generation->generateVarGroupDestruction(session, session->getDestructionStack()->getScopeStartIndex(-1))) {
        session->getDestructionStack()->popScope();
        return false;
      }
      session->getDestructionStack()->popScope();

      generation->registerDestructor(
        ti_cast<Core::Data::Node>(astVar), astType, tgLocalVar, session->getDestructionStack().get()
      );
    }
  } else {
    // Check against circular dependency of global var initialization.
    if (generator->getAstHelper()->getVariableDomain(definition) == Ast::DefinitionDomain::GLOBAL) {
      auto state = generator->getGlobalVarState(session, definition);
      if (state & (GlobalVarState::INITIALIZING | GlobalVarState::TERMINATING) != 0) {
        generator->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::CircularGlobalVarInitNotice>(
            Core::Data::Ast::findSourceLocation(definition)
          )
        );
        return false;
      }
    }
  }

  return true;
}


Bool Generator::_generateTempVar(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::Type *astType, Session *session, Bool initialize,
  TioSharedPtr &tgVar
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  // Generate the type of the variable.
  TiObject *tgType;
  if (!generator->typeGenerator->getGeneratedType(astType, generation, session, tgType, 0)) return false;

  // Also generate the reference type of this type.
  Ast::Type *astRefType = generator->astHelper->getPointerTypeFor(astType);
  if (astRefType == 0) {
    throw EXCEPTION(GenericException, S("Could not find reference type for the given var type."));
  }
  TiObject *tgRefType;
  if (!generator->typeGenerator->getGeneratedType(astRefType, generation, session, tgRefType, 0)) {
    throw EXCEPTION(GenericException, S("Failed to generate pointer type for the given var type."));
  }

  // Ast::setAstType(astNode, astType);

  Core::Data::Ast::Definition tempDef;
  tempDef.setOwner(astNode->getOwner());

  // At this point we should already have a TG context.
  if (session->getTgAllocContext() == 0) {
    throw EXCEPTION(GenericException, S("Missing TG allocation context."));
  }

  // Create the llvm local var.
  Str name = generator->getTempVarName();
  if (!session->getTg()->generateLocalVariable(session->getTgAllocContext(), tgType, name, 0, tgVar)) {
    return false;
  }

  if (initialize) {
    // Initialize the variable.
    // TODO: Should we use default values with local variables?
    // TioSharedPtr tgDefaultValue;
    // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
    //   if (!generator->typeGenerator->generateDefaultValue(
    //     astType, generation, tg, tgContext, tgDefaultValue
    //   )) return false;
    // }
    TioSharedPtr tgVarRef;
    if (!session->getTg()->generateVarReference(session->getTgContext(), tgType, tgVar.get(), tgVarRef)) {
      return false;
    }
    SharedList<TiObject> initTgVals;
    PlainList<TiObject> initAstTypes;
    PlainList<TiObject> initAstNodes;
    if (!generation->generateVarInitialization(
      astType, tgVar.get(), astNode, &initAstNodes, &initAstTypes, &initTgVals, session
    )) return false;

    generation->registerDestructor(astNode, astType, tgVar, session->getDestructionStack().get());
  }

  return true;
}


Bool Generator::_generateVarInitialization(
  TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode,
  PlainList<TiObject> *paramAstNodes, PlainList<TiObject> *paramAstTypes, SharedList<TiObject> *paramTgValues,
  Session *session
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  // Do we have custom initialization?
  if (varAstType->getInitializationMethod(generator->getAstHelper()) != Ast::TypeInitMethod::NONE) {
    TiObject *tgAutoCtor;
    if (!generator->typeGenerator->getGeneratedTypeAutoCtor(varAstType, generation, session, tgAutoCtor)) return false;
    // Call automatic constructors, if any.
    if (tgAutoCtor != 0) {
      PlainList<TiObject> autoTgValues({ tgVarRef });
      TioSharedPtr dummy;
      if (!session->getTg()->generateFunctionCall(session->getTgContext(), tgAutoCtor, &autoTgValues, dummy)) {
        return false;
      }
    }

    // Add `this` to parameter list.
    auto varPtrAstType = generator->getAstHelper()->getReferenceTypeFor(varAstType, Ast::ReferenceMode::IMPLICIT);

    // Do we have constructors matching the given vars?
    static Core::Data::Ast::Identifier ref({{ S("value"), TiStr(S("~init")) }});
    Ast::CalleeLookupRequest lookupRequest;
    lookupRequest.astNode = astNode;
    lookupRequest.target = varAstType;
    lookupRequest.thisType = varPtrAstType;
    lookupRequest.mode = Ast::CalleeLookupMode::OBJECT_MEMBER;
    lookupRequest.skipInjections = true;
    lookupRequest.ref = &ref;
    lookupRequest.argTypes = paramAstTypes;
    lookupRequest.op = S("~init");
    Ast::CalleeLookupResult lookupResult;
    generator->calleeTracer->lookupCallee(lookupRequest, lookupResult);
    if (lookupResult.isSuccessful() && lookupResult.stack.getLength() == 1) {
      auto callee = lookupResult.stack(lookupResult.stack.getLength() - 1);
      // Prepare the arguments to send.
      paramAstNodes->insertElement(0, astNode);
      paramAstTypes->insertElement(0, varPtrAstType);
      paramTgValues->insertElement(0, tgVarRef);
      if (!generator->getExpressionGenerator()->prepareFunctionParams(
        static_cast<Ast::Function*>(callee.obj)->getType().get(), generation, session,
        paramAstNodes, paramAstTypes, paramTgValues
      )) return false;
      // Call the found constructor.
      GenResult result;
      return generator->getExpressionGenerator()->generateFunctionCall(
        astNode, static_cast<Ast::Function*>(callee.obj), paramAstTypes, paramTgValues, generation, session, result
      );
    } else if (
      paramAstTypes->getCount() != 0 ||
      generator->getSeeker()->tryGet(&ref, varAstType, Core::Data::Seeker::Flags::SKIP_OWNERS) != 0
    ) {
      // We have custom initialization but no constructors match the given params.
      generator->rootManager->getNoticeStore()->add(newSrdObj<Spp::Notices::TypeMissingMatchingInitOpNotice>(
        Core::Data::Ast::findSourceLocation(astNode)
      ));
      return false;
    }
  } else {
    // Else do we have a single arg? If so, cast then copy.
    if (paramAstTypes->getCount() == 1) {
      // Cast the value to var type.
      auto paramAstType = generator->getAstHelper()->traceType(paramAstTypes->getElement(0));
      ASSERT(paramAstType);
      GenResult castedValue;
      if (!generation->generateCast(
        session, paramAstType, varAstType, ti_cast<Core::Data::Node>(paramAstNodes->getElement(0)),
        paramTgValues->getElement(0), true, castedValue)
      ) {
        generator->rootManager->getNoticeStore()->add(newSrdObj<Spp::Notices::TypeMissingMatchingInitOpNotice>(
          Core::Data::Ast::findSourceLocation(paramAstNodes->getElement(0))
        ));
        return false;
      }

      // Copy the value into the var.
      TiObject *varTgType;
      if (!generation->getGeneratedType(varAstType, session, varTgType, 0)) return false;
      TioSharedPtr assignTgRes;
      if (!session->getTg()->generateAssign(
        session->getTgContext(), varTgType, castedValue.targetData.get(), tgVarRef, assignTgRes
      )) {
        return false;
      }
    } else if (paramAstTypes->getCount() > 0) {
      generator->rootManager->getNoticeStore()->add(newSrdObj<Spp::Notices::TypeMissingMatchingInitOpNotice>(
        Core::Data::Ast::findSourceLocation(astNode)
      ));
      return false;
    }
  }

  return true;
}


Bool Generator::_generateMemberVarInitialization(
  TiObject *self, TiObject *astMemberNode, Session *session
) {
  if (session->getTgSelf() == 0 || session->getAstSelfType() == 0) {
    throw EXCEPTION(GenericException, S("Missing self while tring to initialize object member variables."));
  }

  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  // Get initialization params, if any.
  TiObject *astParams = 0;
  auto astParamPass = ti_cast<Core::Data::Ast::ParamPass>(astMemberNode);
  if (astParamPass != 0 && astParamPass->getType() == Core::Data::Ast::BracketType::ROUND) {
    astParams = astParamPass->getParam().get();
  }

  // Get the member generated value and type.
  auto tgMemberVar = session->getEda()->tryGetCodeGenData<TiObject>(astMemberNode);
  if (tgMemberVar == 0) {
    // This situation will only happen if we have circular code generation.
    generator->astHelper->getNoticeStore()->add(newSrdObj<Spp::Notices::CircularUserTypeCodeGenNotice>(
      Core::Data::Ast::findSourceLocation(astMemberNode)
    ));
    return false;
  }
  auto astMemberType = Ast::getAstType(astMemberNode);
  auto paramPass = ti_cast<Core::Data::Ast::ParamPass>(astMemberNode);
  if (!paramPass || paramPass->getType() != Core::Data::Ast::BracketType::ROUND) {
    if (astMemberType->getInitializationMethod(generator->getAstHelper()) == Ast::TypeInitMethod::NONE) {
      return true;
    }
  }
  TiObject *tgMemberType;
  if (!generation->getGeneratedType(astMemberType, session, tgMemberType, 0)) return false;

  // Get the struct ptr TG type.
  TiObject *astSelfPtrType = generator->astHelper->getPointerTypeFor(session->getAstSelfType());
  TiObject *tgStructType;
  if (!generation->getGeneratedType(astSelfPtrType, session, tgStructType, 0)) return false;

  // Generate member access.
  TioSharedPtr tgMemberVarRef;
  if (!session->getTg()->generateMemberVarReference(
    session->getTgContext(), tgStructType, tgMemberType, tgMemberVar, session->getTgSelf().get(), tgMemberVarRef
  )) {
    return false;
  }

  session->getDestructionStack()->pushScope();

  // Initialize the member variable.
  SharedList<TiObject> initTgVals;
  PlainList<TiObject> initAstTypes;
  PlainList<TiObject> initAstNodes;
  if (astParams != 0) {
    if (!generator->expressionGenerator->generateParams(
      astParams, generation, session, &initAstNodes, &initAstTypes, &initTgVals
    )) {
      session->getDestructionStack()->popScope();
      return false;
    }
  }
  if (!generation->generateVarInitialization(
    astMemberType, tgMemberVarRef.get(), ti_cast<Core::Data::Node>(astMemberNode),
    &initAstNodes, &initAstTypes, &initTgVals, session
  )) {
    session->getDestructionStack()->popScope();
    return false;
  }

  if (!generation->generateVarGroupDestruction(session, session->getDestructionStack()->getScopeStartIndex(-1))) {
    session->getDestructionStack()->popScope();
    return false;
  }
  session->getDestructionStack()->popScope();

  return true;
}


Bool Generator::_generateVarDestruction(
  TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode, Session *session
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  if (varAstType->getDestructionMethod(generator->getAstHelper()) == Ast::TypeInitMethod::NONE) {
    return true;
  }

  // Prepare param list.
  PlainList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  auto ptrAstType = generator->getAstHelper()->getReferenceTypeFor(varAstType, Ast::ReferenceMode::IMPLICIT);

  // Find the destructor.
  static Core::Data::Ast::Identifier ref({{ S("value"), TiStr(S("~terminate")) }});
  Ast::CalleeLookupRequest lookupRequest;
  lookupRequest.astNode = astNode;
  lookupRequest.target = varAstType;
  lookupRequest.thisType = ptrAstType;
  lookupRequest.mode = Ast::CalleeLookupMode::OBJECT_MEMBER;
  lookupRequest.skipInjections = true;
  lookupRequest.ref = &ref;
  lookupRequest.argTypes = &paramAstTypes;
  lookupRequest.op = S("~terminate");
  Ast::CalleeLookupResult lookupResult;
  generator->calleeTracer->lookupCallee(lookupRequest, lookupResult);
  if (lookupResult.isSuccessful() && lookupResult.stack.getLength() == 1) {
    auto callee = static_cast<Ast::Function*>(lookupResult.stack(lookupResult.stack.getLength() - 1).obj);
    // Call the destructor.
    paramAstTypes.insertElement(0, ptrAstType);
    paramTgValues.insertElement(0, tgVarRef);
    GenResult result;
    if (!generator->getExpressionGenerator()->generateFunctionCall(
      astNode, callee, &paramAstTypes, &paramTgValues, generation, session, result
    )) return false;
  }

  // Call auto destructor, if any.
  TiObject *tgAutoDtor;
  if (!generator->typeGenerator->getGeneratedTypeAutoDtor(varAstType, generation, session, tgAutoDtor)) return false;
  if (tgAutoDtor != 0) {
    PlainList<TiObject> autoTgValues({ tgVarRef });
    TioSharedPtr dummy;
    if (!session->getTg()->generateFunctionCall(session->getTgContext(), tgAutoDtor, &autoTgValues, dummy)) return false;
  }

  return true;
}


Bool Generator::_generateMemberVarDestruction(
  TiObject *self, TiObject *astMemberNode, Session *session
) {
  if (session->getTgSelf() == 0 || session->getAstSelfType() == 0) {
    throw EXCEPTION(GenericException, S("Missing self while tring to destruct object member variables."));
  }

  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  // Get the member generated value and type.
  auto tgMemberVar = session->getEda()->getCodeGenData<TiObject>(astMemberNode);
  auto astMemberType = Ast::getAstType(astMemberNode);
  if (astMemberType->getDestructionMethod(generator->getAstHelper()) == Ast::TypeInitMethod::NONE) {
    return true;
  }
  TiObject *tgMemberType;
  if (!generation->getGeneratedType(astMemberType, session, tgMemberType, 0)) return false;

  // Get the struct ptr TG type.
  TiObject *astSelfPtrType = generator->astHelper->getPointerTypeFor(session->getAstSelfType());
  TiObject *tgStructType;
  if (!generation->getGeneratedType(astSelfPtrType, session, tgStructType, 0)) return false;

  // Generate member access.
  TioSharedPtr tgMemberVarRef;
  if (!session->getTg()->generateMemberVarReference(
    session->getTgContext(), tgStructType, tgMemberType, tgMemberVar, session->getTgSelf().get(), tgMemberVarRef
  )) {
    return false;
  }

  // Destruct the member variable.
  SharedList<TiObject> initTgVals;
  PlainList<TiObject> initAstTypes;
  if (!generation->generateVarDestruction(
    astMemberType, tgMemberVarRef.get(), ti_cast<Core::Data::Node>(astMemberNode), session
  )) {
    return false;
  }

  return true;
}


void Generator::_registerDestructor(
  TiObject *self, Core::Data::Node *varAstNode, Ast::Type *astType, TioSharedPtr tgVar,
  DestructionStack *destructionStack
) {
  PREPARE_SELF(generator, Generator);

  // Skip if the type has no custom destructors.
  auto astUserType = ti_cast<Ast::UserType>(astType);
  if (
    astUserType == 0 || astUserType->getDestructionMethod(generator->getAstHelper()) == Ast::TypeInitMethod::NONE
  ) {
    return;
  }

  // Add the var node to the list.
  auto node = newSrdObj<DestructionNode>();
  node->astNode = varAstNode;
  node->astType = astType;
  node->tgVar = tgVar;
  destructionStack->pushItem(node);
}


Bool Generator::_generateVarGroupDestruction(
  TiObject *self, Session *session, Int index
) {
  PREPARE_SELF(generation, Generation);

  for (; index < session->getDestructionStack()->getItemCount(); ++index) {
    auto node = session->getDestructionStack()->getItem(index);

    TioSharedPtr tgVarRef;
    if (!session->getTg()->generateVarReference(session->getTgContext(), node->astType, node->tgVar.get(), tgVarRef)) {
      return false;
    }

    if (!generation->generateVarDestruction(node->astType, tgVarRef.get(), node->astNode, session)) return false;
  }

  return true;
}


Bool Generator::_generateStatementBlock(
  TiObject *self, TiObject *astBlock, Session *session, TerminalStatement &terminal
) {
  PREPARE_SELF(generation, Generation);
  Bool result = true;
  terminal = TerminalStatement::UNKNOWN;
  session->getDestructionStack()->pushScope();
  if (astBlock->isDerivedFrom<Core::Data::Ast::Scope>()) {
    for (Int i = 0; i < static_cast<Core::Data::Ast::Scope*>(astBlock)->getCount(); ++i) {
      auto astNode = static_cast<Core::Data::Ast::Scope*>(astBlock)->getElement(i);
      if (terminal == TerminalStatement::YES) {
        // Unreachable code.
        PREPARE_SELF(generator, Generator);
        generator->rootManager->getNoticeStore()->add(
          newSrdObj<Spp::Notices::UnreachableCodeNotice>(Core::Data::Ast::findSourceLocation(astNode))
        );
        return false;
      }
      if (astNode->isDerivedFrom<Spp::Ast::Block>()) {
        if (!generation->generateStatementBlock(astNode, session, terminal)) result = false;
      } else {
        if (!generation->generateStatement(astNode, session, terminal)) result = false;
      }
    }
  } else {
    if (!generation->generateStatement(astBlock, session, terminal)) result = false;
  }
  if (terminal != TerminalStatement::YES) {
    if (!generation->generateVarGroupDestruction(session, session->getDestructionStack()->getScopeStartIndex(-1))) {
      result = false;
    }
  }
  session->getDestructionStack()->popScope();

  return result;
}


Bool Generator::_generateStatement(
  TiObject *self, TiObject *astNode, Session *session, TerminalStatement &terminal
) {
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(generator);

  terminal = TerminalStatement::NO;
  Bool retVal = true;

  if (astNode->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition*>(astNode);
    auto target = def->getTarget().get();
    if (target == 0) {
      generator->rootManager->getNoticeStore()->add(
        newSrdObj<Core::Notices::InvalidDefCommandNotice>(def->findSourceLocation())
      );
      retVal = false;
    } else if (target->isDerivedFrom<Spp::Ast::Module>()) {
      retVal = generation->generateModuleInit(static_cast<Spp::Ast::Module*>(target), session);
    // } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
    //   // TODO: Generate function.
    //   generator->rootManager->getNoticeStore()->add(
    //     newSrdObj<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
    //   );
    //   retVal = false;
    // } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
    //   // TODO: Generate type.
    //   generator->rootManager->getNoticeStore()->add(
    //     newSrdObj<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
    //   );
    //   retVal = false;
    } else if (generator->getAstHelper()->isInMemVariable(target)) {
      // Generate local variable.
      retVal = generation->generateVarDef(def, session);
    } else {
      // TODO: Make sure the definition is a literal.
      retVal = true;
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::IfStatement>()) {
    auto ifStatement = static_cast<Spp::Ast::IfStatement*>(astNode);
    retVal = generator->commandGenerator->generateIfStatement(ifStatement, generation, session, terminal);
  } else if (astNode->isDerivedFrom<Spp::Ast::WhileStatement>()) {
    auto whileStatement = static_cast<Spp::Ast::WhileStatement*>(astNode);
    retVal = generator->commandGenerator->generateWhileStatement(whileStatement, generation, session);
  } else if (astNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
    auto forStatement = static_cast<Spp::Ast::ForStatement*>(astNode);
    retVal = generator->commandGenerator->generateForStatement(forStatement, generation, session);
  } else if (astNode->isDerivedFrom<Spp::Ast::ContinueStatement>()) {
    terminal = TerminalStatement::YES;
    auto continueStatement = static_cast<Spp::Ast::ContinueStatement*>(astNode);
    retVal = generator->commandGenerator->generateContinueStatement(continueStatement, generation, session);
  } else if (astNode->isDerivedFrom<Spp::Ast::BreakStatement>()) {
    terminal = TerminalStatement::YES;
    auto breakStatement = static_cast<Spp::Ast::BreakStatement*>(astNode);
    retVal = generator->commandGenerator->generateBreakStatement(breakStatement, generation, session);
  } else if (astNode->isDerivedFrom<Spp::Ast::ReturnStatement>()) {
    terminal = TerminalStatement::YES;
    auto returnStatement = static_cast<Spp::Ast::ReturnStatement*>(astNode);
    retVal = generator->commandGenerator->generateReturnStatement(returnStatement, generation, session);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bridge>()) {
    retVal = generator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(astNode));
  } else {
    session->getDestructionStack()->pushScope();

    GenResult result;
    retVal = generation->generateExpression(astNode, session, result, terminal);

    if (terminal != TerminalStatement::YES) {
      if (!generation->generateVarGroupDestruction(session, session->getDestructionStack()->getScopeStartIndex(-1))) {
        retVal = false;
      }
    }

    session->getDestructionStack()->popScope();
  }

  return retVal;
}


Bool Generator::_generateExpression(
  TiObject *self, TiObject *astNode, Session *session, GenResult &result, TerminalStatement &terminal
) {
  PREPARE_SELF(generator, Generator);
  return generator->expressionGenerator->generate(astNode, ti_cast<Generation>(self), session, result, terminal);
}


Bool Generator::_generateCast(
  TiObject *self, Session *session, Spp::Ast::Type *srcType, Spp::Ast::Type *destType,
  Core::Data::Node *astNode, TiObject *tgValue, Bool implicit, GenResult &castedResult
) {
  PREPARE_SELF(generator, Generator);
  return generator->typeGenerator->generateCast(
    ti_cast<Generation>(self), session, srcType, destType, astNode, tgValue, implicit, castedResult
  );
}


Bool Generator::_generateFunctionCall(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::Function *callee,
  Containing<TiObject> *paramAstTypes, Containing<TiObject> *paramTgValues,
  Session *session, GenResult &result
) {
  PREPARE_SELF(generator, Generator);
  return generator->expressionGenerator->generateFunctionCall(
    astNode, callee, paramAstTypes, paramTgValues, ti_cast<Generation>(self), session, result
  );
}


Bool Generator::_getGeneratedType(
  TiObject *self, TiObject *ref, Session *session, TiObject *&targetTypeResult, Ast::Type **astTypeResult
) {
  PREPARE_SELF(generator, Generator);
  return generator->typeGenerator->getGeneratedType(
    ref, ti_cast<Generation>(self), session, targetTypeResult, astTypeResult
  );
}


Bool Generator::_getTypeAllocationSize(TiObject *self, Spp::Ast::Type *astType, Session *session, Word &result)
{
  PREPARE_SELF(generator, Generator);
  return generator->typeGenerator->getTypeAllocationSize(astType, ti_cast<Generation>(self), session, result);
}


Int Generator::_addThisDefinition(
  TiObject *self, Ast::Block *body, Char const *thisName, Ast::Type *astThisType, Bool skipInjection,
  TioSharedPtr const &tgThis, Session *session
) {
  PREPARE_SELF(generator, Generator);
  Core::Data::Ast::Definition *def = 0;
  Int i;
  // Do we already have the definition?
  for (i = 0; i < body->getCount(); ++i) {
    auto tempDef = body->get(i).ti_cast_get<Core::Data::Ast::Definition>();
    if (
      tempDef != 0 &&
      tempDef->getName() == thisName &&
      generator->getAstHelper()->doesModifierExistOnDef(tempDef, S("__autovar"))
    ) {
      def = tempDef;
      break;
    }
  }
  // We don't have the definition yet, so we'll create it.
  if (def == 0) {
    auto modifierList = Core::Data::Ast::List::create({}, {
      Core::Data::Ast::Identifier::create({ {S("value"), TiStr(S("__autovar"))} })
    });
    if (!skipInjection) {
      modifierList->add(Core::Data::Ast::Identifier::create({ {S("value"), TiStr(S("injection"))} }));
    }
    auto newDef = Core::Data::Ast::Definition::create({
      {S("sourceLocation"), body->getSourceLocation()},
      {S("name"), TiStr(thisName)}
    }, {
      {S("modifiers"), modifierList}      
    });
    body->insert(0, newDef);
    i = 0;
    def = newDef.get();
  }
  // Now we can set the target to the new or existing definition.
  def->setTarget(Spp::Ast::Variable::create({
    {S("valueOnly"), TiBool(true)},
    {S("sourceLocation"), body->getSourceLocation()}
  }, {
    {S("type"), astThisType}
  }));
  session->getEda()->setCodeGenData(def->getTarget().get(), tgThis);
  return i;
}


Bool Generator::_buildDependencies(TiObject *self, Session *session)
{
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(self);

  Bool result = true;

  // Build function dependencies.
  while (session->getFuncDeps()->getCount() > 0) {
    auto astFunc = static_cast<Ast::Function*>(session->getFuncDeps()->get(0));
    session->getFuncDeps()->remove(0);
    if (!generation->generateFunction(astFunc, session)) result = false;
  }

  // Build global var dependencies.
  if (session->getGlobalVarInitializationDeps()->getCount() > 0) {
    generator->funcNameIndex++;

    // Build the constructor function.
    StrStream ctorNameStream;
    ctorNameStream << S("__constructor__") << generator->funcNameIndex;
    GlobalCtorDtorInfo ctorInfo;
    ctorInfo.name = ctorNameStream.str().c_str();
    if (generator->buildGlobalCtorOrDtor(
      session, session->getGlobalVarInitializationDeps(), ctorInfo.name, false,
      [=,&ctorInfo](
        Spp::Ast::Type *varAstType, TiObject *varTgRef, Core::Data::Node *varAstNode, TiObject *astParams,
        Session *childSession
      )->Bool {
        childSession->getDestructionStack()->pushScope();

        SharedList<TiObject> initTgVals;
        PlainList<TiObject> initAstTypes;
        PlainList<TiObject> initAstNodes;
        if (astParams != 0) {
          if (!generator->getExpressionGenerator()->generateParams(
            astParams, generation, childSession, &initAstNodes, &initAstTypes, &initTgVals
          )) {
            childSession->getDestructionStack()->popScope();
            return false;
          }
        }
        if (!generation->generateVarInitialization(
          varAstType, varTgRef, varAstNode, &initAstNodes, &initAstTypes, &initTgVals, childSession
        )) {
          childSession->getDestructionStack()->popScope();
          return false;
        };

        if (!generation->generateVarGroupDestruction(
          childSession, childSession->getDestructionStack()->getScopeStartIndex(-1)
        )) {
          childSession->getDestructionStack()->popScope();
          return false;
        }
        childSession->getDestructionStack()->popScope();

        // Add the name of this var to the ctor info.
        Str name = S("!");
        name += generator->getAstHelper()->resolveNodePath(varAstNode->getOwner());
        ctorInfo.initializedVarNames.add(name);

        return true;
      }
    )) {
      session->getGlobalCtors()->add(ctorInfo);
    } else {
      result = false;
    }
  }

  if (session->getGlobalVarDestructionDeps()->getCount() > 0) {
    generator->funcNameIndex++;

    // Build the destructor function.
    StrStream dtorNameStream;
    dtorNameStream << S("__destructor__") << generator->funcNameIndex;
    GlobalCtorDtorInfo dtorInfo;
    dtorInfo.name = dtorNameStream.str().c_str();
    if (generator->buildGlobalCtorOrDtor(
      session, session->getGlobalVarDestructionDeps(), dtorInfo.name, true,
      [=](
        Spp::Ast::Type *varAstType, TiObject *varTgRef, Core::Data::Node *varAstNode, TiObject *astParams,
        Session *childSession
      )->Bool {
        return generation->generateVarDestruction(varAstType, varTgRef, varAstNode, childSession);
      }
    )) {
      session->getGlobalDtors()->add(dtorInfo);
    } else {
      result = false;
    }
  }

  // Assert that we don't have any remaining dependencies.
  if (session->getFuncDeps()->getCount() > 0 ||
    session->getGlobalVarInitializationDeps()->getCount() > 0 ||
    session->getGlobalVarDestructionDeps()->getCount() > 0) {
      throw EXCEPTION(GenericException, S("Unexpected remaining dependencies."));
  }

  return result;
}


Bool Generator::buildGlobalCtorOrDtor(
  Session *session, DependencyList<Core::Data::Node> *deps, Char const *funcName, Bool dtor,
  std::function<Bool(
    Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *astNode, TiObject *astParams,
    Session *session
  )> varOpCallback
) {
  auto generation = ti_cast<Generation>(this);

  Bool result = true;

  // Mark all global vars as being initilzied/terminated.
  for (Int i = 0; i < deps->getCount(); ++i) {
    auto astVar = deps->get(i);
    Int state = this->getGlobalVarState(session, astVar);
    if (dtor) {
      if ((state & GlobalVarState::TERMINATED) != 0) {
        // Already destructed. This happens if a previous dependency was dependent on this global
        // var and generated the termination code for it.
        continue;
      } else if ((state & GlobalVarState::TERMINATING) != 0) {
        // Already being destructed. This happens in case of circular dependencies, which is an error.
        this->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::CircularGlobalVarInitNotice>(
            Core::Data::Ast::findSourceLocation(astVar)
          )
        );
        result = false;
        continue;
      }
      // Mark as being destructed.
      this->setGlobalVarState(session, astVar, state | GlobalVarState::TERMINATING);
    } else {
      if ((state & GlobalVarState::INITIALIZED) != 0) {
        // Already initialized. This happens if a previous dependency was dependent on this global
        // var and generated the initialization code for it.
        continue;
      } else if ((state & GlobalVarState::INITIALIZING) != 0) {
        // Already being initialized. This happens in case of circular dependencies, which is an error.
        this->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::CircularGlobalVarInitNotice>(
            Core::Data::Ast::findSourceLocation(astVar)
          )
        );
        result = false;
        continue;
      }
      // Mark as being destructed.
      this->setGlobalVarState(session, astVar, state | GlobalVarState::INITIALIZING);
    }
  }

  // Get the session where we want to run global constructors, if different from the given session.
  // Refer to BuildSession::globalCtorSession for more info about this.
  auto ctorSession = session->getGlobalCtorSession();
  if (ctorSession == 0) ctorSession = session;

  // Prepare the function.
  TioSharedPtr tgContext;
  TioSharedPtr tgFunc;
  generation->prepareNullaryProcedure(funcName, ctorSession, tgFunc, tgContext);
  SharedPtr<DestructionStack> destructionStack = newSrdObj<DestructionStack>();
  SharedPtr<DependencyInfo> dependencyInfo = newSrdObj<CodeGen::DependencyInfo>();
  Session childSession(ctorSession, tgContext.get(), tgContext.get(), destructionStack, dependencyInfo);

  for (Int i = 0; i < deps->getCount(); ++i) {
    auto astVar = deps->get(i);
    TiObject *tgVar = childSession.getEda()->tryGetCodeGenData<TiObject>(astVar);

    // Get initialization params, if any.
    TiObject *astTypeRef = astVar;
    TiObject *astParams = 0;
    auto astParamPass = ti_cast<Core::Data::Ast::ParamPass>(astVar);
    if (astParamPass != 0 && astParamPass->getType() == Core::Data::Ast::BracketType::ROUND) {
      astTypeRef = astParamPass->getOperand().get();
      astParams = astParamPass->getParam().get();
    }

    // Get the type of the variable.
    TiObject *tgType;
    Ast::Type *astType;
    if (!generation->getGeneratedType(astTypeRef, &childSession, tgType, &astType)) {
      result = false;
      continue;
    }

    if (tgVar == 0) {
      // Generate the global var if it's not generated yet. This can happen if we use a different
      // session for the global constructors than the current session. This is needed when generating
      // JIT code but still needing to run the constructors using the preprocess session in order to
      // avoid cases where a global var is scheduled for initialization by the jit session but gets
      // referenced by the preprocess session before that.
      TioSharedPtr tgGlobalVar;
      auto mangledName = this->getGlobalVarMangledName(astVar);
      if (!childSession.getTg()->generateGlobalVariable(tgType, mangledName, 0, tgGlobalVar)) {
        childSession.getEda()->setCodeGenFailed(astVar, true);
        return false;
      }
      childSession.getEda()->setCodeGenData(astVar, tgGlobalVar);
      tgVar = tgGlobalVar.get();
    }

    // Init the var
    TioSharedPtr tgVarRef;
    if (!childSession.getTg()->generateVarReference(tgContext.get(), tgType, tgVar, tgVarRef)) {
      result = false;
      continue;
    }
    if (!varOpCallback(astType, tgVarRef.get(), astVar, astParams, &childSession)) {
      result = false;
      continue;
    }
  }

  // Finalize function.
  generation->finalizeNullaryProcedure(ctorSession, tgFunc.get(), tgContext.get());

  // Build the dependencies.
  if (!generation->buildDependencies(&childSession)) result = false;

  // Mark all global vars as initilzied/terminated.
  for (Int i = 0; i < deps->getCount(); ++i) {
    auto astVar = deps->get(i);
    Int state = this->getGlobalVarState(session, astVar);
    if (dtor) {
      this->setGlobalVarState(session, astVar, (state & ~GlobalVarState::TERMINATING) | GlobalVarState::TERMINATED);
    } else {
      this->setGlobalVarState(session, astVar, (state & ~GlobalVarState::INITIALIZING) | GlobalVarState::INITIALIZED);
    }
  }

  deps->clear();

  return result;
}


//==============================================================================
// Helper Functions

Str Generator::getTempVarName()
{
  return Str("#temp") + (LongInt)(this->tempVarIndex++);
}


void Generator::setGlobalVarState(Session *session, Core::Data::Node* astVar, Int state)
{
  if (!astVar->isA<Core::Data::Ast::Definition>()) astVar = astVar->getOwner();
  session->getEda()->setGlobalVarState<TiInt>(astVar, newSrdObj<TiInt>(state));
}


Int Generator::getGlobalVarState(Session *session, Core::Data::Node* astVar)
{
  if (!astVar->isA<Core::Data::Ast::Definition>()) astVar = astVar->getOwner();
  auto state = session->getEda()->tryGetGlobalVarState<TiInt>(astVar);
  return state == 0 ? 0 : state->get();
}


Str Generator::getGlobalVarMangledName(Core::Data::Node *astVar)
{
  if (!astVar->isA<Core::Data::Ast::Definition>()) astVar = astVar->getOwner();
  // We will prefix the name to make sure it doesn't conflict with names from imported C libs.
  auto name = getMangledName(astVar);
  if (name == "") {
    name = Str(S("!")) + this->astHelper->resolveNodePath(astVar);
    setMangledName(astVar, name);
  }
  return name;
}

} // namespace

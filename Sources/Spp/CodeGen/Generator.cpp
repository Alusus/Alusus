/**
 * @file Spp/CodeGen/Generator.cpp
 * Contains the implementation of class Spp::CodeGen::Generator.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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
}


//==============================================================================
// Main Operation Functions

void Generator::prepareBuild(Core::Notices::Store *noticeStore)
{
  VALIDATE_NOT_NULL(noticeStore);

  this->noticeStore = noticeStore;
  this->noticeStore->clearPrefixSourceLocationStack();

  this->astHelper->prepare(this->noticeStore);
  this->typeGenerator->setNoticeStore(this->noticeStore);
  this->commandGenerator->setNoticeStore(this->noticeStore);
  this->expressionGenerator->setNoticeStore(this->noticeStore);
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
      } else if (generator->getAstHelper()->isAstReference(target)) {
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

  if (generator->astProcessor != 0) {
    if (!generator->astProcessor->processFunctionBody(astFunc)) return false;
  }

  auto astBlock = astFunc->getBody().get();
  if (astBlock != 0 && session->getEda()->tryGetCodeGenData<TiObject>(astBlock) == 0) {
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

    DestructionStack destructionStack;
    Session childSession(session, tgContext.get(), tgContext.get(), &destructionStack);

    // Store the generated ret value reference, if needed.
    if (astRetType->getInitializationMethod(
      generator->astHelper, session->getExecutionContext()
    ) != Ast::TypeInitMethod::NONE) {
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
        return false;
      }
      session->getEda()->setCodeGenData(argType, argTgVar);
      Ast::Type *argSourceAstType;
      if (argAstType->getInitializationMethod(
        generator->astHelper, session->getExecutionContext()
      ) != Ast::TypeInitMethod::NONE) {
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
        return false;
      }
      if (!generation->generateVarInitialization(
        argAstType, argTgVarRef.get(), ti_cast<Core::Data::Node>(argType),
        &initAstNodes, &initAstTypes, &initTgVals, &childSession
      )) {
        return false;
      }
      generation->registerDestructor(
        ti_cast<Core::Data::Node>(argType), argAstType, session->getExecutionContext(),
        childSession.getDestructionStack()
      );
    }

    // Generate the function's statements.
    TerminalStatement terminal;
    auto retVal = generation->generateStatementBlock(astBlock, &childSession, terminal);

    // Does this function need to return a value?
    if (!generator->astHelper->isVoid(astRetType) && terminal != TerminalStatement::YES) {
      // A block could have been terminated by a block or continue statement instead of a return, but that's fine
      // since top level breaks and returns will raise an error anyway.
      generator->noticeStore->add(
        newSrdObj<Spp::Notices::MissingReturnStatementNotice>(astFunc->findSourceLocation())
      );
      return false;
    }

    // If there wasn't a return statement then we should destruct the variables.
    if (terminal != TerminalStatement::YES) {
      // Destruct function args first, then return.
      if (!generation->generateVarGroupDestruction(&childSession, 0)) return false;
    }

    // Finalize the body.
    if (!session->getTg()->finishFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext.get())) {
      return false;
    }

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
      // Generate a global or a static variable.
      // We will prefix the name to make sure it doesn't conflict with names from imported C libs.
      Str name = S("!");
      name += generator->getAstHelper()->resolveNodePath(definition);
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
        if (generator->globalItemRepo->findItem(name) == -1) {
          // Add an entry for the variable in the repo.
          Word size;
          if (!generator->typeGenerator->getTypeAllocationSize(astType, generation, session, size)) {
            session->getEda()->setCodeGenFailed(astVar, true);
            return false;
          }
          generator->globalItemRepo->addItem(name, size);
        } else {
          return true;
        }
      }

      if (astParams != 0 || astType->getInitializationMethod(
        generator->astHelper, session->getExecutionContext()
      ) != Ast::TypeInitMethod::NONE) {
        session->getGlobalVarInitializationDeps()->add(static_cast<Core::Data::Node*>(astVar));
      }

      if (astType->getDestructionMethod(
        generator->astHelper, session->getExecutionContext()
      ) != Ast::TypeInitMethod::NONE) {
        session->getGlobalVarDestructionDeps()->add(static_cast<Core::Data::Node*>(astVar));
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
        ti_cast<Core::Data::Node>(astVar), astType, session->getExecutionContext(), session->getDestructionStack()
      );
    }
  }

  return true;
}


Bool Generator::_generateTempVar(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::Type *astType, Session *session, Bool initialize
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  TiObject *tgVar = session->getEda()->tryGetCodeGenData<TiObject>(astNode);

  if (tgVar == 0) {
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

    Ast::setAstType(astNode, astType);

    Core::Data::Ast::Definition tempDef;
    tempDef.setOwner(astNode->getOwner());

    // At this point we should already have a TG context.
    if (session->getTgAllocContext() == 0) {
      throw EXCEPTION(GenericException, S("Missing TG allocation context."));
    }

    // Create the llvm local var.
    TioSharedPtr tgLocalVar;
    Str name = generator->getTempVarName();
    if (!session->getTg()->generateLocalVariable(session->getTgAllocContext(), tgType, name, 0, tgLocalVar)) {
      return false;
    }
    session->getEda()->setCodeGenData(astNode, tgLocalVar);

    if (initialize) {
      // Initialize the variable.
      // TODO: Should we use default values with local variables?
      // TioSharedPtr tgDefaultValue;
      // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
      //   if (!generator->typeGenerator->generateDefaultValue(
      //     astType, generation, tg, tgContext, tgDefaultValue
      //   )) return false;
      // }
      TioSharedPtr tgLocalVarRef;
      if (!session->getTg()->generateVarReference(session->getTgContext(), tgType, tgLocalVar.get(), tgLocalVarRef)) {
        return false;
      }
      SharedList<TiObject> initTgVals;
      PlainList<TiObject> initAstTypes;
      PlainList<TiObject> initAstNodes;
      if (!generation->generateVarInitialization(
        astType, tgLocalVar.get(), astNode, &initAstNodes, &initAstTypes, &initTgVals, session
      )) return false;

      generation->registerDestructor(astNode, astType, session->getExecutionContext(), session->getDestructionStack());
    }
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
  if (varAstType->getInitializationMethod(
    generator->getAstHelper(), session->getExecutionContext()
  ) != Ast::TypeInitMethod::NONE) {
    // Call automatic constructors, if any.
    auto tgAutoCtor = session->getEda()->tryGetAutoCtor<TiObject>(varAstType);
    if (tgAutoCtor != 0) {
      PlainList<TiObject> autoTgValues({ tgVarRef });
      TioSharedPtr dummy;
      if (!session->getTg()->generateFunctionCall(session->getTgContext(), tgAutoCtor, &autoTgValues, dummy)) {
        return false;
      }
    }

    // Add `this` to parameter list.
    auto varPtrAstType = generator->getAstHelper()->getReferenceTypeFor(varAstType, Ast::ReferenceMode::IMPLICIT);
    paramAstNodes->insertElement(0, astNode);
    paramAstTypes->insertElement(0, varPtrAstType);
    paramTgValues->insertElement(0, tgVarRef);

    // Do we have constructors matching the given vars?
    static Core::Data::Ast::Identifier ref({{ S("value"), TiStr(S("~init")) }});
    Ast::CalleeLookupResult calleeResult;
    if (generator->astHelper->lookupCalleeInScope(
      &ref, varAstType, false, 0, paramAstTypes, session->getExecutionContext(), calleeResult
    ) && calleeResult.stack.getCount() == 1) {
      auto callee = calleeResult.stack.get(calleeResult.stack.getCount() - 1);
      // Prepare the arguments to send.
      if (!generator->getExpressionGenerator()->prepareFunctionParams(
        static_cast<Ast::Function*>(callee)->getType().get(), generation, session,
        paramAstNodes, paramAstTypes, paramTgValues
      )) return false;
      // Call the found constructor.
      GenResult result;
      return generator->getExpressionGenerator()->generateFunctionCall(
        astNode, static_cast<Ast::Function*>(callee), paramAstTypes, paramTgValues, generation, session, result
      );
    } else if (paramAstTypes->getCount() != 1 || generator->getSeeker()->tryGet(&ref, varAstType) != 0) {
      // We have custom initialization but no constructors match the given params.
      generator->noticeStore->add(newSrdObj<Spp::Notices::TypeMissingMatchingInitOpNotice>(
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
        generator->noticeStore->add(newSrdObj<Spp::Notices::TypeMissingMatchingInitOpNotice>(
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
      generator->noticeStore->add(newSrdObj<Spp::Notices::TypeMissingMatchingInitOpNotice>(
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
  auto tgMemberVar = session->getEda()->getCodeGenData<TiObject>(astMemberNode);
  auto astMemberType = Ast::getAstType(astMemberNode);
  auto paramPass = ti_cast<Core::Data::Ast::ParamPass>(astMemberNode);
  if (!paramPass || paramPass->getType() != Core::Data::Ast::BracketType::ROUND) {
    if (astMemberType->getInitializationMethod(
      generator->getAstHelper(), session->getExecutionContext()
    ) == Ast::TypeInitMethod::NONE) {
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

  if (varAstType->getDestructionMethod(
    generator->getAstHelper(), session->getExecutionContext()
  ) == Ast::TypeInitMethod::NONE) {
    return true;
  }

  // Prepare param list.
  PlainList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  auto ptrAstType = generator->getAstHelper()->getReferenceTypeFor(varAstType, Ast::ReferenceMode::IMPLICIT);
  paramAstTypes.insertElement(0, ptrAstType);
  paramTgValues.insertElement(0, tgVarRef);

  // Find the destructor.
  static Core::Data::Ast::Identifier ref({{ S("value"), TiStr(S("~terminate")) }});
  Ast::CalleeLookupResult calleeResult;
  if (generator->astHelper->lookupCalleeInScope(
    &ref, varAstType, false, 0, &paramAstTypes, session->getExecutionContext(), calleeResult
  ) && calleeResult.stack.getCount() == 1) {
    auto callee = static_cast<Ast::Function*>(calleeResult.stack.get(calleeResult.stack.getCount() - 1));
    // Call the destructor.
    GenResult result;
    if (!generator->getExpressionGenerator()->generateFunctionCall(
      astNode, callee, &paramAstTypes, &paramTgValues, generation, session, result
    )) return false;
  }

  // Call auto destructor, if any.
  auto tgAutoDtor = session->getEda()->tryGetAutoDtor<TiObject>(varAstType);
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
  if (astMemberType->getDestructionMethod(
    generator->getAstHelper(), session->getExecutionContext()
  ) == Ast::TypeInitMethod::NONE) {
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

  // Initialize the member variable.
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
  TiObject *self, Core::Data::Node *varAstNode, Ast::Type *astType, ExecutionContext const *ec,
  DestructionStack *destructionStack
) {
  PREPARE_SELF(generator, Generator);

  // Skip if the type has no custom destructors.
  auto astUserType = ti_cast<Ast::UserType>(astType);
  if (
    astUserType == 0 || astUserType->getDestructionMethod(generator->getAstHelper(), ec) == Ast::TypeInitMethod::NONE
  ) {
    return;
  }

  // Add the var node to the list.
  destructionStack->pushItem(varAstNode);
}


Bool Generator::_generateVarGroupDestruction(
  TiObject *self, Session *session, Int index
) {
  PREPARE_SELF(generation, Generation);

  for (; index < session->getDestructionStack()->getItemCount(); ++index) {
    auto astNode = session->getDestructionStack()->getItem(index);

    TiObject *tgVar = session->getEda()->getCodeGenData<TiObject>(astNode);
    auto astType = Ast::getAstType(astNode);
    if (tgVar == 0 || astType == 0) {
      throw EXCEPTION(GenericException, S("The provided node does not seem to be a variable."));
    }
    TioSharedPtr tgVarRef;
    if (!session->getTg()->generateVarReference(session->getTgContext(), astType, tgVar, tgVarRef)) {
      return false;
    }

    if (!generation->generateVarDestruction(astType, tgVarRef.get(), astNode, session)) return false;
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
        generator->noticeStore->add(
          newSrdObj<Spp::Notices::UnreachableCodeNotice>(Core::Data::Ast::findSourceLocation(astNode))
        );
        return false;
      }
      if (!generation->generateStatement(astNode, session, terminal)) result = false;
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
    if (target->isDerivedFrom<Spp::Ast::Module>()) {
      retVal = generation->generateModuleInit(static_cast<Spp::Ast::Module*>(target), session);
    // } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
    //   // TODO: Generate function.
    //   generator->noticeStore->add(
    //     newSrdObj<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
    //   );
    //   retVal = false;
    // } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
    //   // TODO: Generate type.
    //   generator->noticeStore->add(
    //     newSrdObj<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
    //   );
    //   retVal = false;
    } else if (generator->getAstHelper()->isAstReference(target)) {
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
    retVal = generation->generateExpression(astNode, session, result);

    if (!generation->generateVarGroupDestruction(session, session->getDestructionStack()->getScopeStartIndex(-1))) {
      retVal = false;
    }
    session->getDestructionStack()->popScope();
  }

  return retVal;
}


Bool Generator::_generateExpression(
  TiObject *self, TiObject *astNode, Session *session, GenResult &result
) {
  PREPARE_SELF(generator, Generator);
  return generator->expressionGenerator->generate(astNode, ti_cast<Generation>(self), session, result);
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


//==============================================================================
// Helper Functions

Str Generator::getTempVarName()
{
  return Str("#temp") + (LongInt)(this->tempVarIndex++);
}

} // namespace

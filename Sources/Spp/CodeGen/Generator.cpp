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

namespace Spp::CodeGen
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
  generation->generateVarDestruction = &Generator::_generateVarDestruction;
  generation->registerDestructor = &Generator::_registerDestructor;
  generation->generateVarGroupDestruction = &Generator::_generateVarGroupDestruction;
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

Bool Generator::_generateModules(TiObject *self, Core::Data::Ast::Scope *root, GenDeps const &deps)
{
  PREPARE_SELF(generation, Generation);

  Bool result = true;
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->getElement(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        if (!generation->generateModule(module, deps)) result = false;
      }
    }
  }

  return result;
}


Bool Generator::_generateModule(TiObject *self, Spp::Ast::Module *astModule, GenDeps const &deps)
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
        if (!generation->generateModule(static_cast<Spp::Ast::Module*>(target), deps)) result = false;
      } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
        if (!generation->generateFunction(static_cast<Spp::Ast::Function*>(target), deps)) result = false;
      } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
        if (!generation->generateUserTypeBody(static_cast<Spp::Ast::UserType*>(target), deps)) result = false;
      } else if (generator->getAstHelper()->isAstReference(target)) {
        // Generate global variable.
        if (!generation->generateVarDef(def, deps)) {
          result = false;
        }
      }
    } else if (obj->isDerivedFrom<Core::Data::Ast::Bridge>()) {
      if (!generator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(obj))) result = false;
    }
  }
  return result;
}


Bool Generator::_generateFunction(TiObject *self, Spp::Ast::Function *astFunc, GenDeps const &deps)
{
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(generator);

  auto tgFunc = tryGetCodeGenData<TiObject>(astFunc);
  if (tgFunc == 0) {
    if (!generation->generateFunctionDecl(astFunc, deps)) return false;
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

    // Prepare the function body.
    SharedList<TiObject> tgVars;
    TioSharedPtr tgContext;
    if (!deps.tg->prepareFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext)) return false;

    DestructionStack destructionStack;
    GenDeps childDeps(deps, tgContext.get(), &destructionStack);

    // Store the generated ret value reference, if needed.
    if (astRetType->hasCustomInitialization(generator->astHelper, deps.tg->getExecutionContext())) {
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
      if (!deps.tg->generateLocalVariable(tgContext.get(), argTgType, astArgs->getElementKey(i).c_str(), 0, argTgVar)) {
        return false;
      }
      setCodeGenData(argType, argTgVar);
      Ast::Type *argSourceAstType;
      if (argAstType->hasCustomInitialization(generator->astHelper, deps.tg->getExecutionContext())) {
        argSourceAstType = generator->astHelper->getReferenceTypeFor(argAstType);
      } else {
        argSourceAstType = argAstType;
      }
      SharedList<TiObject> initTgVals;
      PlainList<TiObject> initAstTypes;
      initTgVals.clear();
      initTgVals.add(tgVars.get(i));
      initAstTypes.clear();
      initAstTypes.add(argSourceAstType);
      TioSharedPtr argTgVarRef;
      if (!deps.tg->generateVarReference(tgContext.get(), argTgType, argTgVar.get(), argTgVarRef)) {
        return false;
      }
      if (!generation->generateVarInitialization(
        argAstType, argTgVarRef.get(), ti_cast<Core::Data::Node>(argType), &initAstTypes, &initTgVals, childDeps
      )) {
        return false;
      }
    }

    // Generate the function's statements.
    TerminalStatement terminal;
    auto retVal = generation->generateStatements(astBlock, childDeps, terminal);

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
    if (!deps.tg->finishFunctionBody(tgFunc, tgFuncType, &tgVars, tgContext.get())) {
      return false;
    }

    return retVal;
  }
  return true;
}


Bool Generator::_generateFunctionDecl(TiObject *self, Spp::Ast::Function *astFunc, GenDeps const &deps)
{
  PREPARE_SELF(generator, Generator);

  auto tgFunc = tryGetCodeGenData<TiObject>(astFunc);
  if (tgFunc != 0) return true;

  // Generate function type.
  TiObject *tgFunctionType;
  if (!generator->typeGenerator->getGeneratedType(astFunc->getType().get(), deps.tg, tgFunctionType, 0)) {
    return false;
  }

  // Generate the function object.
  Str name = generator->astHelper->getFunctionName(astFunc);
  TioSharedPtr tgFuncResult;
  if (!deps.tg->generateFunctionDecl(name.c_str(), tgFunctionType, tgFuncResult)) return false;
  setCodeGenData(astFunc, tgFuncResult);

  // TODO: Do we need these attributes?
  // if (astFunc->getBody() == 0) {
  //   llvmFunc->addFnAttr(llvm::Attribute::NoCapture);
  //   llvmFunc->addFnAttr(llvm::Attribute::NoUnwind);
  // }

  return true;
}


Bool Generator::_generateUserTypeBody(TiObject *self, Spp::Ast::UserType *astType, GenDeps const &deps)
{
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  TiObject *tgType;
  if (!generator->typeGenerator->getGeneratedType(astType, deps.tg, tgType, 0)) return false;
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
          if (!generation->generateVarDef(def, deps)) result = false;
        } else if (obj->isDerivedFrom<Spp::Ast::Function>()) {
          if (!generation->generateFunction(static_cast<Spp::Ast::Function*>(obj), deps)) result = false;
        }
      }
    }
  }

  return true;
}


Bool Generator::_generateVarDef(TiObject *self, Core::Data::Ast::Definition *definition, GenDeps const &deps)
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
    if (!generator->typeGenerator->getGeneratedType(astVar, deps.tg, tgType, &astType)) {
      setCodeGenFailed(astVar, true);
      return false;
    }

    // Also generate the reference type of this type.
    Ast::Type *astRefType = generator->astHelper->getPointerTypeFor(astType);
    if (astRefType == 0) {
      throw EXCEPTION(GenericException, S("Could not find reference type for the given var type."));
    }
    TiObject *tgRefType;
    if (!generator->typeGenerator->getGeneratedType(astRefType, deps.tg, tgRefType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate pointer type for the given var type."));
    }

    Ast::setAstType(astVar, astType);

    if (generator->getAstHelper()->getDefinitionDomain(definition) == Ast::DefinitionDomain::GLOBAL) {
      // Generate a global or a static variable.
      Str name = generator->getAstHelper()->resolveNodePath(definition);
      TioSharedPtr tgGlobalVar;

      // Generate the default value.
      TioSharedPtr tgDefaultValue;
      if (generator->offlineExecution) {
        if (!generator->typeGenerator->generateDefaultValue(astType, deps, tgDefaultValue)) {
          setCodeGenFailed(astVar, true);
          return false;
        }
      }
      // Create the llvm global var.
      if (!deps.tg->generateGlobalVariable(tgType, name.c_str(), tgDefaultValue.get(), tgGlobalVar)) {
        setCodeGenFailed(astVar, true);
        return false;
      }
      setCodeGenData(astVar, tgGlobalVar);

      if (!generator->offlineExecution) {
        if (generator->globalItemRepo->findItem(name.c_str()) == -1) {
          // Add an entry for the variable in the repo.
          Word size;
          if (!generator->typeGenerator->getTypeAllocationSize(astType, deps.tg, size)) {
            setCodeGenFailed(astVar, true);
            return false;
          }
          generator->globalItemRepo->addItem(name.c_str(), size);
        } else {
          return true;
        }
      }

      // Initialize the variable.
      TioSharedPtr tgGlobalVarRef;
      if (!deps.tg->generateVarReference(deps.tgGlobalConstructionContext, tgType, tgGlobalVar.get(), tgGlobalVarRef)) {
        return false;
      }
      SharedList<TiObject> initTgVals;
      PlainList<TiObject> initAstTypes;
      if (!generation->generateVarInitialization(
        astType, tgGlobalVarRef.get(), ti_cast<Core::Data::Node>(astVar), &initAstTypes, &initTgVals,
        GenDeps(deps, deps.tgGlobalConstructionContext, deps.globalDestructionStack)
      )) {
        return false;
      }

      generation->registerDestructor(
        ti_cast<Core::Data::Node>(astVar), astType, deps.tg->getExecutionContext(), deps.globalDestructionStack
      );
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
        if (!deps.tg->generateLocalVariable(tgFuncContext, tgType, definition->getName().get(), 0, tgLocalVar)) {
          setCodeGenFailed(astVar, true);
          return false;
        }
        setCodeGenData(astVar, tgLocalVar);

        // Initialize the variable.
        // TODO: Should we use default values with local variables?
        // TioSharedPtr tgDefaultValue;
        // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
        //   if (!generator->typeGenerator->generateDefaultValue(astType, deps, tgDefaultValue)) return false;
        // }
        TioSharedPtr tgLocalVarRef;
        if (!deps.tg->generateVarReference(deps.tgContext, tgType, tgLocalVar.get(), tgLocalVarRef)) {
          return false;
        }
        SharedList<TiObject> initTgVals;
        PlainList<TiObject> initAstTypes;
        if (!generation->generateVarInitialization(
          astType, tgLocalVarRef.get(), definition, &initAstTypes, &initTgVals, deps
        )) return false;

        generation->registerDestructor(
          ti_cast<Core::Data::Node>(astVar), astType, deps.tg->getExecutionContext(), deps.destructionStack
        );
      }
    }
  }

  return true;
}


Bool Generator::_generateTempVar(
  TiObject *self, Core::Data::Node *astNode, Spp::Ast::Type *astType, GenDeps const &deps, Bool initialize
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  TiObject *tgVar = tryGetCodeGenData<TiObject>(astNode);

  if (tgVar == 0) {
    // Generate the type of the variable.
    TiObject *tgType;
    if (!generator->typeGenerator->getGeneratedType(astType, deps.tg, tgType, 0)) return false;

    // Also generate the reference type of this type.
    Ast::Type *astRefType = generator->astHelper->getPointerTypeFor(astType);
    if (astRefType == 0) {
      throw EXCEPTION(GenericException, S("Could not find reference type for the given var type."));
    }
    TiObject *tgRefType;
    if (!generator->typeGenerator->getGeneratedType(astRefType, deps.tg, tgRefType, 0)) {
      throw EXCEPTION(GenericException, S("Failed to generate pointer type for the given var type."));
    }

    Ast::setAstType(astNode, astType);

    Core::Data::Ast::Definition tempDef;
    tempDef.setOwner(astNode->getOwner());

    // if (generator->getAstHelper()->getDefinitionDomain(&tempDef) == Ast::DefinitionDomain::GLOBAL) {
    //   // TODO: Is there a global temp var?
    //   // Generate a global or a static variable.
    //   Str name = generator->getTempVarName();
    //   TioSharedPtr tgGlobalVar;
    //   if (generator->offlineExecution) {
    //     // Generate the default value.
    //     TioSharedPtr tgDefaultValue;
    //     if (!generator->typeGenerator->generateDefaultValue(astType, tg, 0, tgDefaultValue)) return false;
    //     // Create the llvm global var.
    //     if (!tg->generateGlobalVariable(tgType, name.c_str(), tgDefaultValue.get(), tgGlobalVar)) return false;
    //   } else {
    //     // Create the llvm global var.
    //     if (!tg->generateGlobalVariable(tgType, name.c_str(), 0, tgGlobalVar)) return false;
    //     // Add an entry for the variable in the repo.
    //     Word size;
    //     if (!generator->typeGenerator->getTypeAllocationSize(astType, tg, size)) return false;
    //     generator->globalItemRepo->addItem(name.c_str(), size);
    //   }

    //   // TODO: Initialize the variable.
    //   // TODO: Add to destructor list if necessary.

    //   setCodeGenData(astNode, tgGlobalVar);
    // } else {
      auto astBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(astNode);
      if (ti_cast<Ast::Type>(astBlock->getOwner()) != 0) {
        // This should never happen.
        throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));
      } else {
        // Generate a local variable.

        // To avoid stack overflows we need to allocate at the function or root level rather than any inner block.
        auto astAllocBlock = astBlock;
        while (
          astAllocBlock != 0 && astAllocBlock->getOwner() != 0 && ti_cast<Ast::Function>(astAllocBlock->getOwner()) == 0
        ) {
          astAllocBlock = Core::Data::findOwner<Core::Data::Ast::Scope>(astAllocBlock->getOwner());
        }
        if (astAllocBlock == 0) throw EXCEPTION(GenericException, S("Unexpected error while generating variable."));

        // At this point we should already have a TG context.
        auto tgAllocContext = getCodeGenData<TiObject>(astAllocBlock);

        // Create the llvm local var.
        TioSharedPtr tgLocalVar;
        Str name = generator->getTempVarName();
        if (!deps.tg->generateLocalVariable(tgAllocContext, tgType, name.c_str(), 0, tgLocalVar)) return false;
        setCodeGenData(astNode, tgLocalVar);

        if (initialize) {
          // Initialize the variable.
          // TODO: Should we use default values with local variables?
          // TioSharedPtr tgDefaultValue;
          // if (!astType->isDerivedFrom<Ast::ArrayType>() && !astType->isDerivedFrom<Ast::UserType>()) {
          //   if (!generator->typeGenerator->generateDefaultValue(astType, tg, tgContext, tgDefaultValue)) return false;
          // }
          auto tgContext = getCodeGenData<TiObject>(astBlock);
          TioSharedPtr tgLocalVarRef;
          if (!deps.tg->generateVarReference(tgContext, tgType, tgLocalVar.get(), tgLocalVarRef)) {
            return false;
          }
          SharedList<TiObject> initTgVals;
          PlainList<TiObject> initAstTypes;
          if (!generation->generateVarInitialization(
            astType, tgLocalVar.get(), astNode, &initAstTypes, &initTgVals, deps
          )) return false;

          generation->registerDestructor(astNode, astType, deps.tg->getExecutionContext(), deps.destructionStack);
        }
      }
    // }
  }

  return true;
}


Bool Generator::_generateVarInitialization(
  TiObject *self, Spp::Ast::Type *varAstType, TiObject *tgVarRef, Core::Data::Node *paramsAstNode,
  PlainList<TiObject> *paramAstTypes, SharedList<TiObject> *paramTgValues,
  GenDeps const &deps
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  // Add `this` to parameter list.
  auto varPtrAstType = generator->getAstHelper()->getPointerTypeFor(varAstType);
  paramAstTypes->insertElement(0, varPtrAstType);
  paramTgValues->insertElement(0, tgVarRef);

  // Do we have custom initialization?
  if (varAstType->hasCustomInitialization(generator->getAstHelper(), deps.tg->getExecutionContext())) {
    // If we have only one arg and it's a reference of the same type, then we should pass a pointer rather than the
    // value. This is to avoid infinite recursion due to needing to call the constructor to construct the constructor's
    // value-type argument.
    if (paramAstTypes->getCount() == 2) {
      auto firstArgType = ti_cast<Ast::Type>(paramAstTypes->get(1));
      if (generator->getAstHelper()->isReferenceTypeFor(firstArgType, varAstType, deps.tg->getExecutionContext())) {
        paramAstTypes->set(1, generator->getAstHelper()->getPointerTypeFor(varAstType));
      }
    }

    // Do we have constructors matching the given vars?
    static Core::Data::Ast::Identifier ref({{ S("value"), TiStr(S("construct")) }});
    TiObject *callee;
    Ast::Type *calleeType;
    SharedPtr<Core::Notices::Notice> notice;
    if (generator->astHelper->lookupCallee(
      &ref, varAstType, false, paramAstTypes, deps.tg->getExecutionContext(), callee, calleeType, notice
    )) {
      // TODO: Call automatic (inlined) constructors, if any.

      // Call the found constructor.
      GenResult result;
      return generator->getExpressionGenerator()->generateFunctionCall(
        paramsAstNode, static_cast<Ast::Function*>(callee), paramAstTypes, paramTgValues, generation, deps, result
      );
    } else {
      // We have custom initialization but no constructors match the given params.
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::NoCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(paramsAstNode))
      );
      return false;
    }
  } else {
    // Else do we have a single arg? If so, cast then copy.
    if (paramAstTypes->getCount() == 2) {
      // Cast the value to var type.
      auto paramAstType = generator->getAstHelper()->traceType(paramAstTypes->getElement(1));
      ASSERT(paramAstType);
      if (!paramAstType->isImplicitlyCastableTo(
        varAstType, generator->astHelper, deps.tg->getExecutionContext()
      )) {
        generator->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidReturnValueNotice>(Core::Data::Ast::findSourceLocation(paramsAstNode))
        );
        return false;
      }
      TioSharedPtr tgCastedValue;
      if (!generation->generateCast(
        deps, paramAstType, varAstType, paramTgValues->getElement(1), tgCastedValue)
      ) {
        // This should not happen since non-castable calls should be filtered out earlier.
        throw EXCEPTION(GenericException, S("Invalid cast was unexpectedly found."));
      }

      // Copy the value into the var.
      auto varTgType = getCodeGenData<TiObject>(varAstType);
      TioSharedPtr assignTgRes;
      if (!deps.tg->generateAssign(deps.tgContext, varTgType, tgCastedValue.get(), tgVarRef, assignTgRes)) {
        return false;
      }
    } else if (paramAstTypes->getCount() > 1) {
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::NoCalleeMatchNotice>(Core::Data::Ast::findSourceLocation(paramsAstNode))
      );
      return false;
    } else {
      // TODO: Else call automatic (inlined) constructors, if any?
    }
  }

  return true;
}


Bool Generator::_generateVarDestruction(
  TiObject *self, Core::Data::Node *astNode, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(generator, Generator);
  PREPARE_SELF(generation, Generation);

  TiObject *tgVar = getCodeGenData<TiObject>(astNode);
  auto astType = Ast::getAstType(astNode);
  if (tgVar == 0 || astType == 0) {
    throw EXCEPTION(GenericException, S("The provided node does not seem to be a variable."));
  }
  TioSharedPtr tgVarRef;
  if (!deps.tg->generateVarReference(deps.tgContext, astType, tgVar, tgVarRef)) {
    return false;
  }

  // Prepare param list.
  PlainList<TiObject> paramTgValues;
  PlainList<TiObject> paramAstTypes;
  auto ptrAstType = generator->getAstHelper()->getPointerTypeFor(astType);
  paramAstTypes.insertElement(0, ptrAstType);
  paramTgValues.insertElement(0, tgVarRef.get());

  // Find the destructor.
  static Core::Data::Ast::Identifier ref({{ S("value"), TiStr(S("destruct")) }});
  TiObject *callee;
  Ast::Type *calleeType;
  SharedPtr<Core::Notices::Notice> notice;
  if (!generator->astHelper->lookupCallee(
    &ref, astType, false, &paramAstTypes, deps.tg->getExecutionContext(), callee, calleeType, notice
  )) {
    // No destructor is found for this object, so just return.
    return true;
  }

  // Call the destructor.
  return generator->getExpressionGenerator()->generateFunctionCall(
    astNode, static_cast<Ast::Function*>(callee), &paramAstTypes, &paramTgValues, generation, deps, result
  );
}


void Generator::_registerDestructor(
  TiObject *self, Core::Data::Node *varAstNode, Ast::Type *astType, ExecutionContext const *ec,
  DestructionStack *destructionStack
) {
  PREPARE_SELF(generator, Generator);

  // Skip if the type has no custom destructors.
  auto astUserType = ti_cast<Ast::UserType>(astType);
  if (astUserType == 0 || !astUserType->hasCustomDestruction(generator->getAstHelper(), ec)) return;

  // Add the var node to the list.
  destructionStack->pushItem(varAstNode);
}


Bool Generator::_generateVarGroupDestruction(
  TiObject *self, GenDeps const &deps, Int index
) {
  PREPARE_SELF(generation, Generation);

  for (; index < deps.destructionStack->getItemCount(); ++index) {
    auto astNode = deps.destructionStack->getItem(index);
    GenResult result;
    if (!generation->generateVarDestruction(astNode, deps, result)) return false;
  }

  return true;
}


Bool Generator::_generateStatements(
  TiObject *self, Core::Data::Ast::Scope *astBlock, GenDeps const &deps, TerminalStatement &terminal
) {
  PREPARE_SELF(generation, Generation);
  Bool result = true;
  terminal = TerminalStatement::UNKNOWN;
  deps.destructionStack->pushScope();
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
    if (!generation->generateStatement(astNode, deps, terminal)) result = false;
  }
  if (terminal != TerminalStatement::YES) {
    if (!generation->generateVarGroupDestruction(deps, deps.destructionStack->getScopeStartIndex(-1))) result = false;
  }
  deps.destructionStack->popScope();

  return result;
}


Bool Generator::_generateStatement(
  TiObject *self, TiObject *astNode, GenDeps const &deps, TerminalStatement &terminal
) {
  PREPARE_SELF(generator, Generator);
  auto generation = ti_cast<Generation>(generator);

  terminal = TerminalStatement::NO;
  Bool retVal = true;

  if (astNode->isDerivedFrom<Core::Data::Ast::Definition>()) {
    auto def = static_cast<Core::Data::Ast::Definition*>(astNode);
    auto target = def->getTarget().get();
    if (target->isDerivedFrom<Spp::Ast::Module>()) {
      generator->noticeStore->add(std::make_shared<Spp::Notices::InvalidOperationNotice>(def->findSourceLocation()));
      retVal = false;
    } else if (target->isDerivedFrom<Spp::Ast::Function>()) {
      // TODO: Generate function.
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
      );
      retVal = false;
    } else if (target->isDerivedFrom<Spp::Ast::UserType>()) {
      // TODO: Generate type.
      generator->noticeStore->add(
        std::make_shared<Spp::Notices::UnsupportedOperationNotice>(def->findSourceLocation())
      );
      retVal = false;
    } else if (generator->getAstHelper()->isAstReference(target)) {
      // Generate local variable.
      retVal = generation->generateVarDef(def, deps);
    } else {
      // TODO: Make sure the definition is a literal.
      retVal = true;
    }
  } else if (astNode->isDerivedFrom<Spp::Ast::IfStatement>()) {
    auto ifStatement = static_cast<Spp::Ast::IfStatement*>(astNode);
    retVal = generator->commandGenerator->generateIfStatement(ifStatement, generation, deps, terminal);
  } else if (astNode->isDerivedFrom<Spp::Ast::WhileStatement>()) {
    auto whileStatement = static_cast<Spp::Ast::WhileStatement*>(astNode);
    retVal = generator->commandGenerator->generateWhileStatement(whileStatement, generation, deps);
  } else if (astNode->isDerivedFrom<Spp::Ast::ForStatement>()) {
    auto forStatement = static_cast<Spp::Ast::ForStatement*>(astNode);
    retVal = generator->commandGenerator->generateForStatement(forStatement, generation, deps);
  } else if (astNode->isDerivedFrom<Spp::Ast::ContinueStatement>()) {
    terminal = TerminalStatement::YES;
    auto continueStatement = static_cast<Spp::Ast::ContinueStatement*>(astNode);
    retVal = generator->commandGenerator->generateContinueStatement(continueStatement, generation, deps);
  } else if (astNode->isDerivedFrom<Spp::Ast::BreakStatement>()) {
    terminal = TerminalStatement::YES;
    auto breakStatement = static_cast<Spp::Ast::BreakStatement*>(astNode);
    retVal = generator->commandGenerator->generateBreakStatement(breakStatement, generation, deps);
  } else if (astNode->isDerivedFrom<Spp::Ast::ReturnStatement>()) {
    terminal = TerminalStatement::YES;
    auto returnStatement = static_cast<Spp::Ast::ReturnStatement*>(astNode);
    retVal = generator->commandGenerator->generateReturnStatement(returnStatement, generation, deps);
  } else if (astNode->isDerivedFrom<Core::Data::Ast::Bridge>()) {
    retVal = generator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(astNode));
  } else {
    GenResult result;
    retVal = generation->generateExpression(astNode, deps, result);
  }
  return retVal;
}


Bool Generator::_generateExpression(
  TiObject *self, TiObject *astNode, GenDeps const &deps, GenResult &result
) {
  PREPARE_SELF(generator, Generator);
  return generator->expressionGenerator->generate(astNode, ti_cast<Generation>(self), deps, result);
}


Bool Generator::_generateCast(
  TiObject *self, GenDeps const &deps, Spp::Ast::Type *srcType, Spp::Ast::Type *destType,
  TiObject *tgValue, TioSharedPtr &tgCastedValue
) {
  PREPARE_SELF(generator, Generator);
  return generator->typeGenerator->generateCast(deps, srcType, destType, tgValue, tgCastedValue);
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

} // namespace

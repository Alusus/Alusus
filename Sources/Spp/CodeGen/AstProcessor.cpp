/**
 * @file Spp/CodeGen/AstProcessor.cpp
 * Contains the implementation of class Spp::CodeGen::AstProcessor.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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

void AstProcessor::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->process,
    &this->processParamPass,
    &this->processEvalStatement,
    &this->processMemberFunctionSig,
    &this->processFunctionBody,
    &this->processTypeBody,
    &this->processMacro,
    &this->applyMacroArgs,
    &this->insertInterpolatedAst,
    &this->interpolateAst,
    &this->interpolateAst_identifier,
    &this->interpolateAst_stringLiteral,
    &this->interpolateAst_tiStr,
    &this->interpolateAst_other,
    &this->interpolateAst_binding,
    &this->interpolateAst_containing,
    &this->interpolateAst_dynContaining,
    &this->interpolateAst_dynMapContaining
  });
}


void AstProcessor::initBindings()
{
  this->process = &AstProcessor::_process;
  this->processParamPass = &AstProcessor::_processParamPass;
  this->processEvalStatement = &AstProcessor::_processEvalStatement;
  this->processMemberFunctionSig = &AstProcessor::_processMemberFunctionSig;
  this->processFunctionBody = &AstProcessor::_processFunctionBody;
  this->processTypeBody = &AstProcessor::_processTypeBody;
  this->processMacro = &AstProcessor::_processMacro;
  this->applyMacroArgs = &AstProcessor::_applyMacroArgs;
  this->insertInterpolatedAst = &AstProcessor::_insertInterpolatedAst;
  this->interpolateAst = &AstProcessor::_interpolateAst;
  this->interpolateAst_identifier = &AstProcessor::_interpolateAst_identifier;
  this->interpolateAst_stringLiteral = &AstProcessor::_interpolateAst_stringLiteral;
  this->interpolateAst_tiStr = &AstProcessor::_interpolateAst_tiStr;
  this->interpolateAst_other = &AstProcessor::_interpolateAst_other;
  this->interpolateAst_binding = &AstProcessor::_interpolateAst_binding;
  this->interpolateAst_containing = &AstProcessor::_interpolateAst_containing;
  this->interpolateAst_dynContaining = &AstProcessor::_interpolateAst_dynContaining;
  this->interpolateAst_dynMapContaining = &AstProcessor::_interpolateAst_dynMapContaining;
}


//==============================================================================
// Main Functions

void AstProcessor::preparePass(Core::Notices::Store *noticeStore)
{
  this->setNoticeStore(noticeStore);
  this->getAstHelper()->prepare(noticeStore);
}


Bool AstProcessor::_process(TiObject *self, TiObject *owner)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(owner);

  if (owner == 0 || owner->isDerivedFrom<Core::Data::Grammar::Module>()) return true;

  auto container = ti_cast<Containing<TiObject>>(owner);
  if (container == 0) return true;

  Bool result = true;
  for (Int i = 0; i < container->getElementCount(); ++i) {
    auto child = container->getElement(i);
    if (child == 0) continue;

    if (child->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
      Bool replaced = false;
      if (!astProcessor->processParamPass(static_cast<Core::Data::Ast::ParamPass*>(child), i, replaced)) result = false;
      if (replaced) --i;
    } else if (child->isDerivedFrom<Ast::Function>()) {
      if (
        astProcessor->astHelper->getFunctionDomain(child) == Ast::DefinitionDomain::OBJECT
      ) {
        if (!astProcessor->processMemberFunctionSig(static_cast<Ast::Function*>(child)->getType().get())) {
          result = false;
        }
      }
    } else if (child->isDerivedFrom<Ast::Type>()) {
      // if (!astProcessor->processTypeMethodSigs(static_cast<Ast::UserType*>(child))) result = false;
      continue;
    } else if (child->isDerivedFrom<Ast::EvalStatement>()) {
      if (!astProcessor->processEvalStatement(static_cast<Ast::EvalStatement*>(child), owner, i)) result = false;
      --i;
    } else {
      if (!astProcessor->process(child)) result = false;
    }
  }

  return result;
}


Bool AstProcessor::_processParamPass(
  TiObject *self, Core::Data::Ast::ParamPass *paramPass, TiInt indexInOwner, Bool &replaced
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(paramPass);

  replaced = false;

  if (paramPass->getType() == Core::Data::Ast::BracketType::SQUARE) {
    // Extract args.
    PlainList<TiObject> argsList;
    auto param = paramPass->getParam().get();
    Containing<TiObject> *args = &argsList;
    if (param) {
      if (param->isDerivedFrom<Core::Data::Ast::List>()) {
        args = static_cast<Core::Data::Ast::List*>(param);
      } else {
        argsList.add(param);
      }
    }

    // Find matching macro.
    Ast::Macro *macro = 0;
    Ast::PointerType *pointerType = 0;
    astProcessor->astHelper->getSeeker()->foreach(paramPass->getOperand().get(), paramPass->getOwner(),
      [=, &macro, &pointerType] (TiObject *obj, Core::Notices::Notice *ntc)->Core::Data::Seeker::Verb
      {
        if (ntc != 0) {
          return Core::Data::Seeker::Verb::MOVE;
        }

        auto m = ti_cast<Ast::Macro>(obj);
        if (m != 0 && m->matchCall(args, astProcessor->astHelper)) {
          macro = m;
          return Core::Data::Seeker::Verb::STOP;
        } else if (obj->isDerivedFrom<Ast::Template>()) {
          pointerType = static_cast<Ast::Template*>(obj)->getBody().ti_cast_get<Ast::PointerType>();
          if (pointerType != 0) return Core::Data::Seeker::Verb::STOP;
          else return Core::Data::Seeker::Verb::MOVE;
        } else {
          return Core::Data::Seeker::Verb::MOVE;
        }
      }, 0
    );

    Bool result = true;
    if (macro != 0) {
      auto sl = paramPass->findSourceLocation();
      astProcessor->noticeStore->pushPrefixSourceLocation(sl.get());
      if (astProcessor->processMacro(macro, args, paramPass->getOwner(), indexInOwner, sl.get())) replaced = true;
      else result = false;
      astProcessor->noticeStore->popPrefixSourceLocation(
        Core::Data::getSourceLocationRecordCount(sl.get())
      );
    } else if (pointerType != 0) {
      // Is this a function pointer?
      if (param->isDerivedFrom<Ast::FunctionType>()) {
        if (
          astProcessor->astHelper->getFunctionDomain(param) == Ast::DefinitionDomain::OBJECT
        ) {
          if (!astProcessor->processMemberFunctionSig(static_cast<Ast::FunctionType*>(param))) {
            result = false;
          }
        }
      }
    }
    return result;
  } else{
    return astProcessor->process(paramPass);
  }
}


Bool AstProcessor::_processEvalStatement(
  TiObject *self, Spp::Ast::EvalStatement *eval, TiObject *owner, TiInt indexInOwner
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (!astProcessor->process(eval->getBody().get())) return false;

  // Build the eval statement.
  SharedPtr<BuildSession> buildSession = astProcessor->executing->prepareBuild(
    astProcessor->getNoticeStore(), BuildManager::BuildType::EVAL, eval->getBody().get()
  );
  Bool result = true;
  auto block = eval->getBody().ti_cast_get<Ast::Block>();
  if (block != 0) {
    for (Int i = 0; i < block->getElementCount(); ++i) {
      auto childData = block->getElement(i);
      if (!astProcessor->executing->addElementToBuild(childData, buildSession.get())) result = false;
    }
  } else {
      if (!astProcessor->executing->addElementToBuild(eval->getBody().get(), buildSession.get())) result = false;
  }
  astProcessor->executing->finalizeBuild(astProcessor->getNoticeStore(), eval->getBody().get(), buildSession.get());

  astProcessor->currentEvalOwner = owner;
  astProcessor->currentEvalInsertionPosition = indexInOwner;
  astProcessor->currentEvalSourceLocation = eval->findSourceLocation();

  // Remove the eval statement.
  DynamicContaining<TiObject> *dynContainer;
  DynamicMapContaining<TiObject> *dynMapContainer;
  Containing<TiObject> *container;
  if ((dynContainer = ti_cast<DynamicContaining<TiObject>>(owner)) != 0) {
    dynContainer->removeElement(indexInOwner);
  } else if ((dynMapContainer = ti_cast<DynamicMapContaining<TiObject>>(owner)) != 0) {
    dynMapContainer->removeElement(indexInOwner);
  } else if ((container = ti_cast<Containing<TiObject>>(owner)) != 0) {
    container->setElement(indexInOwner, 0);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("owner"), S("Invalid owner type."));
  }

  // Execute the eval statement.
  if (result) {
    astProcessor->executing->execute(astProcessor->getNoticeStore(), buildSession.get());
  }

  return result;
}


Bool AstProcessor::_processMemberFunctionSig(TiObject *self, Spp::Ast::FunctionType *funcType)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  if (isAstProcessed(funcType)) return true;
  if (funcType->getArgTypes() == 0) {
    funcType->setArgTypes(Core::Data::Ast::Map::create());
  }
  // Make sure we don't have an arg named `this`.
  if (funcType->getArgTypes()->findIndex(S("this")) != -1) {
    astProcessor->noticeStore->add(
      newSrdObj<Spp::Notices::ThisRedefinedNotice>(funcType->findSourceLocation())
    );
    return false;
  }
  // Prepare this type.
  auto thisType = Core::Data::Ast::ParamPass::create({
    {S("sourceLocation"), funcType->findSourceLocation()},
    {S("type"), Core::Data::Ast::BracketType(Core::Data::Ast::BracketType::SQUARE)}
  }, {
    {S("operand"), Core::Data::Ast::Identifier::create({
      {S("sourceLocation"), funcType->findSourceLocation()},
      {S("value"), TiStr(S("iref"))}
    })},
    {S("param"), Spp::Ast::ThisTypeRef::create()}
  });
  // Add this arg.
  funcType->getArgTypes()->insert(0, S("this"), thisType);
  // Mark the function as processed.
  setAstProcessed(funcType, true);
  return true;
}


Bool AstProcessor::_processFunctionBody(TiObject *self, Spp::Ast::Function *func)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(func);
  auto body = func->getBody().get();
  if (body == 0) return true;
  if (isAstProcessed(body)) return true;
  Bool result = astProcessor->process(body);
  setAstProcessed(body, true);
  return result;
}


Bool AstProcessor::_processTypeBody(TiObject *self, Spp::Ast::UserType *type)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(type);
  auto body = type->getBody().get();
  if (body == 0) return true;
  if (isAstProcessed(body)) return true;
  Bool result = astProcessor->process(body);
  setAstProcessed(body, true);
  return result;
}


Bool AstProcessor::_processMacro(
  TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args, TiObject *owner, TiInt indexInOwner,
  Core::Data::SourceLocation *sl
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(macro, args, owner);

  // Apply macro args.
  TioSharedPtr macroInstance;
  if (!astProcessor->applyMacroArgs(macro, args, sl, macroInstance)) return false;
  if (macroInstance == 0) {
    astProcessor->noticeStore->add(
      newSrdObj<Spp::Notices::InvalidMacroNotice>(macro->findSourceLocation())
    );
    return false;
  }
  if (macroInstance->isDerivedFrom<Core::Data::Ast::Scope>()) {
    auto scope = macroInstance.s_cast_get<Core::Data::Ast::Scope>();
    if (scope->getCount() == 1) {
      auto child = scope->get(0);
      macroInstance = child;
    }
  }

  // Replace macro reference with the clone.
  if (owner->isDerivedFrom<Core::Data::Ast::Scope>() && macroInstance->isDerivedFrom<Core::Data::Ast::Scope>()) {
    // Merge the two scopes.
    auto ownerScope = static_cast<Core::Data::Ast::Scope*>(owner);
    auto instanceScope = macroInstance.s_cast_get<Core::Data::Ast::Scope>();
    ownerScope->remove(indexInOwner);
    Int index = indexInOwner;
    Core::Data::Ast::addPossiblyMergeableElements(
      instanceScope, ownerScope, index, astProcessor->astHelper->getSeeker(), astProcessor->noticeStore
    );
  } else {
    // Replace the instance.
    auto ownerContainer = ti_cast<Containing<TiObject>>(owner);
    if (ownerContainer == 0) {
      throw EXCEPTION(InvalidArgumentException, S("owner"), S("Invalid owner type."));
    }
    ownerContainer->setElement(indexInOwner, macroInstance.get());
  }

  return true;
}


Bool AstProcessor::_applyMacroArgs(
  TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args, Core::Data::SourceLocation *sl,
  TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(macro, args);

  if (macro->getArgCount() != args->getElementCount()) {
    throw EXCEPTION(GenericException, S("Invalid args passed to macro."));
  }

  Array<Str> argNames = macro->getArgTypes()->getKeys();
  return astProcessor->interpolateAst(macro->getBody().get(), &argNames, args, sl, result);
}


Bool AstProcessor::_insertInterpolatedAst(
  TiObject *self, TiObject *obj, Array<Str> const *argNames, Containing<TiObject> *args
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  TioSharedPtr result;
  if (!astProcessor->interpolateAst(obj, argNames, args, astProcessor->currentEvalSourceLocation.get(), result)) {
    return false;
  }
  // Insert the interpolated AST.
  if (astProcessor->currentEvalOwner->isDerivedFrom<Core::Data::Ast::Scope>()) {
    if (result->isDerivedFrom<Core::Data::Ast::Scope>()) {
      // Merge the two scopes.
      auto ownerScope = static_cast<Core::Data::Ast::Scope*>(astProcessor->currentEvalOwner);
      auto insertedScope = result.s_cast_get<Core::Data::Ast::Scope>();
      Core::Data::Ast::addPossiblyMergeableElements(
        insertedScope, ownerScope, astProcessor->currentEvalInsertionPosition,
        astProcessor->astHelper->getSeeker(), astProcessor->noticeStore
      );
    } else {
      // Add a single element to the scope
      auto ownerScope = static_cast<Core::Data::Ast::Scope*>(astProcessor->currentEvalOwner);
      Core::Data::Ast::addPossiblyMergeableElement(
        result.get(), ownerScope, astProcessor->currentEvalInsertionPosition,
        astProcessor->astHelper->getSeeker(), astProcessor->noticeStore
      );
    }
  } else {
    auto ownerContainer = ti_cast<Containing<TiObject>>(astProcessor->currentEvalOwner);
    if (ownerContainer == 0) {
      throw EXCEPTION(GenericException, S("Unexpected AST insert location."));
    }
    auto insertedScope = result.s_cast_get<Core::Data::Ast::Scope>();
    if (insertedScope != 0 && insertedScope->getCount() == 1) {
      ownerContainer->setElement(astProcessor->currentEvalInsertionPosition, insertedScope->getElement(0));
    } else {
      ownerContainer->setElement(astProcessor->currentEvalInsertionPosition, result.get());
    }
  }
  return true;
}


Bool AstProcessor::_interpolateAst(
  TiObject *self, TiObject *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (obj->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(obj);
    return astProcessor->interpolateAst_identifier(identifier, argNames, args, sl, result);
  }

  if (obj->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(obj);
    return astProcessor->interpolateAst_stringLiteral(stringLiteral, argNames, args, sl, result);
  }

  if (obj->isDerivedFrom<TiStr>()) {
    auto str = static_cast<TiStr*>(obj);
    return astProcessor->interpolateAst_tiStr(str, argNames, args, sl, result);
  }

  // It's not a replacable identifier, so we'll proceed with cloning the tree.
  return astProcessor->interpolateAst_other(obj, argNames, args, sl, result);
}


Bool AstProcessor::_interpolateAst_identifier(
  TiObject *self, Core::Data::Ast::Identifier *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  astProcessor->parseStringTemplate(obj->getValue().get(), var, 1000, prefixSize, suffix);
  auto index = argNames->findPos(Str(true, var));
  if (index != -1) {
    auto arg = args->getElement(index);
    if (prefixSize != 0 || suffix != 0) {
      // We have an identifier string template, so we need the matching arg to be an identifier as well.
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>()) {
        Char newVar[1000];
        astProcessor->generateStringFromTemplate(
          obj->getValue().get(), prefixSize, static_cast<Core::Data::Ast::Identifier*>(arg)->getValue().get(), suffix,
          newVar, 1000
        );
        result = Core::Data::Ast::Identifier::create({
          { S("value"), TiStr(newVar) },
          { S("sourceLocation"), obj->getSourceLocation() }
        });
        Core::Data::Ast::addSourceLocation(result.get(), sl);
      } else {
        astProcessor->noticeStore->add(
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      result = Core::Data::Ast::clone(args->getElement(index), sl);
    }
  } else {
    result = Core::Data::Ast::clone(obj, sl);
  }
  return true;
}


Bool AstProcessor::_interpolateAst_stringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  astProcessor->parseStringTemplate(obj->getValue().get(), var, 1000, prefixSize, suffix, S("{{"), S("}}"));
  auto index = argNames->findPos(Str(true, var));
  if (index != -1) {
    auto arg = args->getElement(index);
    if (prefixSize != 0 || suffix != 0) {
      // We have an identifier string template, so we need the matching arg to be an identifier as well.
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>() || arg->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
        Char newVar[1000];
        astProcessor->generateStringFromTemplate(
          obj->getValue().get(), prefixSize, static_cast<Core::Data::Ast::Text*>(arg)->getValue().get(), suffix,
          newVar, 1000
        );
        result = Core::Data::Ast::StringLiteral::create({
          { S("value"), TiStr(newVar) },
          { S("sourceLocation"), obj->getSourceLocation() }
        });
        Core::Data::Ast::addSourceLocation(result.get(), sl);
      } else {
        astProcessor->noticeStore->add(
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      result = Core::Data::Ast::clone(args->getElement(index), sl);
    }
  } else {
    result = Core::Data::Ast::clone(obj, sl);
  }
  return true;
}


Bool AstProcessor::_interpolateAst_tiStr(
  TiObject *self, TiStr *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  astProcessor->parseStringTemplate(obj->get(), var, 1000, prefixSize, suffix);
  auto index = argNames->findPos(Str(true, var));
  if (index != -1) {
    auto arg = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
    if (arg != 0) {
      Char newVar[1000];
      astProcessor->generateStringFromTemplate(obj->get(), prefixSize, arg->getValue().get(), suffix, newVar, 1000);
      result = TiStr::create(newVar);
    } else {
      astProcessor->noticeStore->add(
        newSrdObj<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
      );
      return false;
    }
  } else {
    result = TiStr::create(obj->get());
  }
  return true;
}


Bool AstProcessor::_interpolateAst_other(
  TiObject *self, TiObject *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (!obj->isDerivedFrom<Core::Data::Node>()) {
    // We won't clone basic data types.
    result = getSharedPtr(obj);
    return true;
  }

  auto factory = obj->getMyTypeInfo()->getFactory();
  if (!factory) {
    throw EXCEPTION(GenericException, S("A Node derived class is missing a type factory."));
  }
  result = factory->createShared();
  if (sl != 0) Core::Data::Ast::addSourceLocation(result.get(), sl);

  auto srcBinding = ti_cast<Binding>(obj);
  auto binding = result.ti_cast_get<Binding>();
  if (binding != 0) {
    if (!astProcessor->interpolateAst_binding(srcBinding, argNames, args, sl, binding)) return false;
  }

  auto srcDynMapContainer = ti_cast<DynamicMapContaining<TiObject>>(obj);
  auto dynMapContainer = result.ti_cast_get<DynamicMapContaining<TiObject>>();
  if (dynMapContainer != 0) {
    if (!astProcessor->interpolateAst_dynMapContaining(
      srcDynMapContainer, argNames, args, sl, dynMapContainer
    )) return false;
  }

  auto srcDynContainer = ti_cast<DynamicContaining<TiObject>>(obj);
  auto dynContainer = result.ti_cast_get<DynamicContaining<TiObject>>();
  if (dynContainer != 0) {
    if (!astProcessor->interpolateAst_dynContaining(
      srcDynContainer, argNames, args, sl, dynContainer
    )) return false;
  }

  auto srcContainer = ti_cast<Containing<TiObject>>(obj);
  auto container = result.ti_cast_get<Containing<TiObject>>();
  if (dynMapContainer == 0 && dynContainer == 0 && container != 0) {
    if (!astProcessor->interpolateAst_containing(
      srcContainer, argNames, args, sl, container
    )) return false;
  }

  return true;
}


Bool AstProcessor::_interpolateAst_binding(
  TiObject *self, Binding *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, Binding *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getMemberCount(); ++i) {
    if (obj->getMemberHoldMode(i) == HoldMode::VALUE && obj->getMemberNeededType(i) == TiStr::getTypeInfo()) {
      // replace string, if possible.
      Char var[1000];
      Word prefixSize = 0;
      Char const *suffix = 0;
      astProcessor->parseStringTemplate(obj->refMember<TiStr>(i).get(), var, 1000, prefixSize, suffix);
      Int index = argNames->findPos(Str(true, var));
      if (index != -1) {
        auto arg = args->getElement(index);
        Char const *text;
        if (arg->isDerivedFrom<Core::Data::Ast::Identifier>() || arg->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
          text = static_cast<Core::Data::Ast::Text*>(arg)->getValue().get();
        } else if (arg->isDerivedFrom<TiStr>()) {
          text = static_cast<TiStr*>(arg)->get();
        } else {
          auto elementSl = Core::Data::Ast::findSourceLocation(arg);
          astProcessor->noticeStore->add(
            newSrdObj<Spp::Notices::InvalidMacroArgNotice>(elementSl == 0 ? getSharedPtr(sl) : elementSl)
          );
          return false;
        }
        Char newVar[1000];
        astProcessor->generateStringFromTemplate(
          obj->refMember<TiStr>(i).get(), prefixSize, text, suffix, newVar, 1000
        );
        destObj->refMember<TiStr>(i) = newVar;
      } else {
        destObj->setMember(i, obj->getMember(i));
      }
    } else if (obj->getMemberHoldMode(i) == HoldMode::SHARED_REF) {
      // Clone the member.
      TioSharedPtr newChild;
      TiObject *child = obj->getMember(i);
      if (child != 0) {
        if (!astProcessor->interpolateAst(child, argNames, args, sl, newChild)) return false;
        destObj->setMember(i, newChild.get());
      }
    } else {
      destObj->setMember(i, obj->getMember(i));
    }
  }
  return true;
}


Bool AstProcessor::_interpolateAst_containing(
  TiObject *self, Containing<TiObject> *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, Containing<TiObject> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      TioSharedPtr newChild;
      TiObject *child = obj->getElement(i);
      if (child != 0) {
        if (!astProcessor->interpolateAst(child, argNames, args, sl, newChild)) return false;
      }
      destObj->setElement(i, newChild.get());
    } else {
      destObj->setElement(i, obj->getElement(i));
    }
  }
  return true;
}


Bool AstProcessor::_interpolateAst_dynContaining(
  TiObject *self, DynamicContaining<TiObject> *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, DynamicContaining<TiObject> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      TioSharedPtr newChild;
      TiObject *child = obj->getElement(i);
      if (child != 0) {
        if (!astProcessor->interpolateAst(child, argNames, args, sl, newChild)) return false;
      }
      destObj->addElement(newChild.get());
    } else {
      destObj->addElement(obj->getElement(i));
    }
  }
  return true;
}


Bool AstProcessor::_interpolateAst_dynMapContaining(
  TiObject *self, DynamicMapContaining<TiObject> *obj, Array<Str> const *argNames, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, DynamicMapContaining<TiObject> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    // Generate the new key value.
    Char key[1000];
    Char newKey[1000];
    Word prefixSize = 0;
    Char const *suffix = 0;
    astProcessor->parseStringTemplate(obj->getElementKey(i), key, 1000, prefixSize, suffix);
    auto index = argNames->findPos(Str(true, key));
    if (index != -1) {
      // The key can be replaced with the new key.
      auto arg = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
      if (arg != 0) {
        astProcessor->generateStringFromTemplate(
          obj->getElementKey(i), prefixSize, arg->getValue().get(), suffix, newKey, 1000
        );
        // Make sure the new key isn't already used.
        if (obj->findElementIndex(newKey) != -1) {
          astProcessor->noticeStore->add(
            newSrdObj<Spp::Notices::InvalidMacroArgNotice>(arg->findSourceLocation())
          );
          return false;
        }
        auto value = obj->getElement(index);
        obj->insertElement(index, newKey, value);
        obj->removeElement(index + 1);
      } else {
        astProcessor->noticeStore->add(
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      copyStr(obj->getElementKey(i), newKey);
    }
    // Generate the value.
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      TioSharedPtr newChild;
      TiObject *child = obj->getElement(i);
      if (child != 0) {
        if (!astProcessor->interpolateAst(child, argNames, args, sl, newChild)) return false;
      }
      destObj->addElement(newKey, newChild.get());
    } else {
      destObj->addElement(newKey, obj->getElement(i));
    }
  }
  return true;
}


//==============================================================================
// Helper Functions

void AstProcessor::parseStringTemplate(
  Char const *str, Char *var, Word varBufSize, Word &prefixSize, Char const *&suffix,
  Char const *varOpening, Char const *varClosing
) {
  auto varStart = SBSTR(str).findPos(varOpening) + 2;
  if (varStart >= 2) {
    auto varSize = SBSTR(str + varStart).findPos(varClosing);
    if (varSize > 0) {
      SBSTR(var).assign(str + varStart, varSize, varBufSize);
      prefixSize = varStart - 2;
      suffix = str + varStart + varSize + 2;
      return;
    }
  }
  SBSTR(var).assign(str, varBufSize);
  prefixSize = 0;
  suffix = 0;
}


void AstProcessor::generateStringFromTemplate(
  Char const *prefix, Word prefixSize, Char const *var, Char const *suffix, Char *output, Word outputBufSize
) {
  SbStr result(output);
  result.assign(S(""), outputBufSize);
  if (prefixSize > 0) result.append(prefix, prefixSize, outputBufSize);
  result.append(var, outputBufSize);
  if (suffix != 0) result.append(suffix, outputBufSize);
}

} // namespace

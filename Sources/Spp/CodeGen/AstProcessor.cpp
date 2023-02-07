/**
 * @file Spp/CodeGen/AstProcessor.cpp
 * Contains the implementation of class Spp::CodeGen::AstProcessor.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
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
    &this->processPreprocessStatement,
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
  this->processPreprocessStatement = &AstProcessor::_processPreprocessStatement;
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

Bool AstProcessor::_process(TiObject *self, TiObject *owner)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(owner);

  if (owner == 0 || owner->isDerivedFrom<Core::Data::Grammar::Module>()) return true;

  Bool result = true;

  auto def = ti_cast<Core::Data::Ast::Definition>(owner);
  if (def != 0 && def->isToMerge() && def->getTarget().ti_cast_get<Spp::Ast::Type>() != 0) {
    // Having a type definition with to-merge flag set means we eventually didn't find the merge target.
    astProcessor->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::MissingTypeMergeTargetNotice>(findSourceLocation(def))
    );
    // Reset the to-merge flag to avoid duplicate error messages.
    def->setToMerge(false);
    result = false;
  }

  auto container = ti_cast<Containing<TiObject>>(owner);
  if (container == 0) return result;

  for (Int i = 0; i < container->getElementCount(); ++i) {
    auto child = container->getElement(i);
    if (child == 0) continue;

    if (child->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
      Bool replaced = false;
      if (!astProcessor->processParamPass(static_cast<Core::Data::Ast::ParamPass*>(child), i, replaced)) result = false;
      if (replaced) --i;
    } else if (child->isDerivedFrom<Ast::Macro>()) {
      continue;
    } else if (child->isDerivedFrom<Ast::Type>()) {
      continue;
    } else if (child->isDerivedFrom<Ast::Function>()) {
      continue;
    } else if (child->isDerivedFrom<Ast::AstLiteralCommand>()) {
      if (!static_cast<Ast::AstLiteralCommand*>(child)->isPreprocessDisabled()) {
        if (!astProcessor->process(child)) result = false;
      }
    } else if (child->isDerivedFrom<Ast::PreprocessStatement>()) {
      if (astProcessor->processPreprocessStatement(static_cast<Ast::PreprocessStatement*>(child), owner, i)) --i;
      else result = false;
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
      if (param->isA<Core::Data::Ast::List>()) {
        args = static_cast<Core::Data::Ast::List*>(param);
      } else {
        argsList.add(param);
      }
    }

    Bool result = true;

    auto operand = paramPass->getOperand().get();
    if (!astProcessor->process(operand)) result = false;
    TiObject *owner;
    Word foreachFlags = 0;
    Bool member = false;

    // Find matching macro.
    if (operand->isDerivedFrom<Core::Data::Ast::LinkOperator>()) {
      auto linkOp = static_cast<Core::Data::Ast::LinkOperator*>(operand);
      auto thisArg = linkOp->getFirst().get();
      if (!astProcessor->expressionComputation->computeResultType(thisArg, owner, member)) return false;
      if (owner->isDerivedFrom<Spp::Ast::Type>()) {
        owner = astProcessor->astHelper->tryGetDeepReferenceContentType(static_cast<Spp::Ast::Type*>(owner));
        if (member) argsList.insert(0, thisArg);
      }
      operand = linkOp->getSecond().get();
      foreachFlags = Core::Data::Seeker::Flags::SKIP_OWNERS;
    } else {
      owner = paramPass->getOwner();
    }

    Ast::Macro *macro = 0;
    astProcessor->astHelper->getSeeker()->foreach(operand, owner,
      [=, &macro] (TiInt action, TiObject *obj)->Core::Data::Seeker::Verb
      {
        if (action != Core::Data::Seeker::Action::TARGET_MATCH) {
          return Core::Data::Seeker::Verb::MOVE;
        }

        auto m = ti_cast<Ast::Macro>(obj);
        if (m != 0 && m->isMember() == member && m->matchCall(args, astProcessor->astHelper)) {
          macro = m;
          return Core::Data::Seeker::Verb::STOP;
        } else {
          return Core::Data::Seeker::Verb::MOVE;
        }
      }, foreachFlags
    );

    if (macro != 0) {
      auto sl = paramPass->findSourceLocation();
      if (astProcessor->processMacro(macro, args, paramPass->getOwner(), indexInOwner, sl.get())) replaced = true;
      else result = false;
    }
    return result;
  } else{
    return astProcessor->process(paramPass);
  }
}


#define PREPROCESS_SESSION_KEY S("preprocessSession")

Bool AstProcessor::_processPreprocessStatement(
  TiObject *self, Spp::Ast::PreprocessStatement *preprocess, TiObject *owner, TiInt indexInOwner
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (!astProcessor->process(preprocess->getBody().get())) return false;

  // Capture a shared pointer to avoid segfault in case a dependency processes the same statement, or in case
  // elements from this preprocess statement are in use elsewhere, like in templates.
  astProcessor->astNodeRepo->addElement(preprocess);

  Bool result = true;

  // Build the preprocess statement.
  // Was the statement already built? This can happen when the same preprocess statement is encountered again while
  // finalizing the build of that preprocess statement. We'll store the build session on the element and re-use it
  // if encountered again in order to avoid double building the statement.
  SharedPtr<BuildSession> buildSession = preprocess->getExtra(PREPROCESS_SESSION_KEY).ti_cast<BuildSession>();
  if (buildSession == 0) {
    buildSession = astProcessor->executing->prepareBuild(
      BuildManager::BuildType::PREPROCESS, preprocess->getBody().get()
    );
    setExtra(preprocess, PREPROCESS_SESSION_KEY, buildSession);

    auto block = preprocess->getBody().ti_cast_get<Ast::Block>();
    if (block != 0) {
      for (Int i = 0; i < block->getElementCount(); ++i) {
        auto childData = block->getElement(i);
        if (!astProcessor->executing->addElementToBuild(childData, buildSession.get())) result = false;
      }
    } else {
        if (!astProcessor->executing->addElementToBuild(preprocess->getBody().get(), buildSession.get())) result = false;
    }

    astProcessor->executing->finalizeBuild(preprocess->getBody().get(), buildSession.get());
  } else {
    // Statement has already been generated; We'll only force the building of the remaining deps.
    auto tmpBuildSession = astProcessor->executing->prepareBuild(BuildManager::BuildType::PREPROCESS, 0);
    astProcessor->executing->finalizeBuild(0, tmpBuildSession.get());
  }

  // If the build session was removed from the element after finalizing the build it means one of the dependencies
  // executed the statement, so we don't need to execute it again.
  if (preprocess->getExtra(PREPROCESS_SESSION_KEY) == 0) return true;

  astProcessor->currentPreprocessOwner = owner;
  astProcessor->currentPreprocessInsertionPosition = indexInOwner;
  astProcessor->currentPreprocessSourceLocation = preprocess->findSourceLocation();

  // Remove the preprocess statement.
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

  // Execute the preprocess statement.
  if (result) {
    astProcessor->executing->execute(buildSession.get());
  }

  preprocess->removeExtra(PREPROCESS_SESSION_KEY);

  astProcessor->currentPreprocessSourceLocation = 0;

  return result;
}


Bool AstProcessor::_processFunctionBody(TiObject *self, Spp::Ast::Function *func)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(func);
  LOG(
    Spp::LogLevel::PREPROCESS, S("Preprocessing function body: ") << astProcessor->astHelper->getFunctionName(func)
  );
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
    astProcessor->astHelper->getNoticeStore()->add(
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
  if (owner->isDerivedFrom<Core::Data::Ast::Scope>()) {
    auto ownerScope = static_cast<Core::Data::Ast::Scope*>(owner);
    ownerScope->remove(indexInOwner);
    Int index = indexInOwner;
    if (macroInstance->isDerivedFrom<Core::Data::Ast::Scope>()) {
      // Merge the two scopes.
      auto instanceScope = macroInstance.s_cast_get<Core::Data::Ast::Scope>();
      Core::Data::Ast::addPossiblyMergeableElements(
        instanceScope, ownerScope, index, astProcessor->astHelper->getSeeker(),
        astProcessor->astHelper->getNoticeStore()
      );
    } else {
      // Merge the element into the owner scope.
      Core::Data::Ast::addPossiblyMergeableElement(
        macroInstance.get(), ownerScope, index, astProcessor->astHelper->getSeeker(),
        astProcessor->astHelper->getNoticeStore()
      );
    }
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
  if (!astProcessor->interpolateAst(obj, argNames, args, astProcessor->currentPreprocessSourceLocation.get(), result)) {
    return false;
  }
  // Insert the interpolated AST.
  if (astProcessor->currentPreprocessOwner->isDerivedFrom<Core::Data::Ast::Scope>()) {
    auto ownerScope = static_cast<Core::Data::Ast::Scope*>(astProcessor->currentPreprocessOwner);
    if (result->isDerivedFrom<Core::Data::Ast::Scope>()) {
      // Merge the two scopes.
      auto insertedScope = result.s_cast_get<Core::Data::Ast::Scope>();
      Core::Data::Ast::addPossiblyMergeableElements(
        insertedScope, ownerScope, astProcessor->currentPreprocessInsertionPosition,
        astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
    } else {
      // Add a single element to the scope
      Core::Data::Ast::addPossiblyMergeableElement(
        result.get(), ownerScope, astProcessor->currentPreprocessInsertionPosition,
        astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
    }
  } else {
    auto ownerContainer = ti_cast<Containing<TiObject>>(astProcessor->currentPreprocessOwner);
    if (ownerContainer == 0) {
      throw EXCEPTION(GenericException, S("Unexpected AST insert location."));
    }
    auto insertedScope = result.s_cast_get<Core::Data::Ast::Scope>();
    if (insertedScope != 0 && insertedScope->getCount() == 1) {
      ownerContainer->setElement(astProcessor->currentPreprocessInsertionPosition, insertedScope->getElement(0));
    } else {
      ownerContainer->setElement(astProcessor->currentPreprocessInsertionPosition, result.get());
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
      Char const *text;
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>()) {
        text = static_cast<Core::Data::Ast::Text*>(arg)->getValue().get();
      } else if (arg->isDerivedFrom<TiStr>()) {
        text = static_cast<TiStr*>(arg)->get();
      } else {
        auto elementSl = Core::Data::Ast::findSourceLocation(arg);
        astProcessor->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(elementSl == 0 ? getSharedPtr(sl) : elementSl)
        );
        return false;
      }
      Char newVar[1000];
      astProcessor->generateStringFromTemplate(
        obj->getValue().get(), prefixSize, text, suffix, newVar, 1000
      );
      result = Core::Data::Ast::Identifier::create({
        { S("value"), TiStr(newVar) },
        { S("sourceLocation"), obj->getSourceLocation() }
      });
      Core::Data::Ast::addSourceLocation(result.get(), sl);
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      // If the argument is a basic type we'll convert it into a literal, otherwise we'll clone as is.
      if (arg->isA<TiInt>()) {
        Str s;
        s.append((LongInt)static_cast<TiInt*>(arg)->get());
        result = Core::Data::Ast::IntegerLiteral::create({ {S("value"), TiStr(s)} });
        result.s_cast<Core::Data::Ast::IntegerLiteral>()->setSourceLocation(sl);
      } else if (arg->isA<TiFloat>()) {
        Str s;
        s.append(static_cast<TiFloat*>(arg)->get());
        result = Core::Data::Ast::FloatLiteral::create({ {S("value"), TiStr(s)} });
        result.s_cast<Core::Data::Ast::FloatLiteral>()->setSourceLocation(sl);
      } else if (arg->isA<TiStr>()) {
        result = Core::Data::Ast::StringLiteral::create({ {S("value"), static_cast<TiStr*>(arg)} });
        result.s_cast<Core::Data::Ast::StringLiteral>()->setSourceLocation(sl);
      } else {
        result = Core::Data::Ast::clone(
          args->getElement(index),
          Core::Data::concatSourceLocation(obj->getSourceLocation().get(), sl).get()
        );
      }
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
      Char const *text;
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>() || arg->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
        text = static_cast<Core::Data::Ast::Text*>(arg)->getValue().get();
      } else if (arg->isDerivedFrom<TiStr>()) {
        text = static_cast<TiStr*>(arg)->get();
      } else {
        auto elementSl = Core::Data::Ast::findSourceLocation(arg);
        astProcessor->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(elementSl == 0 ? getSharedPtr(sl) : elementSl)
        );
        return false;
      }
      Char newVar[1000];
      astProcessor->generateStringFromTemplate(
        obj->getValue().get(), prefixSize, text, suffix, newVar, 1000
      );
      result = Core::Data::Ast::StringLiteral::create({
        { S("value"), TiStr(newVar) },
        { S("sourceLocation"), obj->getSourceLocation() }
      });
      Core::Data::Ast::addSourceLocation(result.get(), sl);
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
      astProcessor->astHelper->getNoticeStore()->add(
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

  if (sl != 0) Core::Data::Ast::addSourceLocation(result.get(), sl);

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
          astProcessor->astHelper->getNoticeStore()->add(
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
      Core::Data::Ast::addPossiblyMergeableElement(
        newChild.get(), destObj, astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
    } else {
      Core::Data::Ast::addPossiblyMergeableElement(
        obj->getElement(i), destObj, astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
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
          astProcessor->astHelper->getNoticeStore()->add(
            newSrdObj<Spp::Notices::InvalidMacroArgNotice>(arg->findSourceLocation())
          );
          return false;
        }
        auto value = obj->getElement(index);
        obj->insertElement(index, newKey, value);
        obj->removeElement(index + 1);
      } else {
        astProcessor->astHelper->getNoticeStore()->add(
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

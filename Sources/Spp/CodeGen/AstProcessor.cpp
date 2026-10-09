/**
 * @file Spp/CodeGen/AstProcessor.cpp
 * Contains the implementation of class Spp::CodeGen::AstProcessor.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
    &this->processModifiers,
    &this->processPreprocessStatement,
    &this->processFunctionBody,
    &this->processTypeBody,
    &this->processMacro,
    &this->applyMacroArgs,
    &this->insertInterpolatedAst,
    &this->interpolateAst,
    &this->interpolateAst_identifier,
    &this->interpolateAst_stringLiteral,
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
  this->processModifiers = &AstProcessor::_processModifiers;
  this->processPreprocessStatement = &AstProcessor::_processPreprocessStatement;
  this->processFunctionBody = &AstProcessor::_processFunctionBody;
  this->processTypeBody = &AstProcessor::_processTypeBody;
  this->processMacro = &AstProcessor::_processMacro;
  this->applyMacroArgs = &AstProcessor::_applyMacroArgs;
  this->insertInterpolatedAst = &AstProcessor::_insertInterpolatedAst;
  this->interpolateAst = &AstProcessor::_interpolateAst;
  this->interpolateAst_identifier = &AstProcessor::_interpolateAst_identifier;
  this->interpolateAst_stringLiteral = &AstProcessor::_interpolateAst_stringLiteral;
  this->interpolateAst_other = &AstProcessor::_interpolateAst_other;
  this->interpolateAst_binding = &AstProcessor::_interpolateAst_binding;
  this->interpolateAst_containing = &AstProcessor::_interpolateAst_containing;
  this->interpolateAst_dynContaining = &AstProcessor::_interpolateAst_dynContaining;
  this->interpolateAst_dynMapContaining = &AstProcessor::_interpolateAst_dynMapContaining;
}


//==============================================================================
// Main Functions

Bool AstProcessor::_process(TiObject *self, Core::Ast::Node *owner)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(owner);

  if (owner == 0) return true;

  Bool result = true;

  auto def = ti_cast<Core::Ast::Definition>(owner);
  if (def != 0 && def->isToMerge() && def->getTarget().ti_cast_get<Spp::Ast::Type>() != 0) {
    // Having a type definition with to-merge flag set means we eventually didn't find the merge target.
    astProcessor->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::MissingTypeMergeTargetNotice>(findSourceLocation(def))
    );
    // Reset the to-merge flag to avoid duplicate error messages.
    def->setToMerge(false);
    result = false;
  }

  auto container = ti_cast<Containing<Core::Ast::Node>>(owner);
  if (container == 0) return result;

  for (Int i = 0; i < container->getElementCount(); ++i) {
    auto child = container->getElement(i);
    if (child == 0) continue;

    if (!astProcessor->processModifiers(container, i)) result = false;

    // Re-get the child, in case the preprocessor replaced it with something else.
    if (i >= container->getElementCount()) break;
    child = container->getElement(i);
    if (child == 0) continue;

    if (child->isDerivedFrom<Core::Ast::ParamPass>()) {
      Bool replaced = false;
      if (!astProcessor->processParamPass(static_cast<Core::Ast::ParamPass*>(child), i, replaced)) result = false;
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
  TiObject *self, Core::Ast::ParamPass *paramPass, TiInt indexInOwner, Bool &replaced
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(paramPass);

  replaced = false;

  if (paramPass->getType() == Core::Ast::BracketType::SQUARE) {
    // Extract args.
    PlainList<Core::Ast::Node> argsList;
    auto param = paramPass->getParam().get();
    Containing<Core::Ast::Node> *args = &argsList;
    if (param) {
      if (param->isA<Core::Ast::List>()) {
        args = static_cast<Core::Ast::List*>(param);
      } else {
        argsList.add(param);
      }
    }

    Bool result = true;

    auto operand = paramPass->getOperand().get();
    if (!astProcessor->process(operand)) result = false;
    Core::Ast::Node *owner;
    Word foreachFlags = 0;
    Bool member = false;

    // Find matching macro.
    if (operand->isDerivedFrom<Core::Ast::LinkOperator>()) {
      auto linkOp = static_cast<Core::Ast::LinkOperator*>(operand);
      auto thisArg = linkOp->getFirst().get();
      if (!astProcessor->expressionComputation->computeResultType(thisArg, owner, member)) return false;
      if (owner->isDerivedFrom<Spp::Ast::Type>()) {
        owner = astProcessor->astHelper->tryGetDeepReferenceContentType(static_cast<Spp::Ast::Type*>(owner));
        if (member) argsList.insert(0, thisArg);
      }
      operand = linkOp->getSecond().get();
      foreachFlags = Core::Ast::Seeker::Flags::SKIP_OWNERS;
    } else {
      owner = paramPass->getOwner();
    }

    Ast::Macro *macro = 0;
    astProcessor->astHelper->getSeeker()->foreach(operand, owner,
      [=, &macro] (TiInt action, Core::Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &notice)->Core::Ast::Seeker::Verb
      {
        if (action != Core::Ast::Seeker::Action::TARGET_MATCH) {
          return Core::Ast::Seeker::Verb::MOVE;
        }

        auto m = ti_cast<Ast::Macro>(obj);
        if (m != 0 && m->isMember() == member && m->matchCall(args, astProcessor->astHelper)) {
          macro = m;
          return Core::Ast::Seeker::Verb::STOP;
        } else {
          return Core::Ast::Seeker::Verb::MOVE;
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


Bool AstProcessor::_processModifiers(TiObject *self, Containing<Core::Ast::Node> *container, Int indexInContainer)
{
  PREPARE_SELF(astProcessor, AstProcessor);

  Core::Ast::Node *node = container->getElement(indexInContainer);
  if (node == 0) return true;

  SharedPtr<Core::Ast::List> modifiers = node->getModifiers();
  if (modifiers == 0) return true;

  Bool result = true;
  for (Int i = 0; i < modifiers->getCount(); ++i) {
    auto modifier = modifiers->get(i).get();

    // Re-grab the node in case the previous iteration replaced it or removed it.
    if (indexInContainer >= container->getElementCount()) break;
    node = container->getElement(indexInContainer);
    if (node == 0) break;

    // Get the modifier's reference. Unlike Core::Ast::getModifierKeywordIdentifier (which only cares about
    // the plain keyword, e.g. for grammar-level translation), this keeps a LinkOperator modifier's full
    // dotted chain intact (e.g. `@mymodule.mymodifier[...]`), so the seeker below can resolve it as a full
    // qualifier and reach a matching function anywhere in the code base, not just ones directly accessible
    // from the modified node's own scope.
    auto paramPass = ti_cast<Core::Ast::ParamPass>(modifier);
    Core::Ast::Node *modifierRef = paramPass != 0 ? paramPass->getOperand().get() : modifier;
    if (
      modifierRef == 0 ||
      !(modifierRef->isDerivedFrom<Core::Ast::Identifier>() || modifierRef->isDerivedFrom<Core::Ast::LinkOperator>())
    ) {
      astProcessor->astHelper->getNoticeStore()->add(
        newSrdObj<Core::Notices::UnexpectedModifierNotice>(Core::Ast::findSourceLocation(modifier))
      );
      result = false;
      continue;
    }

    // Extract the modifier's params, if any -- whatever appeared between its `[]` brackets.
    Core::Ast::Node *modifierParams = paramPass != 0 ? paramPass->getParam().get() : 0;

    // Look for a directly accessible function matching the modifier's keyword and taking two
    // ref[Core.Ast.Node] params.
    // Core.Ast.Node won't be available if the Core library hasn't been loaded, in which case no handler
    // function could have been defined.
    auto nodeType = astProcessor->astHelper->getNodeType();
    if (nodeType == 0) {
      astProcessor->astHelper->getNoticeStore()->add(
        newSrdObj<Spp::Notices::MissingTypeNotice>(Core::Ast::findSourceLocation(modifier))
      );
      result = false;
      continue;
    }
    auto nodeRefType = astProcessor->astHelper->getReferenceTypeFor(nodeType, Ast::ReferenceMode::EXPLICIT);
    PlainList<Core::Ast::Node> argTypes;
    argTypes.add(nodeRefType);
    argTypes.add(nodeRefType);

    Ast::CalleeLookupRequest lookupRequest;
    lookupRequest.astNode = node;
    lookupRequest.target = node->findOwner<Core::Ast::Scope>();
    lookupRequest.mode = Ast::CalleeLookupMode::DIRECTLY_ACCESSIBLE;
    lookupRequest.ref = modifierRef;
    lookupRequest.op = S("()");
    lookupRequest.argTypes = &argTypes;
    Ast::CalleeLookupResult lookupResult;
    astProcessor->calleeTracer->lookupCallee(lookupRequest, lookupResult);

    if (!lookupResult.isSuccessful() || lookupResult.stack.getLength() != 1) {
      // No handler function was found for this modifier.
      astProcessor->astHelper->getNoticeStore()->add(
        newSrdObj<Core::Notices::UnexpectedModifierNotice>(Core::Ast::findSourceLocation(modifier))
      );
      result = false;
      continue;
    }
    auto func = static_cast<Ast::Function*>(lookupResult.stack(lookupResult.stack.getLength() - 1).obj);

    // Build the handler function and get a pointer to it.
    auto buildSession = astProcessor->executing->prepareBuild(BuildManager::BuildType::PREPROCESS, 0);
    if (!astProcessor->executing->addElementToBuild(func, buildSession.get())) {
      result = false;
      continue;
    }
    typedef void (*ModifierHandlerFunc)(Core::Ast::Node*, Core::Ast::Node*);
    auto funcPtr = (ModifierHandlerFunc)astProcessor->executing->prepareToExecuteFunction(func, buildSession.get());
    if (funcPtr == 0) {
      result = false;
      continue;
    }

    // Call the handler, then remove the now-handled modifier.
    funcPtr(node, modifierParams);
    modifiers->remove(i);
    --i;
  }

  return result;
}


Bool AstProcessor::_processPreprocessStatement(
  TiObject *self, Spp::Ast::PreprocessStatement *preprocess, Core::Ast::Node *owner, TiInt indexInOwner
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (!astProcessor->process(preprocess->getBody().get())) return false;

  // Capture a shared pointer to avoid segfault in case a dependency processes the same statement, or in case
  // elements from this preprocess statement are in use elsewhere, like in templates.
  astProcessor->astNodeRepo->addElement(preprocess);

  Bool result = true;

  // Build the preprocess statement.
  auto buildSession = astProcessor->executing->prepareBuild(BuildManager::BuildType::PREPROCESS, 0);
  astProcessor->executing->prepareExecutionEntry(buildSession.get());

  auto block = preprocess->getBody().ti_cast_get<Ast::Block>();
  if (block != 0) {
    for (Int i = 0; i < block->getElementCount(); ++i) {
      auto childData = block->getElement(i);
      if (!astProcessor->executing->addElementToExecutionEntry(childData, buildSession.get())) result = false;
    }
  } else {
      if (!astProcessor->executing->addElementToExecutionEntry(preprocess->getBody().get(), buildSession.get())) {
        result = false;
      }
  }

  astProcessor->executing->finalizeExecutionEntry(buildSession.get());

  Core::Ast::Node *prevPreprocessOwner = astProcessor->currentPreprocessOwner;
  Int prevPreprocessInsertionPosition = astProcessor->currentPreprocessInsertionPosition;
  SharedPtr<Core::Ast::SourceLocation> prevPreprocessSourceLocation = astProcessor->currentPreprocessSourceLocation;

  astProcessor->currentPreprocessOwner = owner;
  astProcessor->currentPreprocessInsertionPosition = indexInOwner;
  astProcessor->currentPreprocessSourceLocation = preprocess->findSourceLocation();

  // Remove the preprocess statement.
  DynamicContaining<Core::Ast::Node> *dynContainer;
  DynamicMapContaining<Core::Ast::Node> *dynMapContainer;
  Containing<Core::Ast::Node> *container;
  if ((dynContainer = ti_cast<DynamicContaining<Core::Ast::Node>>(owner)) != 0) {
    dynContainer->removeElement(indexInOwner);
  } else if ((dynMapContainer = ti_cast<DynamicMapContaining<Core::Ast::Node>>(owner)) != 0) {
    dynMapContainer->removeElement(indexInOwner);
  } else if ((container = ti_cast<Containing<Core::Ast::Node>>(owner)) != 0) {
    container->setElement(indexInOwner, 0);
  } else {
    throw EXCEPTION(InvalidArgumentException, S("owner"), S("Invalid owner type."));
  }

  // Execute the preprocess statement.
  if (result) {
    astProcessor->executing->execute(buildSession.get());
  }

  astProcessor->currentPreprocessOwner = prevPreprocessOwner;
  astProcessor->currentPreprocessInsertionPosition = prevPreprocessInsertionPosition;
  astProcessor->currentPreprocessSourceLocation = prevPreprocessSourceLocation;

  return result;
}


Bool AstProcessor::_processFunctionBody(TiObject *self, Spp::Ast::Function *func)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(func);
  auto body = func->getBody().get();
  if (body == 0) return true;
  auto state = getAstProcessingState(body);
  if (state == AstProcessingState::PROCESSED) return true;
  if (state == AstProcessingState::PROCESSING) {
    astProcessor->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::CircularFunctionCodeGenNotice>(func->findSourceLocation())
    );
    return false;
  }
  setAstProcessingState(body, AstProcessingState::PROCESSING);
  LOG(
    Spp::LogLevel::PREPROCESS, S("Preprocessing function body: ") << astProcessor->astHelper->getFunctionName(func)
  );
  Bool result = astProcessor->process(body);
  LOG(
    Spp::LogLevel::PREPROCESS, S("Preprocessing function body: ") << astProcessor->astHelper->getFunctionName(func) << " ... DONE"
  );
  setAstProcessingState(body, AstProcessingState::PROCESSED);
  return result;
}


Bool AstProcessor::_processTypeBody(TiObject *self, Spp::Ast::UserType *type)
{
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(type);
  auto body = type->getBody().get();
  if (body == 0) return true;
  auto state = getAstProcessingState(body);
  if (state == AstProcessingState::PROCESSED) return true;
  if (state == AstProcessingState::PROCESSING) {
    astProcessor->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::CircularUserTypeCodeGenNotice>(type->findSourceLocation())
    );
    return false;
  }
  setAstProcessingState(body, AstProcessingState::PROCESSING);
  LOG(
    Spp::LogLevel::PREPROCESS, S("Preprocessing type body: ") << astProcessor->astHelper->resolveNodePath(type)
  );
  Bool result = astProcessor->process(body);
  LOG(
    Spp::LogLevel::PREPROCESS, S("Preprocessing type body: ") << astProcessor->astHelper->resolveNodePath(type) << " ... DONE"
  );
  setAstProcessingState(body, AstProcessingState::PROCESSED);
  return result;
}


Bool AstProcessor::_processMacro(
  TiObject *self, Spp::Ast::Macro *macro, Containing<Core::Ast::Node> *args,
  Core::Ast::Node *owner, TiInt indexInOwner,
  Core::Ast::SourceLocation *sl
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  VALIDATE_NOT_NULL(macro, args, owner);

  // Apply macro args.
  SharedPtr<Core::Ast::Node> macroInstance;
  if (!astProcessor->applyMacroArgs(macro, args, sl, macroInstance)) return false;
  if (macroInstance == 0) {
    astProcessor->astHelper->getNoticeStore()->add(
      newSrdObj<Spp::Notices::InvalidMacroNotice>(macro->findSourceLocation())
    );
    return false;
  }
  if (macroInstance->isDerivedFrom<Core::Ast::Scope>()) {
    auto scope = macroInstance.s_cast_get<Core::Ast::Scope>();
    if (scope->getCount() == 1) {
      auto child = scope->get(0);
      macroInstance = child;
    }
  }

  // Replace macro reference with the clone.
  if (owner->isDerivedFrom<Core::Ast::Scope>()) {
    auto ownerScope = static_cast<Core::Ast::Scope*>(owner);
    ownerScope->remove(indexInOwner);
    Int index = indexInOwner;
    if (macroInstance->isDerivedFrom<Core::Ast::Scope>()) {
      // Merge the two scopes.
      auto instanceScope = macroInstance.s_cast_get<Core::Ast::Scope>();
      Core::Ast::addPossiblyMergeableElements(
        instanceScope, ownerScope, index, astProcessor->astHelper->getSeeker(),
        astProcessor->astHelper->getNoticeStore()
      );
    } else {
      // Merge the element into the owner scope.
      Core::Ast::addPossiblyMergeableElement(
        macroInstance.get(), ownerScope, index, astProcessor->astHelper->getSeeker(),
        astProcessor->astHelper->getNoticeStore()
      );
    }
  } else {
    // Replace the instance.
    auto ownerContainer = ti_cast<Containing<Core::Ast::Node>>(owner);
    if (ownerContainer == 0) {
      throw EXCEPTION(InvalidArgumentException, S("owner"), S("Invalid owner type."));
    }
    ownerContainer->setElement(indexInOwner, macroInstance.get());
  }

  return true;
}


Bool AstProcessor::_applyMacroArgs(
  TiObject *self, Spp::Ast::Macro *macro, Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
  SharedPtr<Core::Ast::Node> &result
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
  TiObject *self, Core::Ast::Node *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  SharedPtr<Core::Ast::Node> result;
  if (
    !astProcessor->interpolateAst(obj, argNames, args, astProcessor->currentPreprocessSourceLocation.get(), result)
  ) {
    return false;
  }
  // Insert the interpolated AST.
  if (astProcessor->currentPreprocessOwner->isDerivedFrom<Core::Ast::Scope>()) {
    auto ownerScope = static_cast<Core::Ast::Scope*>(astProcessor->currentPreprocessOwner);
    if (result->isDerivedFrom<Core::Ast::Scope>()) {
      // Merge the two scopes.
      auto insertedScope = result.s_cast_get<Core::Ast::Scope>();
      Core::Ast::addPossiblyMergeableElements(
        insertedScope, ownerScope, astProcessor->currentPreprocessInsertionPosition,
        astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
    } else {
      // Add a single element to the scope
      Core::Ast::addPossiblyMergeableElement(
        result.get(), ownerScope, astProcessor->currentPreprocessInsertionPosition,
        astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
    }
  } else {
    auto ownerContainer = ti_cast<Containing<Core::Ast::Node>>(astProcessor->currentPreprocessOwner);
    if (ownerContainer == 0) {
      throw EXCEPTION(GenericException, S("Unexpected AST insert location."));
    }
    auto insertedScope = result.s_cast_get<Core::Ast::Scope>();
    if (insertedScope != 0 && insertedScope->getCount() == 1) {
      ownerContainer->setElement(astProcessor->currentPreprocessInsertionPosition, insertedScope->getElement(0));
    } else {
      ownerContainer->setElement(astProcessor->currentPreprocessInsertionPosition, result.get());
    }
  }
  return true;
}


Bool AstProcessor::_interpolateAst(
  TiObject *self, Core::Ast::Node *obj, Array<Str> const *argNames,
  Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
  SharedPtr<Core::Ast::Node> &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  if (obj->isDerivedFrom<Core::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Ast::Identifier*>(obj);
    return astProcessor->interpolateAst_identifier(identifier, argNames, args, sl, result);
  }

  if (obj->isDerivedFrom<Core::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Ast::StringLiteral*>(obj);
    return astProcessor->interpolateAst_stringLiteral(stringLiteral, argNames, args, sl, result);
  }

  // It's not a replacable identifier, so we'll proceed with cloning the tree.
  return astProcessor->interpolateAst_other(obj, argNames, args, sl, result);
}


Bool AstProcessor::_interpolateAst_identifier(
  TiObject *self, Core::Ast::Identifier *obj, Array<Str> const *argNames,
  Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
  SharedPtr<Core::Ast::Node> &result
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
      if (arg->isDerivedFrom<Core::Ast::Identifier>() || arg->isDerivedFrom<Core::Ast::StringLiteral>()) {
        text = static_cast<Core::Ast::Text*>(arg)->getValue().get();
      } else {
        auto elementSl = Core::Ast::findSourceLocation(arg);
        astProcessor->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(elementSl == 0 ? getSharedPtr(sl) : elementSl)
        );
        return false;
      }
      Char newVar[1000];
      astProcessor->generateStringFromTemplate(
        obj->getValue().get(), prefixSize, text, suffix, newVar, 1000
      );
      result = Core::Ast::Identifier::create({
        { S("value"), TiStr(newVar) },
        { S("sourceLocation"), obj->getSourceLocation() }
      });
      Core::Ast::addSourceLocation(result.get(), sl);
    } else {
      // We don't have an identifier string template, so we'll just clone the arg as is.
      result = Core::Ast::clone(
        args->getElement(index),
        Core::Ast::concatSourceLocation(obj->getSourceLocation().get(), sl).get()
      );
    }
  } else {
    result = Core::Ast::clone(obj, sl);
  }
  return true;
}


Bool AstProcessor::_interpolateAst_stringLiteral(
  TiObject *self, Core::Ast::StringLiteral *obj, Array<Str> const *argNames,
  Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
  SharedPtr<Core::Ast::Node> &result
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
      if (arg->isDerivedFrom<Core::Ast::Identifier>() || arg->isDerivedFrom<Core::Ast::StringLiteral>()) {
        text = static_cast<Core::Ast::Text*>(arg)->getValue().get();
      } else {
        auto elementSl = Core::Ast::findSourceLocation(arg);
        astProcessor->astHelper->getNoticeStore()->add(
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(elementSl == 0 ? getSharedPtr(sl) : elementSl)
        );
        return false;
      }
      Char newVar[1000];
      astProcessor->generateStringFromTemplate(
        obj->getValue().get(), prefixSize, text, suffix, newVar, 1000
      );
      result = Core::Ast::StringLiteral::create({
        { S("value"), TiStr(newVar) },
        { S("sourceLocation"), obj->getSourceLocation() }
      });
      Core::Ast::addSourceLocation(result.get(), sl);
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      result = Core::Ast::clone(args->getElement(index), sl);
    }
  } else {
    result = Core::Ast::clone(obj, sl);
  }
  return true;
}


Bool AstProcessor::_interpolateAst_other(
  TiObject *self, Core::Ast::Node *obj, Array<Str> const *argNames,
  Containing<Core::Ast::Node> *args, Core::Ast::SourceLocation *sl,
  SharedPtr<Core::Ast::Node> &result
) {
  PREPARE_SELF(astProcessor, AstProcessor);

  auto factory = obj->getMyTypeInfo()->getFactory();
  if (!factory) {
    throw EXCEPTION(GenericException, S("A Node derived class is missing a type factory."));
  }
  // The object factory of a node always creates a node.
  result = factory->createShared().s_cast<Core::Ast::Node>();

  auto srcBinding = ti_cast<Binding>(obj);
  auto binding = result.ti_cast_get<Binding>();
  if (binding != 0) {
    if (!astProcessor->interpolateAst_binding(srcBinding, argNames, args, sl, binding)) return false;
  }

  auto srcDynMapContainer = ti_cast<DynamicMapContaining<Core::Ast::Node>>(obj);
  auto dynMapContainer = result.ti_cast_get<DynamicMapContaining<Core::Ast::Node>>();
  if (dynMapContainer != 0) {
    if (!astProcessor->interpolateAst_dynMapContaining(
      srcDynMapContainer, argNames, args, sl, dynMapContainer
    )) return false;
  }

  auto srcDynContainer = ti_cast<DynamicContaining<Core::Ast::Node>>(obj);
  auto dynContainer = result.ti_cast_get<DynamicContaining<Core::Ast::Node>>();
  if (dynContainer != 0) {
    if (!astProcessor->interpolateAst_dynContaining(
      srcDynContainer, argNames, args, sl, dynContainer
    )) return false;
  }

  auto srcContainer = ti_cast<Containing<Core::Ast::Node>>(obj);
  auto container = result.ti_cast_get<Containing<Core::Ast::Node>>();
  if (dynMapContainer == 0 && dynContainer == 0 && container != 0) {
    if (!astProcessor->interpolateAst_containing(
      srcContainer, argNames, args, sl, container
    )) return false;
  }

  if (sl != 0) Core::Ast::addSourceLocation(result.get(), sl);

  return true;
}


Bool AstProcessor::_interpolateAst_binding(
  TiObject *self, Binding *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
  Core::Ast::SourceLocation *sl, Binding *destObj
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
        if (arg->isDerivedFrom<Core::Ast::Identifier>() || arg->isDerivedFrom<Core::Ast::StringLiteral>()) {
          text = static_cast<Core::Ast::Text*>(arg)->getValue().get();
        } else {
          auto elementSl = Core::Ast::findSourceLocation(arg);
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
      // Interpolate the member if it's an AST node, otherwise copy it as is. Members that are not nodes are
      // properties of the object rather than parts of the AST, so they need no interpolation.
      auto child = ti_cast<Core::Ast::Node>(obj->getMember(i));
      if (child != 0) {
        SharedPtr<Core::Ast::Node> newChild;
        if (!astProcessor->interpolateAst(child, argNames, args, sl, newChild)) return false;
        destObj->setMember(i, newChild.get());
      } else {
        destObj->setMember(i, obj->getMember(i));
      }
    } else {
      destObj->setMember(i, obj->getMember(i));
    }
  }
  return true;
}


Bool AstProcessor::_interpolateAst_containing(
  TiObject *self, Containing<Core::Ast::Node> *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
  Core::Ast::SourceLocation *sl, Containing<Core::Ast::Node> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      SharedPtr<Core::Ast::Node> newChild;
      Core::Ast::Node *child = obj->getElement(i);
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
  TiObject *self, DynamicContaining<Core::Ast::Node> *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
  Core::Ast::SourceLocation *sl, DynamicContaining<Core::Ast::Node> *destObj
) {
  PREPARE_SELF(astProcessor, AstProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      SharedPtr<Core::Ast::Node> newChild;
      Core::Ast::Node *child = obj->getElement(i);
      if (child != 0) {
        if (!astProcessor->interpolateAst(child, argNames, args, sl, newChild)) return false;
      }
      Core::Ast::addPossiblyMergeableElement(
        newChild.get(), destObj, astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
    } else {
      Core::Ast::addPossiblyMergeableElement(
        obj->getElement(i), destObj, astProcessor->astHelper->getSeeker(), astProcessor->astHelper->getNoticeStore()
      );
    }
  }
  return true;
}


Bool AstProcessor::_interpolateAst_dynMapContaining(
  TiObject *self, DynamicMapContaining<Core::Ast::Node> *obj, Array<Str> const *argNames, Containing<Core::Ast::Node> *args,
  Core::Ast::SourceLocation *sl, DynamicMapContaining<Core::Ast::Node> *destObj
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
      auto arg = ti_cast<Core::Ast::Identifier>(args->getElement(index));
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
          newSrdObj<Spp::Notices::InvalidMacroArgNotice>(Core::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      copyStr(obj->getElementKey(i), newKey);
    }
    // Generate the value.
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      SharedPtr<Core::Ast::Node> newChild;
      Core::Ast::Node *child = obj->getElement(i);
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

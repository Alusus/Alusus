/**
 * @file Spp/Rt/AstMgr.cpp
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::Rt
{

//==============================================================================
// Initialization Functions

void AstMgr::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->findElements,
    &this->getModifiers,
    &this->findModifier,
    &this->findModifierForElement,
    &this->getModifierKeyword,
    &this->getModifierStringParams,
    &this->getSourceFullPathForElement,
    &this->insertAst,
    &this->insertAst_plain,
    &this->insertAst_shared,
    &this->buildAst_plain,
    &this->buildAst_shared,
    &this->getCurrentPreprocessOwner,
    &this->getCurrentPreprocessInsertionPosition,
    &this->getVariableDomain,
    &this->traceType,
    &this->computeResultType,
    &this->cloneAst,
    &this->dumpData,
    &this->getReferenceTypeFor,
    &this->tryGetDeepReferenceContentType
  });
}


void AstMgr::initBindings()
{
  this->findElements = &AstMgr::_findElements;
  this->getModifiers = &AstMgr::_getModifiers;
  this->findModifier = &AstMgr::_findModifier;
  this->findModifierForElement = &AstMgr::_findModifierForElement;
  this->getModifierKeyword = &AstMgr::_getModifierKeyword;
  this->getModifierStringParams = &AstMgr::_getModifierStringParams;
  this->getSourceFullPathForElement = &AstMgr::_getSourceFullPathForElement;
  this->insertAst = &AstMgr::_insertAst;
  this->insertAst_plain = &AstMgr::_insertAst_plain;
  this->insertAst_shared = &AstMgr::_insertAst_shared;
  this->buildAst_plain = &AstMgr::_buildAst_plain;
  this->buildAst_shared = &AstMgr::_buildAst_shared;
  this->getCurrentPreprocessOwner = &AstMgr::_getCurrentPreprocessOwner;
  this->getCurrentPreprocessInsertionPosition = &AstMgr::_getCurrentPreprocessInsertionPosition;
  this->getVariableDomain = &AstMgr::_getVariableDomain;
  this->traceType = &AstMgr::_traceType;
  this->computeResultType = &AstMgr::_computeResultType;
  this->cloneAst = &AstMgr::_cloneAst;
  this->dumpData = &AstMgr::_dumpData;
  this->getReferenceTypeFor = &AstMgr::_getReferenceTypeFor;
  this->tryGetDeepReferenceContentType = &AstMgr::_tryGetDeepReferenceContentType;
}


void AstMgr::initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, AstMgr *astMgr)
{
  globalItemRepo->addItem(S("!Spp.astMgr"), sizeof(void*), &astMgr);
  globalItemRepo->addItem(S("Spp_AstMgr_findElements"), (void*)&AstMgr::_findElements);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifiers"), (void*)&AstMgr::_getModifiers);
  globalItemRepo->addItem(S("Spp_AstMgr_findModifier"), (void*)&AstMgr::_findModifier);
  globalItemRepo->addItem(S("Spp_AstMgr_findModifierForElement"), (void*)&AstMgr::_findModifierForElement);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifierKeyword"), (void*)&AstMgr::_getModifierKeyword);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifierStringParams"), (void*)&AstMgr::_getModifierStringParams);
  globalItemRepo->addItem(S("Spp_AstMgr_getSourceFullPathForElement"), (void*)&AstMgr::_getSourceFullPathForElement);
  globalItemRepo->addItem(S("Spp_AstMgr_insertAst"), (void*)&AstMgr::_insertAst);
  globalItemRepo->addItem(S("Spp_AstMgr_insertAst_plain"), (void*)&AstMgr::_insertAst_plain);
  globalItemRepo->addItem(S("Spp_AstMgr_insertAst_shared"), (void*)&AstMgr::_insertAst_shared);
  globalItemRepo->addItem(S("Spp_AstMgr_buildAst_plain"), (void*)&AstMgr::_buildAst_plain);
  globalItemRepo->addItem(S("Spp_AstMgr_buildAst_shared"), (void*)&AstMgr::_buildAst_shared);
  globalItemRepo->addItem(S("Spp_AstMgr_getCurrentPreprocessOwner"), (void*)&AstMgr::_getCurrentPreprocessOwner);
  globalItemRepo->addItem(
    S("Spp_AstMgr_getCurrentPreprocessInsertionPosition"), (void*)&AstMgr::_getCurrentPreprocessInsertionPosition
  );
  globalItemRepo->addItem(S("Spp_AstMgr_getVariableDomain"), (void*)&AstMgr::_getVariableDomain);
  globalItemRepo->addItem(S("Spp_AstMgr_traceType"), (void*)&AstMgr::_traceType);
  globalItemRepo->addItem(S("Spp_AstMgr_computeResultType"), (void*)&AstMgr::_computeResultType);
  globalItemRepo->addItem(S("Spp_AstMgr_cloneAst"), (void*)&AstMgr::_cloneAst);
  globalItemRepo->addItem(S("Spp_AstMgr_dumpData"), (void*)&AstMgr::_dumpData);
  globalItemRepo->addItem(S("Spp_AstMgr_getReferenceTypeFor"), (void*)&AstMgr::_getReferenceTypeFor);
  globalItemRepo->addItem(
    S("Spp_AstMgr_tryGetDeepReferenceContentType"), (void*)&AstMgr::_tryGetDeepReferenceContentType
  );
}


//==============================================================================
// Operations

Array<TiObject*> AstMgr::_findElements(TiObject *self, TiObject *ref, TiObject *target, Word flags)
{
  PREPARE_SELF(astMgr, AstMgr);
  if (target == 0) target = astMgr->rootManager->getRootScope().get();
  Array<TiObject*> result;
  if (ref->isDerivedFrom<Core::Data::Ast::Scope>()) {
    auto scope = static_cast<Core::Data::Ast::Scope*>(ref);
    if (scope->getCount() != 1) {
      throw EXCEPTION(InvalidArgumentException, S("ref"), S("Should not be a block of statements."));
    }
    ref = scope->getElement(0);
  }
  astMgr->rootManager->getSeeker()->extForeach(ref, target,
    [&result](TiInt action, TiObject *obj)->Core::Data::Seeker::Verb
    {
      if (action != Core::Data::Seeker::Action::TARGET_MATCH) return Core::Data::Seeker::Verb::MOVE;
      if (obj != 0) result.add(obj);
      return Core::Data::Seeker::Verb::MOVE;
    },
    flags
  );
  return result;
}


Containing<TiObject>* AstMgr::_getModifiers(TiObject *self, TiObject *element)
{
  Array<TiObject*> result;
  auto node = ti_cast<Core::Data::Node>(element);
  auto def = Core::Data::findOwner<Core::Data::Ast::Definition>(node);
  if (def == 0 || def->getModifiers() == 0 || def->getModifiers()->getCount() == 0) {
    return 0;
  }
  return def->getModifiers().get();
}


TiObject* AstMgr::_findModifier(TiObject *self, Containing<TiObject> *modifiers, Char const *kwd)
{
  PREPARE_SELF(astMgr, AstMgr);
  for (Int i = 0; i < modifiers->getElementCount(); ++i) {
    auto modifier = modifiers->getElement(i);
    if (modifier == 0) continue;
    if (astMgr->getModifierKeyword(modifier) == kwd) return modifier;
  }
  return 0;
}


TiObject* AstMgr::_findModifierForElement(TiObject *self, TiObject *element, Char const *kwd)
{
  PREPARE_SELF(astMgr, AstMgr);
  auto modifiers = astMgr->getModifiers(element);
  if (modifiers == 0) return 0;
  return astMgr->findModifier(modifiers, kwd);
}


String AstMgr::_getModifierKeyword(TiObject *self, TiObject *modifier)
{
  Core::Data::Ast::Identifier *identifier = 0;
  if (modifier->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    identifier = static_cast<Core::Data::Ast::Identifier*>(modifier);
  } else if (modifier->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(modifier);
    identifier = paramPass->getOperand().ti_cast_get<Core::Data::Ast::Identifier>();
  }
  if (identifier != 0) return identifier->getValue().getStr();
  else return String();
}


Bool AstMgr::_getModifierStringParams(TiObject *self, TiObject *modifier, Array<String> &result)
{
  PREPARE_SELF(astMgr, AstMgr);

  auto paramPass = ti_cast<Core::Data::Ast::ParamPass>(modifier);
  if (paramPass == 0) return true;

  Core::Basic::PlainList<TiObject> strList;
  auto strs = paramPass->getParam().ti_cast_get<Core::Basic::Containing<TiObject>>();
  if (strs == 0) {
    strList.add(paramPass->getParam().get());
    strs = &strList;
  }
  for (Int i = 0; i < strs->getElementCount(); ++i) {
    auto str = ti_cast<Core::Data::Ast::StringLiteral>(strs->getElement(i));
    if (str == 0) {
      astMgr->rootManager->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidModifierDataNotice>(
        Core::Data::Ast::findSourceLocation(strs->getElement(i))
      ));
      astMgr->rootManager->flushNotices();
      return false;
    }
    result.add(str->getValue().getStr());
  }
  return true;
}


String AstMgr::_getSourceFullPathForElement(TiObject *self, TiObject *element)
{
  auto sourceLocation = Core::Data::Ast::findSourceLocation(element).get();
  if (sourceLocation->isDerivedFrom<Core::Data::SourceLocationRecord>()) {
    return static_cast<Core::Data::SourceLocationRecord*>(sourceLocation)->filename;
  } else {
    auto stack = static_cast<Core::Data::SourceLocationStack*>(sourceLocation);
    sourceLocation = stack->get(0).get();
    return static_cast<Core::Data::SourceLocationRecord*>(sourceLocation)->filename;
  }
}


Bool AstMgr::_insertAst(TiObject *self, TiObject* ast)
{
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names;
  Array<TiObject*> values;
  PlainArrayWrapperContainer<TiObject> container(&values);
  Bool result = astMgr->astProcessor->insertInterpolatedAst(ast, &names, &container);
  astMgr->rootManager->flushNotices();
  return result;
}


Bool AstMgr::_insertAst_plain(TiObject *self, TiObject* ast, Map<Str, TiObject*> *interpolations)
{
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<TiObject*> values = interpolations->getValues();
  PlainArrayWrapperContainer<TiObject> container(&values);
  Bool result = astMgr->astProcessor->insertInterpolatedAst(ast, &names, &container);
  astMgr->rootManager->flushNotices();
  return result;
}


Bool AstMgr::_insertAst_shared(TiObject *self, TiObject* ast, Map<Str, SharedPtr<TiObject>> *interpolations)
{
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<SharedPtr<TiObject>> values = interpolations->getValues();
  SharedArrayWrapperContainer<TiObject> container(&values);
  Bool result = astMgr->astProcessor->insertInterpolatedAst(ast, &names, &container);
  astMgr->rootManager->flushNotices();
  return result;
}


Bool AstMgr::_buildAst_plain(TiObject *self, TiObject *ast, Map<Str, TiObject*> *interpolations, TioSharedPtr &result)
{
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<TiObject*> values = interpolations->getValues();
  PlainArrayWrapperContainer<TiObject> container(&values);
  Bool ret = astMgr->astProcessor->interpolateAst(
    ast, &names, &container, Core::Data::Ast::findSourceLocation(ast).get(), result
  );
  astMgr->rootManager->flushNotices();
  return ret;
}


Bool AstMgr::_buildAst_shared(
  TiObject *self, TiObject *ast, Map<Str, SharedPtr<TiObject>> *interpolations, TioSharedPtr &result
) {
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<SharedPtr<TiObject>> values = interpolations->getValues();
  SharedArrayWrapperContainer<TiObject> container(&values);
  Bool ret = astMgr->astProcessor->interpolateAst(
    ast, &names, &container, Core::Data::Ast::findSourceLocation(ast).get(), result
  );
  astMgr->rootManager->flushNotices();
  return ret;
}


TiObject* AstMgr::_getCurrentPreprocessOwner(TiObject *self)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astProcessor->getCurrentPreprocessOwner();
}


Int AstMgr::_getCurrentPreprocessInsertionPosition(TiObject *self)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astProcessor->getCurrentPreprocessInsertionPosition();
}


Int AstMgr::_getVariableDomain(TiObject *self, TiObject *ast)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->getVariableDomain(ast);
}


Spp::Ast::Type* AstMgr::_traceType(TiObject *self, TiObject *astNode)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->traceType(astNode);
}


Bool AstMgr::_computeResultType(TiObject *self, TiObject *astNode, TiObject *&result, Bool &resultIsValue)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->expressionComputation->computeResultType(astNode, result, resultIsValue);
}


SharedPtr<TiObject> AstMgr::_cloneAst(TiObject *self, TiObject *astNodeToCopy, TiObject *astNodeForSourceLocation)
{
  PREPARE_SELF(astMgr, AstMgr);
  auto sourceLocation = Core::Data::Ast::findSourceLocation(astNodeForSourceLocation).get();
  return Core::Data::Ast::clone(astNodeToCopy, sourceLocation);
}


void AstMgr::_dumpData(TiObject *self, TiObject *obj)
{
  Core::Data::dumpData(outStream, obj, 0);
}


Spp::Ast::ReferenceType* AstMgr::_getReferenceTypeFor(TiObject *self, TiObject *type)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->getReferenceTypeFor(type, Spp::Ast::ReferenceMode::EXPLICIT);
}


Spp::Ast::Type* AstMgr::_tryGetDeepReferenceContentType(TiObject *self, Spp::Ast::Type *type)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->tryGetDeepReferenceContentType(type);
}

} // namespace

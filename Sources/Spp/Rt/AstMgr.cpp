/**
 * @file Spp/Rt/AstMgr.cpp
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
    &this->getModifierParams,
    &this->getModifierStringParams,
    &this->getStringsFromStringParams,
    &this->getSourceFullPathForElement,
    &this->addPossiblyMergeableElement,
    &this->insertAst,
    &this->insertAst_plain,
    &this->insertAst_shared,
    &this->buildAst_plain,
    &this->buildAst_shared,
    &this->getCurrentPreprocessOwner,
    &this->preprocessTypeBody,
    &this->getCurrentPreprocessInsertionPosition,
    &this->getVariableDomain,
    &this->traceType,
    &this->isCastableTo,
    &this->matchTemplateInstance,
    &this->computeResultType,
    &this->cloneAst,
    &this->dumpAst,
    &this->getReferenceTypeFor,
    &this->tryGetDeepReferenceContentType,
    &this->isInjection
  });
}


void AstMgr::initBindings()
{
  this->findElements = &AstMgr::_findElements;
  this->getModifiers = &AstMgr::_getModifiers;
  this->findModifier = &AstMgr::_findModifier;
  this->findModifierForElement = &AstMgr::_findModifierForElement;
  this->getModifierKeyword = &AstMgr::_getModifierKeyword;
  this->getModifierParams = &AstMgr::_getModifierParams;
  this->getModifierStringParams = &AstMgr::_getModifierStringParams;
  this->getStringsFromStringParams = &AstMgr::_getStringsFromStringParams;
  this->getSourceFullPathForElement = &AstMgr::_getSourceFullPathForElement;
  this->addPossiblyMergeableElement = &AstMgr::_addPossiblyMergeableElement;
  this->insertAst = &AstMgr::_insertAst;
  this->insertAst_plain = &AstMgr::_insertAst_plain;
  this->insertAst_shared = &AstMgr::_insertAst_shared;
  this->buildAst_plain = &AstMgr::_buildAst_plain;
  this->buildAst_shared = &AstMgr::_buildAst_shared;
  this->getCurrentPreprocessOwner = &AstMgr::_getCurrentPreprocessOwner;
  this->preprocessTypeBody = &AstMgr::_preprocessTypeBody;
  this->getCurrentPreprocessInsertionPosition = &AstMgr::_getCurrentPreprocessInsertionPosition;
  this->getVariableDomain = &AstMgr::_getVariableDomain;
  this->traceType = &AstMgr::_traceType;
  this->isCastableTo = &AstMgr::_isCastableTo;
  this->matchTemplateInstance = &AstMgr::_matchTemplateInstance;
  this->computeResultType = &AstMgr::_computeResultType;
  this->cloneAst = &AstMgr::_cloneAst;
  this->dumpAst = &AstMgr::_dumpAst;
  this->getReferenceTypeFor = &AstMgr::_getReferenceTypeFor;
  this->tryGetDeepReferenceContentType = &AstMgr::_tryGetDeepReferenceContentType;
  this->isInjection = &AstMgr::_isInjection;
}


void AstMgr::initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, AstMgr *astMgr)
{
  globalItemRepo->addItem(S("!Spp.astMgr"), sizeof(void*), &astMgr);
  globalItemRepo->addItem(S("Spp_AstMgr_findElements"), (void*)&AstMgr::_findElements);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifiers"), (void*)&AstMgr::_getModifiers);
  globalItemRepo->addItem(S("Spp_AstMgr_findModifier"), (void*)&AstMgr::_findModifier);
  globalItemRepo->addItem(S("Spp_AstMgr_findModifierForElement"), (void*)&AstMgr::_findModifierForElement);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifierKeyword"), (void*)&AstMgr::_getModifierKeyword);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifierParams"), (void*)&AstMgr::_getModifierParams);
  globalItemRepo->addItem(S("Spp_AstMgr_getModifierStringParams"), (void*)&AstMgr::_getModifierStringParams);
  globalItemRepo->addItem(S("Spp_AstMgr_getStringsFromStringParams"), (void*)&AstMgr::_getStringsFromStringParams);
  globalItemRepo->addItem(S("Spp_AstMgr_getSourceFullPathForElement"), (void*)&AstMgr::_getSourceFullPathForElement);
  globalItemRepo->addItem(
    S("Spp_AstMgr_addPossiblyMergeableElement"), (void*)&AstMgr::_addPossiblyMergeableElement
  );
  globalItemRepo->addItem(S("Spp_AstMgr_insertAst"), (void*)&AstMgr::_insertAst);
  globalItemRepo->addItem(S("Spp_AstMgr_insertAst_plain"), (void*)&AstMgr::_insertAst_plain);
  globalItemRepo->addItem(S("Spp_AstMgr_insertAst_shared"), (void*)&AstMgr::_insertAst_shared);
  globalItemRepo->addItem(S("Spp_AstMgr_buildAst_plain"), (void*)&AstMgr::_buildAst_plain);
  globalItemRepo->addItem(S("Spp_AstMgr_buildAst_shared"), (void*)&AstMgr::_buildAst_shared);
  globalItemRepo->addItem(S("Spp_AstMgr_getCurrentPreprocessOwner"), (void*)&AstMgr::_getCurrentPreprocessOwner);
  globalItemRepo->addItem(S("Spp_AstMgr_preprocessTypeBody"), (void*)&AstMgr::_preprocessTypeBody);
  globalItemRepo->addItem(
    S("Spp_AstMgr_getCurrentPreprocessInsertionPosition"), (void*)&AstMgr::_getCurrentPreprocessInsertionPosition
  );
  globalItemRepo->addItem(S("Spp_AstMgr_getVariableDomain"), (void*)&AstMgr::_getVariableDomain);
  globalItemRepo->addItem(S("Spp_AstMgr_traceType"), (void*)&AstMgr::_traceType);
  globalItemRepo->addItem(S("Spp_AstMgr_isCastableTo"), (void*)&AstMgr::_isCastableTo);
  globalItemRepo->addItem(S("Spp_AstMgr_matchTemplateInstance"), (void*)&AstMgr::_matchTemplateInstance);
  globalItemRepo->addItem(S("Spp_AstMgr_computeResultType"), (void*)&AstMgr::_computeResultType);
  globalItemRepo->addItem(S("Spp_AstMgr_cloneAst"), (void*)&AstMgr::_cloneAst);
  globalItemRepo->addItem(S("Spp_AstMgr_dumpAst"), (void*)&AstMgr::_dumpAst);
  globalItemRepo->addItem(S("Spp_AstMgr_getReferenceTypeFor"), (void*)&AstMgr::_getReferenceTypeFor);
  globalItemRepo->addItem(
    S("Spp_AstMgr_tryGetDeepReferenceContentType"), (void*)&AstMgr::_tryGetDeepReferenceContentType
  );
  globalItemRepo->addItem(S("Spp_AstMgr_isInjection"), (void*)&AstMgr::_isInjection);
}


//==============================================================================
// Operations

Array<Core::Ast::Node*> AstMgr::_findElements(
  TiObject *self, Core::Ast::Node *ref, Core::Ast::Node *target, Word flags
) {
  PREPARE_SELF(astMgr, AstMgr);
  if (target == 0) target = astMgr->rootManager->getRootScope().get();
  Array<Core::Ast::Node*> result;
  if (ref->isDerivedFrom<Core::Ast::Scope>()) {
    auto scope = static_cast<Core::Ast::Scope*>(ref);
    if (scope->getCount() != 1) {
      throw EXCEPTION(InvalidArgumentException, S("ref"), S("Should not be a block of statements."));
    }
    ref = scope->getElement(0);
  }
  astMgr->rootManager->getSeeker()->extForeach(ref, target,
    [&result](TiInt action, Core::Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &notice)->Core::Ast::Seeker::Verb
    {
      if (action != Core::Ast::Seeker::Action::TARGET_MATCH) return Core::Ast::Seeker::Verb::MOVE;
      if (obj != 0) result.add(obj);
      return Core::Ast::Seeker::Verb::MOVE;
    },
    flags
  );
  return result;
}


Containing<Core::Ast::Node>* AstMgr::_getModifiers(TiObject *self, Core::Ast::Node *element)
{
  Array<TiObject*> result;
  auto def = Core::Ast::findOwner<Core::Ast::Definition>(element);
  if (def == 0 || def->getModifiers() == 0 || def->getModifiers()->getCount() == 0) {
    return 0;
  }
  return def->getModifiers().get();
}


Core::Ast::Node* AstMgr::_findModifier(TiObject *self, Containing<Core::Ast::Node> *modifiers, Char const *kwd)
{
  PREPARE_SELF(astMgr, AstMgr);
  for (Int i = 0; i < modifiers->getElementCount(); ++i) {
    auto modifier = modifiers->getElement(i);
    if (modifier == 0) continue;
    if (astMgr->getModifierKeyword(modifier) == kwd) return modifier;
  }
  return 0;
}


Core::Ast::Node* AstMgr::_findModifierForElement(TiObject *self, Core::Ast::Node *element, Char const *kwd)
{
  PREPARE_SELF(astMgr, AstMgr);
  auto modifiers = astMgr->getModifiers(element);
  if (modifiers == 0) return 0;
  return astMgr->findModifier(modifiers, kwd);
}


String AstMgr::_getModifierKeyword(TiObject *self, Core::Ast::Node *modifier)
{
  Core::Ast::Identifier *identifier = 0;
  if (modifier->isDerivedFrom<Core::Ast::Identifier>()) {
    identifier = static_cast<Core::Ast::Identifier*>(modifier);
  } else if (modifier->isDerivedFrom<Core::Ast::ParamPass>()) {
    auto paramPass = static_cast<Core::Ast::ParamPass*>(modifier);
    identifier = paramPass->getOperand().ti_cast_get<Core::Ast::Identifier>();
  }
  if (identifier != 0) return identifier->getValue().getStr();
  else return String();
}


Bool AstMgr::_getModifierParams(TiObject *self, Core::Ast::Node *modifier, Array<Core::Ast::Node*> &result)
{
  PREPARE_SELF(astMgr, AstMgr);

  auto paramPass = ti_cast<Core::Ast::ParamPass>(modifier);
  if (paramPass == 0) return true;

  auto params = paramPass->getParam().ti_cast_get<Core::Basic::Containing<Core::Ast::Node>>();
  if (params == 0) {
    result.add(paramPass->getParam().get());
  } else {
    for (Int i = 0; i < params->getElementCount(); ++i) {
      result.add(params->getElement(i));
    }
  }
  return true;
}


Bool AstMgr::_getModifierStringParams(TiObject *self, Core::Ast::Node *modifier, Array<String> &result)
{
  PREPARE_SELF(astMgr, AstMgr);

  auto paramPass = ti_cast<Core::Ast::ParamPass>(modifier);
  if (paramPass == 0) return true;
  return astMgr->getStringsFromStringParams(paramPass->getParam().get(), result);
}


Bool AstMgr::_getStringsFromStringParams(TiObject *self, Core::Ast::Node *params, Array<String> &result)
{
  PREPARE_SELF(astMgr, AstMgr);

  if (params == 0) return true;

  Core::Basic::PlainList<Core::Ast::Node> strList;
  auto strs = ti_cast<Core::Basic::Containing<Core::Ast::Node>>(params);
  if (strs == 0) {
    strList.add(params);
    strs = &strList;
  }
  for (Int i = 0; i < strs->getElementCount(); ++i) {
    auto str = ti_cast<Core::Ast::StringLiteral>(strs->getElement(i));
    if (str == 0) {
      astMgr->rootManager->getNoticeStore()->add(newSrdObj<Spp::Notices::InvalidModifierDataNotice>(
        Core::Ast::findSourceLocation(strs->getElement(i))
      ));
      astMgr->rootManager->flushNotices();
      return false;
    }
    result.add(str->getValue().getStr());
  }
  return true;
}


String AstMgr::_getSourceFullPathForElement(TiObject *self, Core::Ast::Node *element)
{
  auto sourceLocation = Core::Ast::findSourceLocation(element).get();
  if (sourceLocation->isDerivedFrom<Core::Ast::SourceLocationRecord>()) {
    return static_cast<Core::Ast::SourceLocationRecord*>(sourceLocation)->filename;
  } else {
    auto stack = static_cast<Core::Ast::SourceLocationStack*>(sourceLocation);
    sourceLocation = stack->get(0).get();
    return static_cast<Core::Ast::SourceLocationRecord*>(sourceLocation)->filename;
  }
}


Bool AstMgr::_addPossiblyMergeableElement(
  TiObject *self, Core::Ast::Node *src, DynamicContaining<Core::Ast::Node> *target, Int &index
) {
  PREPARE_SELF(astMgr, AstMgr);
  Bool result = Core::Ast::addPossiblyMergeableElement(
    src, target, index, astMgr->astHelper->getSeeker(), astMgr->astHelper->getNoticeStore()
  );
  astMgr->rootManager->flushNotices();
  return result;
}


Bool AstMgr::_insertAst(TiObject *self, Core::Ast::Node* ast)
{
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names;
  Array<Core::Ast::Node*> values;
  PlainArrayWrapperContainer<Core::Ast::Node> container(&values);
  Bool result = astMgr->astProcessor->insertInterpolatedAst(ast, &names, &container);
  astMgr->rootManager->flushNotices();
  return result;
}


Bool AstMgr::_insertAst_plain(
  TiObject *self, Core::Ast::Node *ast, Map<Str, Core::Ast::Node*> *interpolations
) {
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<Core::Ast::Node*> values = interpolations->getValues();
  PlainArrayWrapperContainer<Core::Ast::Node> container(&values);
  Bool result = astMgr->astProcessor->insertInterpolatedAst(ast, &names, &container);
  astMgr->rootManager->flushNotices();
  return result;
}


Bool AstMgr::_insertAst_shared(
  TiObject *self, Core::Ast::Node *ast, Map<Str, SharedPtr<Core::Ast::Node>> *interpolations
) {
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<SharedPtr<Core::Ast::Node>> values = interpolations->getValues();
  SharedArrayWrapperContainer<Core::Ast::Node> container(&values);
  Bool result = astMgr->astProcessor->insertInterpolatedAst(ast, &names, &container);
  astMgr->rootManager->flushNotices();
  return result;
}


Bool AstMgr::_buildAst_plain(
  TiObject *self, Core::Ast::Node *ast, Map<Str, Core::Ast::Node*> *interpolations,
  SharedPtr<Core::Ast::Node> &result
) {
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<Core::Ast::Node*> values = interpolations->getValues();
  PlainArrayWrapperContainer<Core::Ast::Node> container(&values);
  Bool ret = astMgr->astProcessor->interpolateAst(
    ast, &names, &container, Core::Ast::findSourceLocation(ast).get(), result
  );
  astMgr->rootManager->flushNotices();
  return ret;
}


Bool AstMgr::_buildAst_shared(
  TiObject *self, Core::Ast::Node *ast, Map<Str, SharedPtr<Core::Ast::Node>> *interpolations,
  SharedPtr<Core::Ast::Node> &result
) {
  PREPARE_SELF(astMgr, AstMgr);
  Array<Str> names = interpolations->getKeys();
  Array<SharedPtr<Core::Ast::Node>> values = interpolations->getValues();
  SharedArrayWrapperContainer<Core::Ast::Node> container(&values);
  Bool ret = astMgr->astProcessor->interpolateAst(
    ast, &names, &container, Core::Ast::findSourceLocation(ast).get(), result
  );
  astMgr->rootManager->flushNotices();
  return ret;
}


Core::Ast::Node* AstMgr::_getCurrentPreprocessOwner(TiObject *self)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astProcessor->getCurrentPreprocessOwner();
}


Bool AstMgr::_preprocessTypeBody(TiObject *self, Spp::Ast::UserType *type)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astProcessor->processTypeBody(type);
}


Int AstMgr::_getCurrentPreprocessInsertionPosition(TiObject *self)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astProcessor->getCurrentPreprocessInsertionPosition();
}


Int AstMgr::_getVariableDomain(TiObject *self, Core::Ast::Node *ast)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->getVariableDomain(ast);
}


Spp::Ast::Type* AstMgr::_traceType(TiObject *self, Core::Ast::Node *astNode)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->traceType(astNode);
}


Bool AstMgr::_isCastableTo(
  TiObject *self, Core::Ast::Node *srcTypeRef, Core::Ast::Node *targetTypeRef, Bool implicit
) {
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->isCastableTo(srcTypeRef, targetTypeRef, implicit);
}


Bool AstMgr::_matchTemplateInstance(
  TiObject *self, Spp::Ast::Template *tmplt, Core::Ast::Node *templateInputs,
  SharedPtr<Core::Ast::Node> &result
) {
  PREPARE_SELF(astMgr, AstMgr);
  SharedPtr<Core::Notices::Notice> notice;
  if (tmplt->matchInstance(templateInputs, astMgr->astHelper, result, notice)) return true;
  if (notice != 0) astMgr->rootManager->getNoticeStore()->add(notice);
  return false;
}

Bool AstMgr::_computeResultType(
  TiObject *self, Core::Ast::Node *astNode, Core::Ast::Node *&result, Bool &resultIsValue
) {
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->expressionComputation->computeResultType(astNode, result, resultIsValue);
}


SharedPtr<Core::Ast::Node> AstMgr::_cloneAst(
  TiObject *self, Core::Ast::Node *astNodeToCopy, Core::Ast::Node *astNodeForSourceLocation
) {
  PREPARE_SELF(astMgr, AstMgr);
  auto sourceLocation = Core::Ast::findSourceLocation(astNodeForSourceLocation).get();
  return Core::Ast::clone(astNodeToCopy, sourceLocation);
}


void AstMgr::_dumpAst(TiObject *self, Core::Ast::Node *obj)
{
  Core::Ast::dumpAst(outStream, obj, 0);
}


Spp::Ast::ReferenceType* AstMgr::_getReferenceTypeFor(TiObject *self, Core::Ast::Node *type)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->getReferenceTypeFor(type, Spp::Ast::ReferenceMode::EXPLICIT);
}


Spp::Ast::Type* AstMgr::_tryGetDeepReferenceContentType(TiObject *self, Spp::Ast::Type *type)
{
  PREPARE_SELF(astMgr, AstMgr);
  return astMgr->astHelper->tryGetDeepReferenceContentType(type);
}


Bool AstMgr::_isInjection(TiObject *self, Core::Ast::Node *obj)
{
  auto def = ti_cast<Core::Ast::Definition>(obj);
  if (def == 0) {
    def = ti_cast<Core::Ast::Definition>(obj->getOwner());
    if (def == 0) return false;
  }
  return Ast::isInjection(def);
}

} // namespace

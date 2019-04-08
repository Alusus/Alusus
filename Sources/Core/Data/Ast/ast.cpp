/**
 * @file Core/Data/Ast/ast.cpp
 * Contains the global implementations of Data::Ast namespace's declarations.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Ast
{

//============================================================================
// Global Functions

SharedPtr<SourceLocation> const& findSourceLocation(TiObject *obj)
{
  auto metadata = ti_cast<MetaHaving>(obj);
  if (metadata != 0) return metadata->findSourceLocation();
  else return SharedPtr<SourceLocation>::null;
}


Bool mergeDefinition(Definition *def, DynamicContaining<TiObject> *target, Notices::Store *noticeStore)
{
  VALIDATE_NOT_NULL(def, target, noticeStore);
  for (Int i = 0; i < target->getElementCount(); ++i) {
    auto targetDef = ti_cast<Definition>(target->getElement(i));
    if (targetDef != 0 && targetDef->getName() == def->getName()) {
      auto targetObj = targetDef->getTarget().ti_cast<Mergeable>();
      if (targetObj == 0) {
        noticeStore->add(
          std::make_shared<Core::Notices::IncompatibleDefMergeNotice>(findSourceLocation(def))
        );
        return false;
      }
      return targetObj->merge(def->getTarget().get(), noticeStore);
    }
  }
  target->addElement(def);
  return true;
}


Bool addPossiblyMergeableElement(TiObject *src, DynamicContaining<TiObject> *target, Notices::Store *noticeStore)
{
  VALIDATE_NOT_NULL(src, target, noticeStore);
  if (src->isDerivedFrom<Definition>()) {
    auto def = static_cast<Definition*>(src);
    if (def->isToMerge()) {
      return mergeDefinition(def, target, noticeStore);
    } else {
      target->addElement(src);
    }
  } else if(src->isDerivedFrom<List>()){
    target->addElement(static_cast<List*>(src)->getElement(0));
    target->addElement(static_cast<List*>(src)->getElement(1));
  } else {
    target->addElement(src);
  }
  return true;
}


Bool addPossiblyMergeableElements(Containing<TiObject> *src, DynamicContaining<TiObject> *target, Notices::Store *noticeStore)
{
  VALIDATE_NOT_NULL(src, target, noticeStore);
  Bool result = true;
  for (Int i = 0; i < src->getElementCount(); ++i) {
    if (!addPossiblyMergeableElement(src->getElement(i), target, noticeStore)) result = false;
  }
  return result;
}


void translateModifier(Data::Grammar::SymbolDefinition *symbolDef, TiObject *modifier)
{
  if (modifier->isDerivedFrom<Data::Ast::Identifier>()) {
    auto identifier = static_cast<Data::Ast::Identifier*>(modifier);
    identifier->setValue(symbolDef->getTranslatedModifierKeyword(identifier->getValue().get()).c_str());
  } else if (modifier->isDerivedFrom<Data::Ast::LinkOperator>()) {
    auto link = static_cast<Data::Ast::LinkOperator*>(modifier);
    translateModifier(symbolDef, link->getFirst().get());
  } else if (modifier->isDerivedFrom<Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Data::Ast::ParamPass*>(modifier);
    translateModifier(symbolDef, paramPass->getOperand().get());
  }
}

} // namespace

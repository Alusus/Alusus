/**
 * @file Core/Data/Ast/ast.cpp
 * Contains the global implementations of Data::Ast namespace's declarations.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Ast
{

//============================================================================
// Global Functions

SharedPtr<SourceLocation> const& findSourceLocation(TiObject const *obj)
{
  auto metadata = ti_cast<MetaHaving const>(obj);
  if (metadata != 0) return metadata->findSourceLocation();
  else return SharedPtr<SourceLocation>::null;
}


void addSourceLocation(TiObject *obj, SourceLocation *sl)
{
  auto metadata = ti_cast<MetaHaving>(obj);
  if (metadata == 0) return;

  auto currentSl = metadata->findSourceLocation();
  if (currentSl == 0) {
    metadata->setSourceLocation(sl);
  } else {
    auto newSl = newSrdObj<SourceLocationStack>();
    newSl->push(sl);
    newSl->push(currentSl.get());
    metadata->setSourceLocation(newSl);
  }
}


Bool mergeDefinition(
  Definition *def, DynamicContaining<TiObject> *target, Data::Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(def, target, noticeStore);
  Core::Data::Ast::Identifier ref({{ S("value"), def->getName() }});
  Bool result = true;
  Bool found = false;
  seeker->foreach(&ref, target->getTiObject(),
    [=,&result,&found](TiObject *obj, Notices::Notice *notice)->Seeker::Verb {
      found = true;
      auto targetObj = ti_cast<Mergeable>(obj);
      if (targetObj == 0) {
        noticeStore->add(
          newSrdObj<Core::Notices::IncompatibleDefMergeNotice>(findSourceLocation(def))
        );
        result = false;
        return Seeker::Verb::STOP;
      }
      auto node = ti_cast<Node>(obj);
      auto targetDef = node != 0 ? ti_cast<Definition>(node->getOwner()) : 0;
      if (targetDef != 0) {
        // Merge the definition modifiers.
        if (def->getModifiers() != 0) {
          if (targetDef->getModifiers() == 0) {
            targetDef->setModifiers(def->getModifiers());
          } else {
            for (Int i = 0; i < def->getModifiers()->getCount(); ++i) {
              targetDef->getModifiers()->add(def->getModifiers()->get(i));
            }
          }
        }
      }
      // Merge the target itself.
      result = targetObj->merge(def->getTarget().get(), seeker, noticeStore);
      return Seeker::Verb::STOP;
    },
    Seeker::Flags::SKIP_OWNERS | Seeker::Flags::SKIP_OWNED | Seeker::Flags::SKIP_USES
  );
  if (!found) {
    target->addElement(def);
  }
  return result;
}


Bool addPossiblyMergeableElement(
  TiObject *src, DynamicContaining<TiObject> *target, Data::Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(src, target, noticeStore);
  if (src->isDerivedFrom<Definition>()) {
    auto def = static_cast<Definition*>(src);
    if (def->isToMerge()) {
      return mergeDefinition(def, target, seeker, noticeStore);
    } else {
      target->addElement(src);
    }
  } else if(src->isDerivedFrom<MergeList>()){
    auto mergeList = static_cast<MergeList*>(src);
    for (Int i = 0; i < mergeList->getElementCount(); ++i) {
      target->addElement(mergeList->getElement(i));
    }
  } else {
    target->addElement(src);
  }
  return true;
}


Bool addPossiblyMergeableElements(
  Containing<TiObject> *src, DynamicContaining<TiObject> *target, Data::Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(src, target, noticeStore);
  Bool result = true;
  for (Int i = 0; i < src->getElementCount(); ++i) {
    if (!addPossiblyMergeableElement(src->getElement(i), target, seeker, noticeStore)) result = false;
  }
  return result;
}


void translateModifier(Data::Grammar::SymbolDefinition *symbolDef, TiObject *modifier)
{
  if (modifier->isDerivedFrom<Data::Ast::Identifier>()) {
    auto identifier = static_cast<Data::Ast::Identifier*>(modifier);
    identifier->setValue(symbolDef->getTranslatedModifierKeyword(identifier->getValue().get()));
  } else if (modifier->isDerivedFrom<Data::Ast::LinkOperator>()) {
    auto link = static_cast<Data::Ast::LinkOperator*>(modifier);
    translateModifier(symbolDef, link->getFirst().get());
  } else if (modifier->isDerivedFrom<Data::Ast::ParamPass>()) {
    auto paramPass = static_cast<Data::Ast::ParamPass*>(modifier);
    translateModifier(symbolDef, paramPass->getOperand().get());
  }
}


TioSharedPtr _clone(TioSharedPtr const &obj, SourceLocation *sl)
{
  if (obj.ti_cast_get<Node>() == 0) return obj;

  auto factory = obj->getMyTypeInfo()->getFactory();
  if (!factory) {
    throw EXCEPTION(GenericException, S("A Node derived class is missing a type factory."));
  }
  auto clone = factory->createShared();

  auto bindings = obj.ti_cast_get<Binding>();
  auto cloneBindings = clone.ti_cast_get<Binding>();
  if (cloneBindings != 0) {
    for (Int i = 0; i < bindings->getMemberCount(); ++i) {
      if (bindings->getMemberHoldMode(i) == HoldMode::SHARED_REF) {
        auto childMember = getSharedPtr(bindings->getMember(i));
        cloneBindings->setMember(i, _clone(childMember, sl).get());
      } else {
        cloneBindings->setMember(i, bindings->getMember(i));
      }
    }
  }

  auto dynMapContainer = obj.ti_cast_get<DynamicMapContaining<TiObject>>();
  auto cloneDynMapContainer = clone.ti_cast_get<DynamicMapContaining<TiObject>>();
  if (cloneDynMapContainer != 0) {
    for (Int i = 0; i < dynMapContainer->getElementCount(); ++i) {
      if (dynMapContainer->getElementHoldMode(i) == HoldMode::SHARED_REF) {
        auto childElement = getSharedPtr(dynMapContainer->getElement(i));
        cloneDynMapContainer->addElement(dynMapContainer->getElementKey(i), _clone(childElement, sl).get());
      } else {
        cloneDynMapContainer->addElement(dynMapContainer->getElementKey(i), dynMapContainer->getElement(i));
      }
    }
  }

  auto dynContainer = obj.ti_cast_get<DynamicContaining<TiObject>>();
  auto cloneDynContainer = clone.ti_cast_get<DynamicContaining<TiObject>>();
  if (cloneDynContainer != 0) {
    for (Int i = 0; i < dynContainer->getElementCount(); ++i) {
      if (dynContainer->getElementHoldMode(i) == HoldMode::SHARED_REF) {
        auto childElement = getSharedPtr(dynContainer->getElement(i));
        cloneDynContainer->addElement(_clone(childElement, sl).get());
      } else {
        cloneDynContainer->addElement(dynContainer->getElement(i));
      }
    }
  }

  auto container = obj.ti_cast_get<Containing<TiObject>>();
  auto cloneContainer = clone.ti_cast_get<Containing<TiObject>>();
  if (cloneDynContainer == 0 && cloneDynMapContainer == 0 && cloneContainer != 0) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      if (container->getElementHoldMode(i) == HoldMode::SHARED_REF) {
        auto childElement = getSharedPtr(container->getElement(i));
        cloneContainer->setElement(i, _clone(childElement, sl).get());
      } else {
        cloneContainer->setElement(i, container->getElement(i));
      }
    }
  }

  if (sl != 0) addSourceLocation(clone.get(), sl);

  return clone;
}

} // namespace

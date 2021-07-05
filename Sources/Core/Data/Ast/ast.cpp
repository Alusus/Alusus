/**
 * @file Core/Data/Ast/ast.cpp
 * Contains the global implementations of Data::Ast namespace's declarations.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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
  Definition *def, DynamicContaining<TiObject> *target, Int &index, Data::Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(def, target, noticeStore);
  Core::Data::Ast::Identifier ref({{ S("value"), def->getName() }});
  Bool result = true;
  Bool found = false;
  seeker->extForeach(&ref, target->getTiObject(),
    [=,&result,&found](TiInt action, TiObject *obj)->Seeker::Verb {
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
    if (index == -1) target->addElement(def);
    else target->insertElement(index++, def);
  }
  return result;
}


Bool addPossiblyMergeableElement(
  TiObject *src, DynamicContaining<TiObject> *target, Int &index, Data::Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(src, target, noticeStore);
  if (src->isDerivedFrom<Definition>()) {
    auto def = static_cast<Definition*>(src);
    if (def->isToMerge()) {
      return mergeDefinition(def, target, index, seeker, noticeStore);
    } else {
      if (index == -1) target->addElement(src);
      else target->insertElement(index++, src);
    }
  } else if(src->isDerivedFrom<MergeList>()){
    auto mergeList = static_cast<MergeList*>(src);
    for (Int i = 0; i < mergeList->getElementCount(); ++i) {
      if (index == -1) target->addElement(mergeList->getElement(i));
      else target->insertElement(index++, mergeList->getElement(i));
    }
  } else {
    if (index == -1) target->addElement(src);
    else target->insertElement(index++, src);
  }
  return true;
}


Bool addPossiblyMergeableElement(
  TiObject *src, DynamicContaining<TiObject> *target, Data::Seeker *seeker, Notices::Store *noticeStore
) {
  Int index = -1;
  return addPossiblyMergeableElement(src, target, index, seeker, noticeStore);
}


Bool addPossiblyMergeableElements(
  Containing<TiObject> *src, DynamicContaining<TiObject> *target, Int &index,
  Data::Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(src, target, noticeStore);
  Bool result = true;
  for (Int i = 0; i < src->getElementCount(); ++i) {
    if (!addPossiblyMergeableElement(src->getElement(i), target, index, seeker, noticeStore)) result = false;
  }
  return result;
}


Bool addPossiblyMergeableElements(
  Containing<TiObject> *src, DynamicContaining<TiObject> *target, Data::Seeker *seeker, Notices::Store *noticeStore
) {
  Int index = -1;
  return addPossiblyMergeableElements(src, target, index, seeker, noticeStore);
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


TioSharedPtr _clone(TiObject *obj, SourceLocation *sl)
{
  if (ti_cast<Node>(obj) == 0) return getSharedPtr(obj);

  auto factory = obj->getMyTypeInfo()->getFactory();
  if (!factory) {
    throw EXCEPTION(GenericException, S("A Node derived class is missing a type factory."));
  }
  auto clone = factory->createShared();

  auto bindings = ti_cast<Binding>(obj);
  auto cloneBindings = clone.ti_cast_get<Binding>();
  if (cloneBindings != 0) {
    for (Int i = 0; i < bindings->getMemberCount(); ++i) {
      if (bindings->getMemberHoldMode(i) == HoldMode::SHARED_REF) {
        auto childMember = bindings->getMember(i);
        cloneBindings->setMember(i, _clone(childMember, sl).get());
      } else {
        cloneBindings->setMember(i, bindings->getMember(i));
      }
    }
  }

  auto dynMapContainer = ti_cast<DynamicMapContaining<TiObject>>(obj);
  auto cloneDynMapContainer = clone.ti_cast_get<DynamicMapContaining<TiObject>>();
  if (cloneDynMapContainer != 0) {
    for (Int i = 0; i < dynMapContainer->getElementCount(); ++i) {
      if (dynMapContainer->getElementHoldMode(i) == HoldMode::SHARED_REF) {
        auto childElement = dynMapContainer->getElement(i);
        cloneDynMapContainer->addElement(dynMapContainer->getElementKey(i), _clone(childElement, sl).get());
      } else {
        cloneDynMapContainer->addElement(dynMapContainer->getElementKey(i), dynMapContainer->getElement(i));
      }
    }
  }

  auto dynContainer = ti_cast<DynamicContaining<TiObject>>(obj);
  auto cloneDynContainer = clone.ti_cast_get<DynamicContaining<TiObject>>();
  if (cloneDynContainer != 0) {
    for (Int i = 0; i < dynContainer->getElementCount(); ++i) {
      if (dynContainer->getElementHoldMode(i) == HoldMode::SHARED_REF) {
        auto childElement = dynContainer->getElement(i);
        cloneDynContainer->addElement(_clone(childElement, sl).get());
      } else {
        cloneDynContainer->addElement(dynContainer->getElement(i));
      }
    }
  }

  auto container = ti_cast<Containing<TiObject>>(obj);
  auto cloneContainer = clone.ti_cast_get<Containing<TiObject>>();
  if (cloneDynContainer == 0 && cloneDynMapContainer == 0 && cloneContainer != 0) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      if (container->getElementHoldMode(i) == HoldMode::SHARED_REF) {
        auto childElement = container->getElement(i);
        cloneContainer->setElement(i, _clone(childElement, sl).get());
      } else {
        cloneContainer->setElement(i, container->getElement(i));
      }
    }
  }

  if (sl != 0) addSourceLocation(clone.get(), sl);

  return clone;
}


Bool isEqual(TiObject *obj1, TiObject *obj2)
{
  if (obj1 == 0 && obj2 == 0) return true;
  else if (obj1 == 0 || obj2 == 0) return false;

  if (obj1->getMyTypeInfo() != obj2->getMyTypeInfo()) return false;

  // Check basic types.

  if (obj1->isDerivedFrom<TiInt>()) {
    auto tiInt1 = static_cast<TiInt*>(obj1);
    auto tiInt2 = static_cast<TiInt*>(obj2);
    return tiInt1->get() == tiInt2->get();
  } else if (obj1->isDerivedFrom<TiWord>()) {
    auto tiWord1 = static_cast<TiWord*>(obj1);
    auto tiWord2 = static_cast<TiWord*>(obj2);
    return tiWord1->get() == tiWord2->get();
  } else if (obj1->isDerivedFrom<TiFloat>()) {
    auto tiFloat1 = static_cast<TiFloat*>(obj1);
    auto tiFloat2 = static_cast<TiFloat*>(obj2);
    return tiFloat1->get() == tiFloat2->get();
  } else if (obj1->isDerivedFrom<TiBool>()) {
    auto tiBool1 = static_cast<TiBool*>(obj1);
    auto tiBool2 = static_cast<TiBool*>(obj2);
    return tiBool1->get() == tiBool2->get();
  } else if (obj1->isDerivedFrom<TiStr>()) {
    auto tiStr1 = static_cast<TiStr*>(obj1);
    auto tiStr2 = static_cast<TiStr*>(obj2);
    return tiStr1->getStr() == tiStr2->getStr();
  } else if (obj1->isDerivedFrom<TiWStr>()) {
    auto tiStr1 = static_cast<TiWStr*>(obj1);
    auto tiStr2 = static_cast<TiWStr*>(obj2);
    return tiStr1->getWStr() == tiStr2->getWStr();
  }

  // Check class properties.

  auto bindings1 = ti_cast<Binding>(obj1);
  auto bindings2 = ti_cast<Binding>(obj2);
  if (bindings1 != 0 && bindings2 != 0) {
    for (Int i = 0; i < bindings1->getMemberCount(); ++i) {
      if (!isEqual(bindings1->getMember(i), bindings2->getMember(i))) return false;
    }
  } else if (bindings1 != 0 || bindings2 != 0) return false;

  auto mapContainer1 = ti_cast<MapContaining<TiObject>>(obj1);
  auto mapContainer2 = ti_cast<MapContaining<TiObject>>(obj2);
  if (mapContainer1 != 0 && mapContainer2 != 0) {
    for (Int i = 0; i < mapContainer1->getElementCount(); ++i) {
      if (mapContainer1->getElementKey(i) != mapContainer2->getElementKey(i)) return false;
    }
  } else if (mapContainer1 != 0 || mapContainer2 != 0) return false;

  auto container1 = ti_cast<Containing<TiObject>>(obj1);
  auto container2 = ti_cast<Containing<TiObject>>(obj2);
  if (container1 != 0 && container2 != 0) {
    for (Int i = 0; i < container1->getElementCount(); ++i) {
      if (!isEqual(container1->getElement(i), container2->getElement(i))) return false;
    }
  } else if (container1 != 0 || container2 != 0) return false;

  return true;
}

} // namespace

/**
 * @file Core/Ast/ast.cpp
 * Contains the global implementations of Core::Ast namespace's declarations.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Ast
{

//============================================================================
// Global Functions

Node* findOwner(Node *obj, TypeInfo const *typeInfo)
{
  while (obj != 0) {
    if (obj->isDerivedFrom(typeInfo)) break;
    obj = obj->getOwner();
  }
  return obj;
}


SharedPtr<SourceLocation> const& findSourceLocation(Node const *node)
{
  if (node != 0) return node->findSourceLocation();
  else return SharedPtr<SourceLocation>::null;
}


void addSourceLocation(Node *node, SourceLocation *sl)
{
  if (sl == 0) return;
  if (node == 0) return;

  auto currentSl = node->findSourceLocation();
  if (currentSl == 0) {
    node->setSourceLocation(sl);
  } else if (currentSl.get() == sl) {
    return;
  } else {
    auto newSl = newSrdObj<SourceLocationStack>();
    newSl->push(sl);
    newSl->push(currentSl.get());
    node->setSourceLocation(newSl);
  }
}


Bool mergeDefinition(
  Definition *def, DynamicContaining<Node> *target, Int &index, Ast::Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(def, target, noticeStore);
  Identifier ref({{ S("value"), def->getName() }});
  Bool result = true;
  Bool found = false;
  auto targetObj = ti_cast<Node>(target->getTiObject());
  if (targetObj != 0) {
    seeker->extForeach(&ref, targetObj,
      [=,&result,&found](TiInt action, Ast::Node *obj, Ast::Seeker::NoticePtr const &notice)->Seeker::Verb {
        if (action != Seeker::Action::TARGET_MATCH) return Seeker::Verb::MOVE;
        found = true;
        auto targetObj = ti_cast<Mergeable>(obj);
        if (targetObj == 0) {
          noticeStore->add(
            newSrdObj<Core::Notices::IncompatibleDefMergeNotice>(findSourceLocation(def))
          );
          result = false;
          return Seeker::Verb::STOP;
        }
        auto targetDef = obj != 0 ? ti_cast<Definition>(obj->getOwner()) : 0;
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
  }
  if (!found) {
    if (index == -1) target->addElement(def);
    else target->insertElement(index++, def);
  }
  return result;
}


Bool addPossiblyMergeableElement(
  Node *src, DynamicContaining<Node> *target, Int &index, Seeker *seeker, Notices::Store *noticeStore
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
  Node *src, DynamicContaining<Node> *target, Seeker *seeker, Notices::Store *noticeStore
) {
  Int index = -1;
  return addPossiblyMergeableElement(src, target, index, seeker, noticeStore);
}


Bool addPossiblyMergeableElements(
  Containing<Node> *src, DynamicContaining<Node> *target, Int &index,
  Seeker *seeker, Notices::Store *noticeStore
) {
  VALIDATE_NOT_NULL(src, target, noticeStore);
  Bool result = true;
  for (Int i = 0; i < src->getElementCount(); ++i) {
    if (!addPossiblyMergeableElement(src->getElement(i), target, index, seeker, noticeStore)) result = false;
  }
  return result;
}


Bool addPossiblyMergeableElements(
  Containing<Node> *src, DynamicContaining<Node> *target, Seeker *seeker, Notices::Store *noticeStore
) {
  Int index = -1;
  return addPossiblyMergeableElements(src, target, index, seeker, noticeStore);
}


void translateModifier(Grammar::SymbolDefinition *symbolDef, Node *modifier)
{
  if (modifier->isDerivedFrom<Identifier>()) {
    auto identifier = static_cast<Identifier*>(modifier);
    identifier->setValue(symbolDef->getTranslatedModifierKeyword(identifier->getValue().get()));
  } else if (modifier->isDerivedFrom<LinkOperator>()) {
    auto link = static_cast<LinkOperator*>(modifier);
    translateModifier(symbolDef, link->getFirst().get());
  } else if (modifier->isDerivedFrom<ParamPass>()) {
    auto paramPass = static_cast<ParamPass*>(modifier);
    translateModifier(symbolDef, paramPass->getOperand().get());
  }
}


SharedPtr<Node> _clone(Node *obj, SourceLocation *sl)
{
  if (obj == 0) return getSharedPtr(obj);

  auto factory = obj->getMyTypeInfo()->getFactory();
  if (!factory) {
    throw EXCEPTION(GenericException, S("A Node derived class is missing a type factory."));
  }
  auto clone = factory->createShared().s_cast<Node>();

  auto bindings = ti_cast<Binding>(obj);
  auto cloneBindings = clone.ti_cast_get<Binding>();
  if (cloneBindings != 0) {
    for (Int i = 0; i < bindings->getMemberCount(); ++i) {
      if (bindings->getMemberHoldMode(i) == HoldMode::SHARED_REF) {
        auto childMember = ti_cast<Node>(bindings->getMember(i));
        if (childMember != 0) {
          cloneBindings->setMember(i, _clone(childMember, sl).get());
        } else {
          cloneBindings->setMember(i, bindings->getMember(i));
        }
      } else {
        cloneBindings->setMember(i, bindings->getMember(i));
      }
    }
  }

  auto dynMapContainer = ti_cast<DynamicMapContaining<Node>>(obj);
  auto cloneDynMapContainer = clone.ti_cast_get<DynamicMapContaining<Node>>();
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

  auto dynContainer = ti_cast<DynamicContaining<Node>>(obj);
  auto cloneDynContainer = clone.ti_cast_get<DynamicContaining<Node>>();
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

  auto container = ti_cast<Containing<Node>>(obj);
  auto cloneContainer = clone.ti_cast_get<Containing<Node>>();
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


SharedPtr<SourceLocation> cloneSourceLocation(SourceLocation const *sl)
{
  auto record = ti_cast<SourceLocationRecord const>(sl);
  if (record != 0) {
    return newSrdObj<SourceLocationRecord>(*record);
  }
  
  auto stack = ti_cast<SourceLocationStack const>(sl);
  if (stack != 0) {
    auto newSl = newSrdObj<SourceLocationStack>();
    for (Int i = 0; i < stack->getCount(); ++i) {
      newSl->add(cloneSourceLocation(stack->get(i).get()));
    }
    return newSl;
  }

  throw EXCEPTION(InvalidArgumentException, S("sl"), S("Arg is null or of invalid type"), sl);
}


Bool isEqual(Node *obj1, Node *obj2)
{
  return _isEqual(obj1, obj2);
}

Bool _isEqual(TiObject *obj1, TiObject *obj2)
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
      if (!_isEqual(bindings1->getMember(i), bindings2->getMember(i))) return false;
    }
  } else if (bindings1 != 0 || bindings2 != 0) return false;

  auto mapContainer1 = ti_cast<MapContaining<Node>>(obj1);
  auto mapContainer2 = ti_cast<MapContaining<Node>>(obj2);
  if (mapContainer1 != 0 && mapContainer2 != 0) {
    for (Int i = 0; i < mapContainer1->getElementCount(); ++i) {
      if (mapContainer1->getElementKey(i) != mapContainer2->getElementKey(i)) return false;
    }
  } else if (mapContainer1 != 0 || mapContainer2 != 0) return false;

  auto container1 = ti_cast<Containing<Node>>(obj1);
  auto container2 = ti_cast<Containing<Node>>(obj2);
  if (container1 != 0 && container2 != 0) {
    for (Int i = 0; i < container1->getElementCount(); ++i) {
      if (!_isEqual(container1->getElement(i), container2->getElement(i))) return false;
    }
  } else if (container1 != 0 || container2 != 0) return false;

  return true;
}

void dumpAst(OutStream &stream, TiObject *ptr, int indents)
{
  if (ptr == 0) {
    stream << S("NULL");
    return;
  }

  auto printable = ti_cast<Printable>(ptr);
  if (printable) {
    printable->print(stream, indents);
  } else {
    stream << ptr->getMyTypeInfo()->getUniqueName();
    auto node = ti_cast<Ast::Node>(ptr);
    if (node != 0) {
      Word id = node->getProdId();
      if (id != UNKNOWN_ID) {
        stream << S(" [") << ID_GENERATOR->getDesc(id) << S("]");
      }
    }
    MapContaining<Ast::Node> *mapContainer;
    Containing<Ast::Node> *container;
    if ((mapContainer = ptr->getInterface<MapContaining<Ast::Node>>()) != 0) {
      for (Word i = 0; i < mapContainer->getElementCount(); ++i) {
        stream << S("\n");
        printIndents(stream, indents+1);
        stream << mapContainer->getElementKey(i) << S(": ");
       dumpAst(stream, mapContainer->getElement(i), indents+1);
      }
    } else if ((container = ptr->getInterface<Containing<Ast::Node>>()) != 0) {
      for (Word i = 0; i < container->getElementCount(); ++i) {
        stream << S("\n");
        printIndents(stream, indents + 1);
       dumpAst(stream, container->getElement(i), indents+1);
      }
    } else if (ptr->isA<TiWord>()) {
      auto tiWord = static_cast<TiWord*>(ptr);
      stream << S(": ") << tiWord->get();
    } else if (ptr->isA<TiInt>()) {
      auto tiInt = static_cast<TiInt*>(ptr);
      stream << S(": ") << tiInt->get();
    } else if (ptr->isA<TiFloat>()) {
      auto tiFloat = static_cast<TiFloat*>(ptr);
      stream << S(": ") << tiFloat->get();
    } else if (ptr->isA<TiStr>()) {
      auto tiStr = static_cast<TiStr*>(ptr);
      stream << S("\n");
      printIndents(stream, indents+1);
      stream << tiStr->get();
    }

    // No-op statement to work around a bug in GCC optimizer causing undefined behavior.
    ptr->isA<TiObject>();
  }
}


SharedPtr<SourceLocation> concatSourceLocation(SourceLocation *sl1, SourceLocation *sl2)
{
  if (sl1 == 0) return getSharedPtr(sl2);
  else if (sl2 == 0 || sl2 == sl1) return getSharedPtr(sl1);

  auto newSl = newSrdObj<SourceLocationStack>();
  newSl->push(sl2);
  newSl->push(sl1);
  return newSl;
}


SharedPtr<SourceLocation> concatFlattenedSourceLocation(SourceLocation *sl, SourceLocationStack const &stack)
{
  if (stack.getCount() == 0) return getSharedPtr(sl);
  else if (sl == 0 && stack.getCount() == 1) return stack.get(0);

  auto newSl = newSrdObj<SourceLocationStack>();
  for (Int i = 0; i < stack.getCount(); ++i) newSl->push(stack.get(i).get());
  if (sl != 0) newSl->push(sl);
  return newSl;
}


Bool isEqual(SourceLocation const *sl1, SourceLocation const *sl2)
{
  if (sl1 == sl2) return true;
  auto stack1 = ti_cast<SourceLocationStack const>(sl1);
  if (stack1 == 0) return false;
  auto stack2 = ti_cast<SourceLocationStack const>(sl2);
  if (stack2 == 0) return false;
  if (stack1->getCount() != stack1->getCount()) return false;
  for (Int i = 0; i < stack1->getCount(); ++i) {
    if (!isEqual(stack1->get(i).get(), stack2->get(i).get())) return false;
  }
  return true;
}

} // namespace

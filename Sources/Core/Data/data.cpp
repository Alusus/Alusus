/**
 * @file Core/Data/data.cpp
 * Contains the global implementations of Data namespace's declarations.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"
#include <stdlib.h>
#include <string.h>

namespace Core { namespace Data
{

//============================================================================
// Global Functions

void clearCaches(TiObject *obj)
{
  if (obj == 0) return;
  auto dh = ti_cast<CacheHaving>(obj);
  if (dh != 0) dh->clearCache();

  auto binding = ti_cast<Binding>(obj);
  if (binding != 0) {
    for (Int i = 0; i < binding->getMemberCount(); ++i) clearCaches(binding->getMember(i));
  }

  auto containing = ti_cast<Containing<TiObject>>(obj);
  if (containing != 0) {
    for (Int i = 0; i < containing->getElementCount(); ++i) clearCaches(containing->getElement(i));
  }
}


Node* findOwner(Node *obj, TypeInfo const *typeInfo)
{
  while (obj != 0) {
    if (obj->isDerivedFrom(typeInfo)) break;
    obj = obj->getOwner();
  }
  return obj;
}


void dumpData(OutStream &stream, TiObject *ptr, int indents)
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
    auto metadata = ti_cast<Ast::MetaHaving>(ptr);
    if (metadata) {
      Word id = metadata->getProdId();
      if (id != UNKNOWN_ID) {
        stream << S(" [") << IdGenerator::getSingleton()->getDesc(id) << S("]");
      }
    }
    MapContaining<TiObject> *mapContainer;
    Containing<TiObject> *container;
    if ((mapContainer = ptr->getInterface<MapContaining<TiObject>>()) != 0) {
      for (Word i = 0; i < mapContainer->getElementCount(); ++i) {
        stream << S("\n");
        printIndents(stream, indents+1);
        stream << mapContainer->getElementKey(i) << S(": ");
        dumpData(stream, mapContainer->getElement(i), indents+1);
      }
    } else if ((container = ptr->getInterface<Containing<TiObject>>()) != 0) {
      for (Word i = 0; i < container->getElementCount(); ++i) {
        stream << S("\n");
        printIndents(stream, indents + 1);
        dumpData(stream, container->getElement(i), indents+1);
      }
    } else if (ptr->isA<TioSharedBox>()) {
      TioSharedBox *sharedBox = static_cast<TioSharedBox*>(ptr);
      stream << S("\n");
      printIndents(stream, indents+1);
      dumpData(stream, sharedBox->get().get(), indents+1);
    } else if (ptr->isA<TioWeakBox>()) {
      TioWeakBox *weakBox = static_cast<TioWeakBox*>(ptr);
      stream << S("\n");
      printIndents(stream, indents+1);
      dumpData(stream, weakBox->get().get(), indents+1);
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

} } // namespace

/**
 * @file Core/Data/data.cpp
 * Contains the global implementations of Data namespace's declarations.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdlib.h>
#include <string.h>

namespace Core { namespace Data
{

//============================================================================
// Global Functions

void unsetIndexes(TiObject *obj, Int from, Int to)
{
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, S("obj"), S("Obj is null."));
  }
  DataHaving *mt = obj->getInterface<DataHaving>();
  if (mt != 0) mt->unsetIndexes(from, to);
}


void clearCaches(TiObject *obj)
{
  if (obj == 0) return;
  auto dh = ti_cast<DataHaving>(obj);
  if (dh != 0) dh->unsetIndexes(0, 1000);

  auto binding = ti_cast<Binding>(obj);
  if (binding != 0) {
    for (Int i = 0; i < binding->getMemberCount(); ++i) clearCaches(binding->getMember(i));
  }

  auto containing = ti_cast<Containing<TiObject>>(obj);
  if (containing != 0) {
    for (Int i = 0; i < containing->getElementCount(); ++i) clearCaches(containing->getElement(i));
  }
}


Node* findOwner(Node *obj, TypeInfo *typeInfo)
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
        stream << mapContainer->getElementKey(i).c_str() << S(": ");
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
      dumpData(stream, weakBox->get().lock().get(), indents+1);
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


Word getSourceLocationRecordCount(SourceLocation const *sl)
{
  if (sl == 0) return 0;
  else if (sl->isA<SourceLocationRecord>())  return 1;
  else return static_cast<SourceLocationStack const*>(sl)->getCount();
}

} } // namespace

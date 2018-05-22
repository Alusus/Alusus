/**
 * @file Core/Data/data.cpp
 * Contains the global implementations of Data namespace's declarations.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
    throw EXCEPTION(InvalidArgumentException, STR("obj"), STR("Obj is null."));
  }
  DataOwner *mt = obj->getInterface<DataOwner>();
  if (mt != 0) mt->unsetIndexes(from, to);
}


void setTreeIds(TiObject *obj, Node *root)
{
  StrStream stream;
  Node *node = tio_cast<Node>(obj);
  if (node != 0) generateId(node, root, stream);
  setTreeIds(obj, root, stream.str().c_str());
}


void setTreeIds(TiObject *obj, Node *root, const Char *id)
{
  IdHolder *idh = tii_cast<IdHolder>(obj);
  if (idh != 0) idh->setId(ID_GENERATOR->getId(id));

  StrStream childId;
  Basic::MapContaining<TiObject> *map; Basic::Containing<TiObject> *list;
  if ((map = tii_cast<Basic::MapContaining<TiObject>>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < map->getElementCount(); ++i) {
      childId.str(Str());
      childId << id;
      if (childId.tellp() != 0) childId << CHR('.');
      childId << map->getElementKey(i).c_str();
      setTreeIds(map->getElement(i), root, childId.str().c_str());
    }
  } else if ((list = tii_cast<Basic::Containing<TiObject>>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < list->getElementCount(); ++i) {
      childId.str(Str());
      childId << id;
      if (childId.tellp() != 0) childId << CHR('.');
      childId << i;
      setTreeIds(list->getElement(i), root, childId.str().c_str());
    }
  }
}


void generateId(Node *obj, Node *root, StrStream &id)
{
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("obj"), STR("Value is null."));
  }
  Node *owner = obj->getOwner();
  if (owner == 0 || obj == root) return;
  generateId(owner, root, id);
  if (id.tellp() != 0) id << CHR('.');
  auto container = owner->getInterface<Basic::Containing<TiObject>>();
  if (container != 0) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      if (container->getElement(i) == obj) {
        auto mapContainer = owner->getInterface<Basic::MapContaining<TiObject>>();
        if (mapContainer != 0) {
          id << mapContainer->getElementKey(i).c_str();
        } else {
          id << i;
        }
        return;
      }
    }
    throw EXCEPTION(GenericException, STR("The provided object has an invalid owner."));
  } else {
    id << STR("_");
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
    stream << STR("NULL");
    return;
  }

  auto printable = tii_cast<Printable>(ptr);
  if (printable) {
    printable->print(stream, indents);
  } else {
    stream << ptr->getMyTypeInfo()->getUniqueName();
    auto metadata = ti_cast<Ast::Metadata>(ptr);
    if (metadata) {
      Word id = metadata->getProdId();
      if (id != UNKNOWN_ID) {
        stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
      }
    }
    Basic::ListContaining<TiObject> *listContainer;
    Basic::MapContaining<TiObject> *mapContainer;
    if ((listContainer = ptr->getInterface<Basic::ListContaining<TiObject>>()) != 0) {
      for (Word i = 0; i < listContainer->getElementCount(); ++i) {
        stream << STR("\n");
        printIndents(stream, indents + 1);
        dumpData(stream, listContainer->getElement(i), indents+1);
      }
    } else if ((mapContainer = ptr->getInterface<Basic::MapContaining<TiObject>>()) != 0) {
      for (Word i = 0; i < mapContainer->getElementCount(); ++i) {
        stream << STR("\n");
        printIndents(stream, indents+1);
        stream << mapContainer->getElementKey(i).c_str() << STR(": ");
        dumpData(stream, mapContainer->getElement(i), indents+1);
      }
    } else if (ptr->isA<TioSharedBox>()) {
      TioSharedBox *sharedBox = static_cast<TioSharedBox*>(ptr);
      stream << STR("\n");
      printIndents(stream, indents+1);
      dumpData(stream, sharedBox->get().get(), indents+1);
    } else if (ptr->isA<TioWeakBox>()) {
      TioWeakBox *weakBox = static_cast<TioWeakBox*>(ptr);
      stream << STR("\n");
      printIndents(stream, indents+1);
      dumpData(stream, weakBox->get().lock().get(), indents+1);
    } else if (ptr->isA<TiWord>()) {
      auto tiWord = static_cast<TiWord*>(ptr);
      stream << STR(": ") << tiWord->get();
    } else if (ptr->isA<TiInt>()) {
      auto tiInt = static_cast<TiInt*>(ptr);
      stream << STR(": ") << tiInt->get();
    } else if (ptr->isA<TiFloat>()) {
      auto tiFloat = static_cast<TiFloat*>(ptr);
      stream << STR(": ") << tiFloat->get();
    } else if (ptr->isA<TiStr>()) {
      auto tiStr = static_cast<TiStr*>(ptr);
      stream << STR("\n");
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

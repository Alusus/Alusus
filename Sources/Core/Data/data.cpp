/**
 * @file Core/Data/data.cpp
 * Contains the global implementations of Data namespace's declarations.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data
{

//============================================================================
// Global Functions

Bool isPerform(RefOp op)
{
  if (op == RefOp::PERFORM_AND_STOP || op == RefOp::PERFORM_AND_MOVE) {
    return true;
  } else {
    return false;
  }
}


Bool isMove(RefOp op)
{
  if (op == RefOp::MOVE || op == RefOp::PERFORM_AND_MOVE) {
    return true;
  } else {
    return false;
  }
}


void unsetIndexes(TiObject *obj, Int from, Int to)
{
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("obj"), STR("Obj is null."));
  }
  DataOwner *mt = obj->getInterface<DataOwner>();
  if (mt != 0) mt->unsetIndexes(from, to);
}


void setTreeIds(TiObject *obj)
{
  StrStream stream;
  Node *node = tio_cast<Node>(obj);
  if (node != 0) generateId(node, stream);
  setTreeIds(obj, stream.str().c_str());
}


void setTreeIds(TiObject *obj, const Char *id)
{
  IdHolder *idh = tii_cast<IdHolder>(obj);
  if (idh != 0) idh->setId(ID_GENERATOR->getId(id));

  StrStream childId;
  MapContainer *map; Container *list;
  if ((map = tii_cast<MapContainer>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < map->getCount(); ++i) {
      childId.str(Str());
      childId << id;
      if (childId.tellp() != 0) childId << CHR('.');
      childId << map->getKey(i).c_str();
      setTreeIds(map->get(i), childId.str().c_str());
    }
  } else if ((list = tii_cast<Container>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < list->getCount(); ++i) {
      childId.str(Str());
      childId << id;
      if (childId.tellp() != 0) childId << CHR('.');
      childId << i;
      setTreeIds(list->get(i), childId.str().c_str());
    }
  }
}


void generateId(Node *obj, StrStream &id)
{
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("obj"), STR("Value is null."));
  }
  Node *owner = obj->getOwner();
  if (owner == 0) return;
  generateId(owner, id);
  if (id.tellp() != 0) id << CHR('.');
  Container *container = owner->getInterface<Container>();
  if (container != 0) {
    for (Int i = 0; i < container->getCount(); ++i) {
      if (container->get(i) == obj) {
        MapContainer *mapContainer = owner->getInterface<MapContainer>();
        if (mapContainer != 0) {
          id << mapContainer->getKey(i).c_str();
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


Bool matchCharGroup(WChar ch, CharGroupUnit *unit)
{
  ASSERT(unit);

  if (unit->isA<SequenceCharGroupUnit>()) {
    SequenceCharGroupUnit *u = static_cast<SequenceCharGroupUnit*>(unit);
    if (u->getStartCode() == 0 && u->getEndCode() == 0) {
      throw EXCEPTION(GenericException, STR("Sequence char group unit is not configured yet."));
    }
    if (ch >= u->getStartCode() && ch <= u->getEndCode()) return true;
    else return false;
  } else if (unit->isA<RandomCharGroupUnit>()) {
    RandomCharGroupUnit *u = static_cast<RandomCharGroupUnit*>(unit);
    if (u->getCharList() == 0) {
      throw EXCEPTION(GenericException, STR("Random char group unit is not configured yet."));
    }
    for (Int i = 0; i < u->getCharListSize(); i++) {
      if (u->getCharList()[i] == ch) return true;
    }
    return false;
  } else if (unit->isA<UnionCharGroupUnit>()) {
    UnionCharGroupUnit *u = static_cast<UnionCharGroupUnit*>(unit);
    if (u->getCharGroupUnits()->size() == 0) {
      throw EXCEPTION(GenericException, STR("Union char group unit is not configured yet."));
    }
    for (Int i = 0; i < static_cast<Int>(u->getCharGroupUnits()->size()); i++) {
      if (matchCharGroup(ch, u->getCharGroupUnits()->at(i).get()) == true) {
        return true;
      }
    }
    return false;
  } else if (unit->isA<InvertCharGroupUnit>()) {
    InvertCharGroupUnit *u = static_cast<InvertCharGroupUnit*>(unit);
    if (u->getChildCharGroupUnit() == 0) {
      throw EXCEPTION(GenericException, STR("Invert char group unit is not configured yet."));
    }
    if (matchCharGroup(ch, u->getChildCharGroupUnit().get()) == true) {
      return false;
    } else {
      return true;
    }
  } else {
    throw EXCEPTION(GenericException, STR("Invalid char group type."));
  }
  return false; // just to prevent warnings
}


void dumpData(OutStream &stream, TiObject *ptr, int indents)
{
  if (ptr == 0) {
    stream << STR("NULL");
  }

  auto printable = tii_cast<Printable>(ptr);
  if (printable) {
    printable->print(stream, indents);
  } else {
    stream << ptr->getMyTypeInfo()->getUniqueName();
    auto metadata = tii_cast<Ast::MetadataHolder>(ptr);
    if (metadata) {
      Word id = metadata->getProdId();
      if (id != UNKNOWN_ID) {
        stream << STR(" [") << IdGenerator::getSingleton()->getDesc(id) << STR("]");
      }
    }
    ListContainer *listContainer;
    MapContainer *mapContainer;
    NamedListContainer *namedListContainer;
    if ((listContainer = ptr->getInterface<ListContainer>()) != 0) {
      for (Word i = 0; i < listContainer->getCount(); ++i) {
        stream << STR("\n");
        printIndents(stream, indents + 1);
        dumpData(stream, listContainer->get(i), indents+1);
      }
    } else if ((mapContainer = ptr->getInterface<MapContainer>()) != 0) {
      for (Word i = 0; i < mapContainer->getCount(); ++i) {
        stream << STR("\n");
        printIndents(stream, indents+1);
        stream << mapContainer->getKey(i).c_str() << STR(": ");
        dumpData(stream, mapContainer->get(i), indents+1);
      }
    } else if ((namedListContainer = ptr->getInterface<NamedListContainer>()) != 0) {
      for (Word i = 0; i < namedListContainer->getCount(); ++i) {
        stream << STR("\n");
        printIndents(stream, indents+1);
        stream << STR("[") << namedListContainer->getName(i).c_str() << STR("] ");
        dumpData(stream, namedListContainer->get(i), indents+1);
      }
    }
  }
}

} } // namespace

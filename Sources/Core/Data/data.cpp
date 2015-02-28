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

/**
 * Call the unsetIndexes method of DataOwner interface, if implemented by
 * the object.
 */
void unsetIndexes(IdentifiableObject *obj, Int from, Int to)
{
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("obj"), STR("Obj is null."));
  }
  DataOwner *mt = obj->getInterface<DataOwner>();
  if (mt != 0) mt->unsetIndexes(from, to);
}


void setTreeIds(IdentifiableObject *obj, const Char *id)
{
  IdHolder *idh = ii_cast<IdHolder>(obj);
  if (idh != 0) idh->setId(ID_GENERATOR->getId(id));

  MapContainer *map; Container *list;
  if ((map = ii_cast<MapContainer>(obj)) != 0) {
    Str childId;
    for (Int i = 0; static_cast<Word>(i) < map->getCount(); ++i) {
      childId = id;
      childId += CHR('.');
      childId += map->getKey(i).c_str();
      setTreeIds(map->get(i), childId.c_str());
    }
  } else if ((list = ii_cast<Container>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < list->getCount(); ++i) {
      StrStream childId;
      childId << id << CHR('.') << i;
      setTreeIds(list->get(i), childId.str().c_str());
    }
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


/// Print 'indents' number of spaces.
void printIndents(int indents)
{
  for (Int i=0; i < indents; ++i) {
    outStream << STR(" ");
  }
}


void dumpParsedData(IdentifiableObject *ptr, int indents, Bool start_indent)
{
  if (start_indent) printIndents(indents);
  if (ptr == 0) {
    outStream << STR("NULL:\n");
    return;
  }

  // Is this a default data type?
  ParsingMetadataHolder *metadata;
  if ((metadata = ptr->getInterface<ParsingMetadataHolder>()) != 0) {
    // Print the production name.
    Word id = metadata->getProdId();
    if (id != UNKNOWN_ID) {
      outStream << IdGenerator::getSingleton()->getDesc(id) << STR(" -- ");
    }
  } else {
    // Unkown data type not even implementing ParsingMetadataHolder.
    outStream << ptr->getMyTypeInfo()->getUniqueName() << STR(" -- ");
  }
  // Print the data itself.
  MapContainer *mapContainer;
  ListContainer *listContainer;
  if (ptr->isDerivedFrom<ParsedList>()) {
    outStream << STR("[LIST]:\n");
    for (Word i = 0; i < static_cast<ParsedList*>(ptr)->getCount(); ++i) {
      dumpParsedData(static_cast<ParsedList*>(ptr)->get(i), indents+1);
    }
  } else if (ptr->isDerivedFrom<ParsedRoute>()) {
    outStream << STR("[ROUTE]: ");
    outStream << static_cast<ParsedRoute*>(ptr)->getRoute() << STR("\n");
    dumpParsedData(static_cast<ParsedRoute*>(ptr)->getData().get(), indents+1);
  } else if (ptr->isDerivedFrom<ParsedToken>()) {
    outStream << STR("[TOKEN]: ");
    // Print the token type.
    Int id = static_cast<ParsedToken*>(ptr)->getId();
    outStream << IdGenerator::getSingleton()->getDesc(id);
    // Print the token text.
    outStream << STR(" (\"") << static_cast<ParsedToken*>(ptr)->getText() << STR("\")\n");
  } else if ((listContainer = ptr->getInterface<ListContainer>()) != 0) {
    outStream << STR("[LIST]:\n");
    for (Word i = 0; i < listContainer->getCount(); ++i) {
      dumpParsedData(listContainer->get(i), indents+1);
    }
  } else if ((mapContainer = ptr->getInterface<MapContainer>()) != 0) {
    outStream << STR("[MAP]:\n");
    for (Word i = 0; i < mapContainer->getCount(); ++i) {
      printIndents(indents+1);
      outStream << mapContainer->getKey(i).c_str() << STR(": ");
      dumpParsedData(mapContainer->get(i), indents+1, false);
    }
  } else {
    // A default parsed item but not one of the three known types.
    outStream << STR("[UNKNOWN TYPE]\n");
  }
}

} } // namespace

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
    throw InvalidArgumentException(STR("obj"), STR("Core::Data::unsetIndexes"),
                                   STR("Obj is null."));
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

} } // namespace

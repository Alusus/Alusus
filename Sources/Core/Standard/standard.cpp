/**
 * @file Core/Standard/standard.cpp
 * Contains the global implementations of Standard namespace's declarations.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Standard
{

using namespace Data;

//==============================================================================
// Global Functions

/**
 * Merge a given container tree into another container tree. Merging is done by
 * setting all values in a source countainer into the destination container. If
 * the value being set is a container itself, the function will recursively call
 * itself. For map containers, the value will override any value with the same
 * key in the destination container. For list containers, the source values will
 * be appended at the end of the dest list. The function will handle plain
 * containers as well.
 */
Bool mergeContainers(IdentifiableObject *dest, IdentifiableObject *src, Parser::State *state)
{
  if (!dest->isA(src->getMyTypeInfo())) return false;

  MapContainer *destMap, *srcMap;
  MapPlainContainer *destPlainMap, *srcPlainMap;
  ListContainer *destList, *srcList;
  ListPlainContainer *destPlainList, *srcPlainList;
  if ((destMap = dest->getInterface<MapContainer>()) != 0) {
    srcMap = src->getInterface<MapContainer>();
    for (Int i = 0; i < srcMap->getCount(); ++i) {
      // Merge if it already exists, otherwise set the new value.
      Int di = destMap->findIndex(srcMap->getKey(i).c_str());
      if (di == -1 || destMap->get(di) == 0) {
        destMap->set(srcMap->getKey(i).c_str(), srcMap->get(i));
      } else if (!mergeContainers(destMap->get(di).get(), srcMap->get(i).get(), state)) {
        // Generate a build message.
        const Char *name;
        IdOwner *idOwner = srcMap->get(i)->getInterface<IdOwner>();
        if (idOwner != 0) name = ID_GENERATOR->getDesc(idOwner->getId()).c_str();
        else name = srcMap->getKey(i).c_str();
        ParsedItem *item = srcMap->get(i).io_cast_get<ParsedItem>();
        Int line=0, column=0;
        if (item != 0) {
          line = item->getLine();
          column = item->getColumn();
        }
        state->addBuildMsg(std::make_shared<RedefinitionMsg>(name, line, column));
        // Overwrite old data.
        destMap->set(srcMap->getKey(i).c_str(), srcMap->get(i));
      }
    }
  } else if ((destPlainMap = dest->getInterface<MapPlainContainer>()) != 0) {
    srcPlainMap = src->getInterface<MapPlainContainer>();
    for (Int i = 0; i < srcPlainMap->getCount(); ++i) {
      // Merge if it already exists, otherwise set the new value.
      Int di = destPlainMap->findIndex(srcPlainMap->getKey(i).c_str());
      if (di == -1 || destPlainMap->get(di) == 0) {
        destPlainMap->set(srcPlainMap->getKey(i).c_str(), srcPlainMap->get(i));
      } else if (!mergeContainers(destPlainMap->get(di), srcPlainMap->get(i), state)) {
        // Generate a build message.
        const Char *name;
        IdOwner *idOwner = srcPlainMap->get(i)->getInterface<IdOwner>();
        if (idOwner != 0) name = ID_GENERATOR->getDesc(idOwner->getId()).c_str();
        else name = srcPlainMap->getKey(i).c_str();
        ParsedItem *item = io_cast<ParsedItem>(srcPlainMap->get(i));
        Int line=0, column=0;
        if (item != 0) {
          line = item->getLine();
          column = item->getColumn();
        }
        state->addBuildMsg(std::make_shared<RedefinitionMsg>(name, line, column));
        // Overwrite old data.
        destPlainMap->set(srcPlainMap->getKey(i).c_str(), srcPlainMap->get(i));
      }
    }
  } else if ((destList = dest->getInterface<ListContainer>()) != 0) {
    srcList = src->getInterface<ListContainer>();
    for (Int i = 0; i < srcList->getCount(); ++i) {
      destList->add(srcList->get(i));
    }
  } else if ((destPlainList = dest->getInterface<ListPlainContainer>()) != 0) {
    srcPlainList = src->getInterface<ListPlainContainer>();
    for (Int i = 0; i < srcPlainList->getCount(); ++i) {
      destPlainList->add(srcPlainList->get(i));
    }
  } else {
    return false;
  }
  return true;
}


/**
 * If no object is already defined at the given qualifier, the new object will
 * be set. If an object of different type is already defined, it will be
 * overwritten after a build message is raised. If an object of the same type
 * is already defined and it supports a Container interface, the objects will
 * be merged. If the objects are the same type but does not support a
 * Container interface, the destination will be overwritten after a build msg
 * is created.
 */
void mergeDefinition(Data::Manager *manager, const Char *qualifier, const SharedPtr<IdentifiableObject> &obj,
                     Parser::State *state)
{
  IdentifiableObject *dest;
  Bool ret = manager->tryGetPlainValue(qualifier, dest);
  if (ret == false || dest == 0) {
    manager->setValue(qualifier, obj);
  } else {
    if (!mergeContainers(dest, obj.get(), state)) {
      // Generate a build message.
      ParsedItem *item = obj.io_cast_get<ParsedItem>();
      Int line=0, column=0;
      if (item != 0) {
        line = item->getLine();
        column = item->getColumn();
      }
      state->addBuildMsg(std::make_shared<RedefinitionMsg>(qualifier, line, column));
      // Overwrite old data.
      manager->setValue(qualifier, obj);
    }
  }
}

} } // namespace

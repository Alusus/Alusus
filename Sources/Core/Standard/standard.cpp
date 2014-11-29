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
Bool mergeContainers(IdentifiableObject *dest, IdentifiableObject *src, Processing::ParserState *state)
{
  if (!dest->isA(src->getMyTypeInfo())) return false;

  MapContainer *destMap, *srcMap;
  ListContainer *destList, *srcList;
  if ((destMap = dest->getInterface<MapContainer>()) != 0) {
    srcMap = src->getInterface<MapContainer>();
    for (Int i = 0; i < srcMap->getCount(); ++i) {
      // Merge if it already exists, otherwise set the new value.
      Int di = destMap->findIndex(srcMap->getKey(i).c_str());
      if (di == -1 || destMap->get(di) == 0) {
        destMap->set(srcMap->getKey(i).c_str(), srcMap->get(i));
      } else if (!mergeContainers(destMap->get(di), srcMap->get(i), state)) {
        // Generate a build message.
        Char const *name;
        IdHolder *idHolder = srcMap->get(i)->getInterface<IdHolder>();
        if (idHolder != 0) name = ID_GENERATOR->getDesc(idHolder->getId()).c_str();
        else name = srcMap->getKey(i).c_str();
        ParsingMetadataHolder *itemMeta = ii_cast<ParsingMetadataHolder>(srcMap->get(i));
        Int line=0, column=0;
        if (itemMeta != 0) {
          line = itemMeta->getLine();
          column = itemMeta->getColumn();
        }
        state->addBuildMsg(std::make_shared<RedefinitionMsg>(name, line, column));
        // Overwrite old data.
        destMap->set(srcMap->getKey(i).c_str(), srcMap->get(i));
      }
    }
  } else if ((destList = dest->getInterface<ListContainer>()) != 0) {
    srcList = src->getInterface<ListContainer>();
    for (Int i = 0; i < srcList->getCount(); ++i) {
      destList->add(srcList->get(i));
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
void mergeDefinition(Char const *qualifier, IdentifiableObject *obj, Processing::ParserState *state)
{
  auto repository = state->getDataStack();
  IdentifiableObject *dest;
  Bool ret = repository->tryGet(qualifier, dest);
  if (ret == false || dest == 0) {
    repository->set(qualifier, obj);
  } else {
    if (!mergeContainers(dest, obj, state)) {
      // Generate a build message.
      ParsingMetadataHolder *itemMeta = ii_cast<ParsingMetadataHolder>(obj);
      Int line=0, column=0;
      if (itemMeta != 0) {
        line = itemMeta->getLine();
        column = itemMeta->getColumn();
      }
      state->addBuildMsg(std::make_shared<RedefinitionMsg>(qualifier, line, column));
      // Overwrite old data.
      repository->set(qualifier, obj);
    }
  }
}

} } // namespace

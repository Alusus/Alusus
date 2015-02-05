/**
 * @file Core/Standard/standard.cpp
 * Contains the global implementations of Standard namespace's declarations.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
  #include <direct.h>
  #include <windows.h>
#else
  #include <unistd.h>
  #include <limits.h>
#endif

namespace Core { namespace Standard
{

using namespace Data;

#ifdef WINDOWS
  #define _getWorkingDirectory _getcwd
#else
  #define _getWorkingDirectory getcwd
#endif


//==============================================================================
// Global Functions

std::string getWorkingDirectory()
{
  thread_local static std::array<Char,FILENAME_MAX> currentPath;

  if (!_getWorkingDirectory(currentPath.data(), currentPath.size()))
  {
    throw GeneralException(STR("Couldn't obtain the current working directory."),
                           STR("Core::Basic::getWorkingDirectory"));
  }

  std::string path(currentPath.data());
  if (path.back() != CHR('/')) path += CHR('/');
  return path;
}


std::string getModuleDirectory()
{
  thread_local static std::array<Char,FILENAME_MAX> currentPath;

  #ifdef WINDOWS
    std::string path(currentPath.data(), GetModuleFileName(NULL, currentPath.data(), currentPath.size()));
  #else
    ssize_t count = readlink("/proc/self/exe", currentPath.data(), currentPath.size());
    std::string path(currentPath.data(), (count > 0) ? count : 0);
  #endif

  Int pos = path.rfind(CHR('/'));
  return std::string(path, 0, pos+1);
}


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

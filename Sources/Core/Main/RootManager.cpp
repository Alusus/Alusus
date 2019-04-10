/**
 * @file Core/Main/RootManager.cpp
 * Contains the implementation of class Core::Main::RootManager.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdlib.h>
#include <limits.h>

namespace Core::Main
{

//==============================================================================
// Constructor

RootManager::RootManager() : libraryManager(this), processedFiles(true)
{
  this->rootScope = Data::Ast::Scope::create();
  this->rootScope->setProdId(ID_GENERATOR->getId("Root"));
  this->exprRootScope = Data::Ast::Scope::create();
  this->exprRootScope->setProdId(ID_GENERATOR->getId("Root"));

  this->noticeSignal.connect(this->noticeSlot);

  Data::Grammar::StandardFactory factory;
  factory.createGrammar(this->rootScope.get(), this, false);
  factory.createGrammar(this->exprRootScope.get(), this, true);

  // Initialize current paths.
  this->pushSearchPath(getModuleDirectory().c_str());
  this->pushSearchPath((getModuleDirectory()+S("../Lib/")).c_str());
  this->pushSearchPath(getWorkingDirectory().c_str());
  // Add the paths from ALUSUS_LIBS environment variable, after splitting it by ':'.
  Char *alususLibs = getenv(S("ALUSUS_LIBS"));
  if (alususLibs != nullptr) {
    Str envPath = alususLibs;
    Int endPos = -1;
    Str path;
    while (endPos < static_cast<Int>(envPath.size())) {
      Int startPos = endPos+1;
      endPos = envPath.find(C(':'), startPos);
      if (endPos == Str::npos) endPos = envPath.size();
      path.assign(envPath, startPos, endPos-startPos);
      if (path.size() > 0) {
        this->pushSearchPath(path.c_str());
      }
    }
  }
  // TODO: Do we need to add the paths from LD_LIBRARY_PATH env variable?
}


//==============================================================================
// Member Functions

SharedPtr<TiObject> RootManager::parseExpression(Char const *str)
{
  Processing::Engine engine(this->exprRootScope);
  auto result = engine.processString(str, str);

  if (result == 0) {
    throw EXCEPTION(
      InvalidArgumentException,
      S("str"),
      S("Parsing did not result in a valid expression"),
      str
    );
  }

  return result;
}


SharedPtr<TiObject> RootManager::processString(Char const *str, Char const *name)
{
  Processing::Engine engine(this->rootScope);
  this->noticeSignal.relay(engine.noticeSignal);
  return engine.processString(str, name);
}


SharedPtr<TiObject> RootManager::processFile(Char const *filename, Bool allowReprocess)
{
  // Find the absolute path of the file.
  Str fullPath = this->findAbsolutePath(filename);
  if (fullPath.empty()) {
    throw EXCEPTION(FileException, filename, C('r'));
  }

  // Do not reprocess if already processed.
  if (!allowReprocess) {
    if (this->processedFiles.findIndex(fullPath.c_str()) != -1) return TioSharedPtr::null;
  }
  this->processedFiles.add(fullPath.c_str(), TioSharedPtr::null);

  // Extract the directory part and add it to the current paths.
  Int pos;
  Str searchPath;
  if ((pos = fullPath.rfind(C('/'))) != Str::npos) {
    searchPath = Str(fullPath, 0, pos+1);
    this->pushSearchPath(searchPath.c_str());
  }
  // Process the file
  Processing::Engine engine(this->rootScope);
  this->noticeSignal.relay(engine.noticeSignal);
  auto result = engine.processFile(fullPath.c_str());
  // Remove the added path, if any.
  if (!searchPath.empty()) {
    this->popSearchPath(searchPath.c_str());
  }
  // Return final results.
  return result;
}


SharedPtr<TiObject> RootManager::processStream(InStream *is, Char const *streamName)
{
  Processing::Engine engine(this->rootScope);
  this->noticeSignal.relay(engine.noticeSignal);
  return engine.processStream(is, streamName);
}


void RootManager::pushSearchPath(Char const *path)
{
  if (path == 0 || *path == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Argument is null or empty string."));
  }
  // Only accept absolute paths.
  if (*path != C('/')) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Path must be an absolute path."));
  }
  Str fullPath(path);
  if (fullPath.back() != C('/')) fullPath += C('/');
  // Only add the path if it doesn't already exists.
  // We will only check the top of the stack. If this path exists deeper in the stack then we'll
  // add it again to make it available at the top of the stack. We won't remove the other copy
  // coz we don't expect any penalties from keeping it there.
  if (!this->searchPaths.empty() && this->searchPaths.back() == fullPath) {
    ++this->searchPathCounts.back();
  } else {
    this->searchPaths.push_back(fullPath);
    this->searchPathCounts.push_back(1);
  }
}


void RootManager::popSearchPath(Char const *path)
{
  if (path == 0 || *path == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Argument is null or empty string."));
  }
  // Only accept absolute paths.
  if (*path != C('/')) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Path must be an absolute path."), path);
  }
  Str fullPath(path);
  if (fullPath.back() != C('/')) fullPath += C('/');
  // Search for the path to pop.
  for (Int i = this->searchPaths.size()-1; i >= 0; ++i) {
    if (this->searchPaths[i] == fullPath) {
      // Decrement the count and only remove it when it reaches 0.
      --this->searchPathCounts[i];
      if (this->searchPathCounts[i] == 0) {
        this->searchPaths.erase(this->searchPaths.begin()+i);
        this->searchPathCounts.erase(this->searchPathCounts.begin()+i);
      }
      return;
    }
  }
  throw EXCEPTION(InvalidArgumentException, S("path"), S("Path was not found in the stack."), path);
}


Str RootManager::findAbsolutePath(Char const *filename)
{
  if (filename == 0 || *filename == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("filename"), S("Argument is null or empty string."));
  }

  // Is the filename an absolute path already?
  if (filename[0] == C('/')) {
    Char canonicalPath[PATH_MAX];
    realpath(filename, canonicalPath);
    return Str(canonicalPath);
  }

  // Try all current paths.
  Str fullPath;
  std::ifstream fin;
  for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
    fullPath = this->searchPaths[i];
    if (fullPath.back() != C('/')) fullPath += C('/');
    fullPath += filename;

    // Check if the file exists.
    fin.open(fullPath);
    if (!fin.fail()) {
      Char canonicalPath[PATH_MAX];
      realpath(fullPath.c_str(), canonicalPath);
      return Str(canonicalPath);
    }
  }

  // No file was found with that name.
  return Str();
}

} // namespace

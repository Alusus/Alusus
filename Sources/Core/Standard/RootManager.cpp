/**
 * @file Core/Standard/RootManager.cpp
 * Contains the implementation of class Core::Standard::RootManager.
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

//==============================================================================
// Constructor

RootManager::RootManager() : grammarPlant(this, false), exprGrammarPlant(this, true), libraryManager(this)
{
  this->rootScope = Data::Ast::Scope::create();

  // Initialize current paths.
  this->pushSearchPath(getModuleDirectory().c_str());
  this->pushSearchPath((getModuleDirectory()+STR("../Lib/")).c_str());
  this->pushSearchPath(getWorkingDirectory().c_str());
  // Add the paths from ALUSUS_LIBS environment variable, after splitting it by ':'.
  Char *alususLibs = getenv(STR("ALUSUS_LIBS"));
  if (alususLibs != nullptr) {
    Str envPath = alususLibs;
    Int endPos = -1;
    Str path;
    while (endPos < static_cast<Int>(envPath.size())) {
      Int startPos = endPos+1;
      endPos = envPath.find(CHR(':'), startPos);
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
  Processing::Engine engine(this->exprGrammarPlant.getRepository(), SharedPtr<Data::Ast::Scope>::null);
  auto result = engine.processString(str, str);

  if (result == 0) {
    throw EXCEPTION(
      InvalidArgumentException,
      STR("str"),
      STR("Parsing did not result in a valid expression"),
      str
    );
  }

  return result;
}


SharedPtr<TiObject> RootManager::processString(Char const *str, Char const *name)
{
  Processing::Engine engine(this->grammarPlant.getRepository(), this->rootScope);
  this->buildMsgNotifier.relay(engine.buildMsgNotifier);
  return engine.processString(str, name);
}


SharedPtr<TiObject> RootManager::processFile(Char const *filename)
{
  // Find the absolute path of the file.
  Str fullPath = this->findAbsolutePath(filename);
  if (fullPath.empty()) {
    throw EXCEPTION(InvalidArgumentException, STR("filename"), STR("File not found."), filename);
  }
  // Extract the directory part and add it to the current paths.
  Int pos;
  Str searchPath;
  if ((pos = fullPath.rfind(CHR('/'))) != Str::npos) {
    searchPath = Str(fullPath, 0, pos+1);
    this->pushSearchPath(searchPath.c_str());
  }
  // Process the file
  Processing::Engine engine(this->grammarPlant.getRepository(), this->rootScope);
  this->buildMsgNotifier.relay(engine.buildMsgNotifier);
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
  Processing::Engine engine(this->grammarPlant.getRepository(), this->rootScope);
  this->buildMsgNotifier.relay(engine.buildMsgNotifier);
  return engine.processStream(is, streamName);
}


void RootManager::pushSearchPath(Char const *path)
{
  if (path == 0 || *path == CHR('\0')) {
    throw EXCEPTION(InvalidArgumentException, STR("path"), STR("Argument is null or empty string."));
  }
  // Only accept absolute paths.
  if (*path != CHR('/')) {
    throw EXCEPTION(InvalidArgumentException, STR("path"), STR("Path must be an absolute path."));
  }
  Str fullPath(path);
  if (fullPath.back() != CHR('/')) fullPath += CHR('/');
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
  if (path == 0 || *path == CHR('\0')) {
    throw EXCEPTION(InvalidArgumentException, STR("path"), STR("Argument is null or empty string."));
  }
  // Only accept absolute paths.
  if (*path != CHR('/')) {
    throw EXCEPTION(InvalidArgumentException, STR("path"), STR("Path must be an absolute path."), path);
  }
  Str fullPath(path);
  if (fullPath.back() != CHR('/')) fullPath += CHR('/');
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
  throw EXCEPTION(InvalidArgumentException, STR("path"), STR("Path was not found in the stack."), path);
}


Str RootManager::findAbsolutePath(Char const *filename)
{
  if (filename == 0 || *filename == CHR('\0')) {
    throw EXCEPTION(InvalidArgumentException, STR("filename"), STR("Argument is null or empty string."));
  }

  // Is the filename an absolute path already?
  if (filename[0] == CHR('/')) return Str(filename);

  // Try all current paths.
  Str fullPath;
  std::ifstream fin;
  for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
    fullPath = this->searchPaths[i];
    if (fullPath.back() != CHR('/')) fullPath += CHR('/');
    fullPath += filename;

    // Check if the file exists.
    fin.open(fullPath);
    if (!fin.fail()) {
      return fullPath;
    }
  }

  // No file was found with that name.
  return Str();
}

} } // namespace

/**
 * @file Core/Main/RootManager.cpp
 * Contains the implementation of class Core::Main::RootManager.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"
#include <sys/stat.h>
#include <stdlib.h>

namespace Core::Main
{

Char const *sourceExtensions[] = {
  S(".alusus"),
  S(".source"),
  S(".الأسس"),
  S(".أسس"),
  S(".مصدر")
};


//==============================================================================
// Constructor

RootManager::RootManager() : libraryManager(this), processedFiles(true)
{
  this->rootScope = Data::Ast::Scope::create();
  this->rootScope->setProdId(ID_GENERATOR->getId("Root"));
  this->exprRootScope = Data::Ast::Scope::create();
  this->exprRootScope->setProdId(ID_GENERATOR->getId("Root"));

  this->rootScopeHandler.setSeeker(&this->seeker);
  this->rootScopeHandler.setRootScope(this->rootScope);

  this->noticeSignal.relay(this->inerNoticeSignal);
  this->noticeSignal.connect(this->noticeSlot);

  Data::Grammar::StandardFactory factory;
  factory.createGrammar(this->rootScope.get(), this, false);
  factory.createGrammar(this->exprRootScope.get(), this, true);

  this->interactive = false;
  this->processArgCount = 0;
  this->processArgs = 0;

  // Initialize current paths.
  this->pushSearchPath(getModuleDirectory());
  this->pushSearchPath((getModuleDirectory()+S("../Lib/")));
  this->pushSearchPath(getWorkingDirectory());
  // Add the paths from ALUSUS_LIBS environment variable, after splitting it by ':'.
  Char *alususLibs = getenv(S("ALUSUS_LIBS"));
  if (alususLibs != nullptr) {
    Str envPath = alususLibs;
    Int endPos = -1;
    Str path;
    while (endPos < static_cast<Int>(envPath.getLength())) {
      Int startPos = endPos+1;
      endPos = envPath.find(startPos, C(':'));
      if (endPos == -1) endPos = envPath.getLength();
      path.assign(envPath, startPos, endPos-startPos);
      if (path.getLength() > 0) {
        this->pushSearchPath(path);
      }
    }
  }
  // TODO: Do we need to add the paths from LD_LIBRARY_PATH env variable?
}


//==============================================================================
// Member Functions

void RootManager::flushNotices()
{
  Int count = this->getNoticeStore()->getCount();
  if (count == 0) return;

  // Now emit the messages.
  for (Int i = 0; i < count; ++i) {
    this->inerNoticeSignal.emit(this->getNoticeStore()->get(i));
  }
  this->getNoticeStore()->flush(count);
}


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
  // Find the absolute path of the requested file.
  thread_local static std::array<Char,PATH_MAX> resultFilename;
  if (this->findFile(filename, resultFilename)) {
    return this->_processFile(resultFilename.data(), allowReprocess);
  } else {
    throw EXCEPTION(FileException, filename, C('r'));
  }
}


SharedPtr<TiObject> RootManager::_processFile(Char const *fullPath, Bool allowReprocess)
{
  // Do not reprocess if already processed.
  if (!allowReprocess) {
    if (this->processedFiles.findIndex(fullPath) != -1) return TioSharedPtr::null;
  }
  this->processedFiles.add(fullPath, TioSharedPtr::null);

  // Extract the directory part and add it to the current paths.
  Str searchPath;
  Char const *dirEnd = strrchr(fullPath, C('/'));
  if (dirEnd != 0) {
    searchPath = Str(fullPath, 0, dirEnd - fullPath + 1);
    this->pushSearchPath(searchPath);
  }

  // Process the file.
  Processing::Engine engine(this->rootScope);
  this->noticeSignal.relay(engine.noticeSignal);
  auto result = engine.processFile(fullPath);

  // Remove the added path, if any.
  if (searchPath.getLength() > 0) {
    this->popSearchPath(searchPath);
  }

  return result;
}


SharedPtr<TiObject> RootManager::processStream(Processing::CharInStreaming *is, Char const *streamName)
{
  Processing::Engine engine(this->rootScope);
  this->noticeSignal.relay(engine.noticeSignal);
  return engine.processStream(is, streamName);
}


Bool RootManager::tryImportFile(Char const *filename, Str &errorDetails)
{
  // Lookup the file in the search paths.
  Bool loadSource = false;
  thread_local static std::array<Char,PATH_MAX> resultFilename;
  if (this->findFile(filename, resultFilename)) {
    filename = resultFilename.data();
    for (Int i = 0; i < sizeof(sourceExtensions) / sizeof(sourceExtensions[0]); ++i) {
      if (compareStrSuffix(filename, sourceExtensions[i])) {
        loadSource = true;
        break;
      }
    }
  }

  if (loadSource) {
    // Load a source file.
    try {
      LOG(LogLevel::PARSER_MAJOR, S("Importing source file: ") << filename);

      this->_processFile(filename);
      return true;
    } catch (...) {
      return false;
    }
  } else {
    // Load a library.
    LOG(LogLevel::PARSER_MAJOR, S("Importing library: ") << filename);

    PtrWord id = this->getLibraryManager()->load(filename, errorDetails);

    return id != 0;
  }
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
  if (fullPath(fullPath.getLength() - 1) != C('/')) fullPath += C('/');
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
  if (fullPath(fullPath.getLength() - 1) != C('/')) fullPath += C('/');
  // Search for the path to pop.
  for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
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


Bool RootManager::findFile(Char const *filename, std::array<Char,PATH_MAX> &resultFilename)
{
  if (filename == 0 || *filename == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("filename"), S("Argument is null or empty string."));
  }

  thread_local static std::array<Char,PATH_MAX> tmpFilename;

  // Is the filename an absolute path already?
  if (filename[0] == C('/')) {
    if (this->tryFileName(filename, tmpFilename)) {
        #ifdef WINDOWS
          _fullpath(resultFilename.data(), tmpFilename.data(), PATH_MAX);
        #else
          realpath(tmpFilename.data(), resultFilename.data());
        #endif
      return true;
    }
  } else {
    // Try all current paths.
    thread_local static std::array<Char,PATH_MAX> fullPath;
    for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
      Int len = this->searchPaths[i].getLength();
      copyStr(this->searchPaths[i], fullPath.data());
      if (fullPath.data()[len - 1] != C('/')) {
        copyStr(S("/"), fullPath.data() + len);
        ++len;
      }
      copyStr(filename, fullPath.data() + len);

      if (this->tryFileName(fullPath.data(), tmpFilename)) {
        #ifdef WINDOWS
          _fullpath(resultFilename.data(), tmpFilename.data(), PATH_MAX);
        #else
          realpath(tmpFilename.data(), resultFilename.data());
        #endif
        return true;
      }
    }
  }

  // No file was found with that name.
  return false;
}


Bool RootManager::tryFileName(Char const *path, std::array<Char,PATH_MAX> &resultFilename)
{
  if (this->doesFileExist(path)) {
    copyStr(path, resultFilename.data());
    return true;
  }

  auto pathLen = getStrLen(path);

  // Try source extensions.
  for (Int i = 0; i < sizeof(sourceExtensions) / sizeof(sourceExtensions[0]); ++i) {
    copyStr(path, resultFilename.data());
    copyStr(sourceExtensions[i], resultFilename.data()+pathLen);
    if (this->doesFileExist(resultFilename.data())) return true;
  }

  Char const *filename = strrchr(path, C('/'));
  if (filename == 0) filename = path;
  else ++filename;
  Int fnIndex = filename - path;
  Int fnLen = pathLen - fnIndex;

  #ifndef RELEASE
    // Try debug library extension.
    #ifdef WINDOWS
      copyStr(path, resultFilename.data());
      copyStr(S(".dbg.dll"), resultFilename.data()+pathLen);
    #elif __APPLE__
      copyStr(path, resultFilename.data(), fnIndex);
      copyStr(S("lib"), resultFilename.data()+fnIndex);
      copyStr(filename, resultFilename.data()+fnIndex+3);
      copyStr(S(".dbg.dylib"), resultFilename.data()+fnIndex+3+fnLen);
    #else
      copyStr(path, resultFilename.data(), fnIndex);
      copyStr(S("lib"), resultFilename.data()+fnIndex);
      copyStr(filename, resultFilename.data()+fnIndex+3);
      copyStr(S(".dbg.so"), resultFilename.data()+fnIndex+3+fnLen);
    #endif
    if (this->doesFileExist(resultFilename.data())) return true;
  #endif

  // Try normal lib.
  #ifdef WINDOWS
    copyStr(path, resultFilename.data());
    copyStr(S(".dll"), resultFilename.data()+pathLen);
  #elif __APPLE__
    copyStr(path, resultFilename.data(), fnIndex);
    copyStr(S("lib"), resultFilename.data()+fnIndex);
    copyStr(filename, resultFilename.data()+fnIndex+3);
    copyStr(S(".dylib"), resultFilename.data()+fnIndex+3+fnLen);
  #else
    copyStr(path, resultFilename.data(), fnIndex);
    copyStr(S("lib"), resultFilename.data()+fnIndex);
    copyStr(filename, resultFilename.data()+fnIndex+3);
    copyStr(S(".so"), resultFilename.data()+fnIndex+3+fnLen);
  #endif
  if (this->doesFileExist(resultFilename.data())) return true;

  return false;
}


Bool RootManager::doesFileExist(Char const *filename)
{
  struct stat buffer;
  return (stat (filename, &buffer) == 0 && (buffer.st_mode & S_IFMT) != S_IFDIR);
}

} // namespace

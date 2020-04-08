/**
 * @file Core/Main/RootManager.cpp
 * Contains the implementation of class Core::Main::RootManager.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <filesystem>

// Get the shared library extention by OS.
#if defined(_WIN32)
#include "Win32Helpers.h"
#include <windows.h>
#endif

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

  this->rootScopeHandler.setRootScope(this->rootScope);

  this->noticeSignal.connect(this->noticeSlot);

  Data::Grammar::StandardFactory factory;
  factory.createGrammar(this->rootScope.get(), this, false);
  factory.createGrammar(this->exprRootScope.get(), this, true);

  this->interactive = false;
  this->processArgCount = 0;
  this->processArgs = 0;

  // Initialize current paths.
  Str path1 = std::filesystem::u8path(getModuleDirectory().c_str()).string();
  Str path2 = (std::filesystem::u8path(getModuleDirectory().c_str()) / ".." / "Lib").string();
  Str path3 = std::filesystem::u8path(getWorkingDirectory().c_str()).string();
  this->pushSearchPath(path1.c_str());
  this->pushSearchPath(path2.c_str());
  this->pushSearchPath(path3.c_str());
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
      path = std::filesystem::u8path(path.c_str()).string();
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
  // Find the absolute path of the requested file.
  thread_local static std::array<Char,PATH_MAX> resultFilename;
  if (this->findFile(filename, resultFilename)) {
    for (Int i = 0; i < sizeof(sourceExtensions) / sizeof(sourceExtensions[0]); ++i) {
      if (compareStrSuffix(resultFilename.data(), sourceExtensions[i])) {
        return this->_processFile(resultFilename.data(), allowReprocess);
      }
    }
    throw EXCEPTION(FileException, filename, C('r'), S("invalid"));
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
  Char const *dirEnd = strrchr(fullPath, std::filesystem::path::preferred_separator);
  if (dirEnd != 0) {
    searchPath = Str(fullPath, 0, dirEnd - fullPath + 1);
    this->pushSearchPath(searchPath.c_str());
  }

  // Process the file.
  Processing::Engine engine(this->rootScope);
  this->noticeSignal.relay(engine.noticeSignal);
  auto result = engine.processFile(fullPath);

  // Remove the added path, if any.
  if (!searchPath.empty()) {
    this->popSearchPath(searchPath.c_str());
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
  // Convert Unix path to Windows path in Windows OS.
  Str newFileNameCppStr = std::filesystem::u8path(filename).make_preferred().string();
  Char const *newFileName = newFileNameCppStr.c_str();

  // Lookup the file in the search paths.
  Bool loadSource = false;
  thread_local static std::array<Char,PATH_MAX> resultFilename;
  if (this->findFile(newFileName, resultFilename)) {
    newFileName = resultFilename.data();
    for (Int i = 0; i < sizeof(sourceExtensions) / sizeof(sourceExtensions[0]); ++i) {
      if (compareStrSuffix(newFileName, sourceExtensions[i])) {
        loadSource = true;
        break;
      }
    }
  }

  if (loadSource) {
    // Load a source file.
    try {
      LOG(LogLevel::PARSER_MAJOR, S("Importing source file: ") << newFileName);

      this->_processFile(newFileName);
      return true;
    } catch (...) {
      return false;
    }
  } else {
    // Load a library.
    LOG(LogLevel::PARSER_MAJOR, S("Importing library: ") << newFileName);

    PtrWord id = this->getLibraryManager()->load(newFileName, errorDetails);

    return id != 0;
  }
}


void RootManager::pushSearchPath(Char const *path)
{
  if (path == 0 || *path == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Argument is null or empty string."));
  }

  std::filesystem::path thisPath = std::filesystem::u8path(path);

  // Only accept absolute paths.
  if (!thisPath.is_absolute()) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Path must be an absolute path."));
  }

  thisPath /= ""; // Append the directory seperator.

  // Only add the path if it doesn't already exists.
  // We will only check the top of the stack. If this path exists deeper in the stack then we'll
  // add it again to make it available at the top of the stack. We won't remove the other copy
  // coz we don't expect any penalties from keeping it there.
  if (!this->searchPaths.empty() && this->searchPaths.back() == thisPath.string()) {
    ++this->searchPathCounts.back();
  } else {
    this->searchPaths.push_back(thisPath.string());
    this->searchPathCounts.push_back(1);
  }
}


void RootManager::popSearchPath(Char const *path)
{
  if (path == 0 || *path == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Argument is null or empty string."));
  }

  std::filesystem::path thisPath = std::filesystem::u8path(path);

  // Only accept absolute paths.
  if (!thisPath.is_absolute()) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Path must be an absolute path."), path);
  }

  thisPath /= ""; // Append the directory seperator.

  // Search for the path to pop.
  for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
    if (this->searchPaths[i] == thisPath.string()) {
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
  std::filesystem::path p(filename);
  if (p.is_absolute()) {
    return this->tryFileName(filename, resultFilename);
  } else {
    // Try all current paths.
    thread_local static std::array<Char,PATH_MAX> fullPath;
    for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
      Int len = this->searchPaths[i].size();
      copyStr(this->searchPaths[i].c_str(), fullPath.data());
      if (fullPath.data()[len - 1] != std::filesystem::path::preferred_separator) {
#if defined(_WIN32)
        Str tmpStr = utf8Encode(WStr(&std::filesystem::path::preferred_separator));
        for (Char c : tmpStr) {
          copyStr(&c, fullPath.data() + len);
          ++len;
        }
#else       
        copyStr(std::filesystem::path::preferred_separator, fullPath.data() + len);
        ++len;
#endif
      }
      copyStr(filename, fullPath.data() + len);

      if (this->tryFileName(fullPath.data(), tmpFilename)) {
        #if defined(_WIN32)
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

  Char const *filename = strrchr(path, std::filesystem::path::preferred_separator);
  if (filename == 0) filename = path;
  else ++filename;
  Int fnIndex = filename - path;

  Int fnLen = pathLen - fnIndex;

  #ifndef RELEASE
    // Try debug library extension.
    #if defined(_WIN32)
      copyStr(path, resultFilename.data(), fnIndex);
      copyStr(S("lib"), resultFilename.data()+fnIndex);
      copyStr(filename, resultFilename.data()+fnIndex+3);
      copyStr(S(".dbg.dll"), resultFilename.data()+fnIndex+3+fnLen);
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
  #if defined(_WIN32)
    copyStr(path, resultFilename.data(), fnIndex);
    copyStr(S("lib"), resultFilename.data()+fnIndex);
    copyStr(filename, resultFilename.data()+fnIndex+3);
    copyStr(S(".dll"), resultFilename.data()+fnIndex+3+fnLen);
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
  std::filesystem::path p = std::filesystem::u8path(filename);
  return std::filesystem::exists(p);
}

} // namespace

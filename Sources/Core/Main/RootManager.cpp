/**
 * @file Core/Main/RootManager.cpp
 * Contains the implementation of class Core::Main::RootManager.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdlib.h>
#include <limits.h>
#include <filesystem>

// Get the shared library extention by OS.
#if defined(__MINGW32__) || defined(__MINGW64__) // Windows under MinGW.
#include "win32api.h"
#include <Windows.h>
Str SHARED_LIB_EXT = "dll";
#elif defined(APPLE) // Apple (Mac OS, iOS, Apple Watch).
Str SHARED_LIB_EXT = "dylib";
#else // Linux.
Str SHARED_LIB_EXT = "so";
#endif

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
  if ((pos = fullPath.rfind(std::filesystem::path::preferred_separator)) != Str::npos) {
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

  // Check the file type.
  if (
    compareStrSuffix(newFileName, S(".source")) || compareStrSuffix(newFileName, S(".alusus")) ||
    compareStrSuffix(newFileName, S(".مصدر")) || compareStrSuffix(newFileName, S(".الأسس")) ||
    compareStrSuffix(newFileName, S(".أسس"))
  ) {
    try {
      LOG(LogLevel::PARSER_MAJOR, S("Importing source file: ") << newFileName);

      this->processFile(newFileName);
    } catch (...) {
      return false;
    }
  } else {
    Str tempStr = newFileName + Str(".") + SHARED_LIB_EXT;
    Char const* sharedLibFileName = tempStr.c_str();

    LOG(LogLevel::PARSER_MAJOR, S("Importing library: ") << sharedLibFileName);

    // Import library and return.
    PtrWord id = 0;

    #ifndef RELEASE
      // We are running in debug mode, so we will look first for a debug verion.
      // Find the first '.' after the last '/'.
      thread_local static std::array<Char,FILENAME_MAX> dbgFilename;
      Char const *dotPos = strrchr(sharedLibFileName, std::filesystem::path::preferred_separator);
      if (dotPos == 0) dotPos = sharedLibFileName;
      dotPos = strchr(dotPos, C('.'));
      if (dotPos == 0) {
        // The filename has no extension, so we'll attach .dbg to the end.
        copyStr(sharedLibFileName, dbgFilename.data());
        copyStr(S(".dbg"), dbgFilename.data()+getStrLen(sharedLibFileName));
        id = this->getLibraryManager()->load(dbgFilename.data(), errorDetails);
      } else if (compareStr(dotPos, S(".dbg."), 5) != 0) {
        // The position of the file's extension is found, and it doesn't already have
        // the .dbg extension, so we'll attach it.
        Int dotIndex = dotPos - sharedLibFileName;
        copyStr(sharedLibFileName, dbgFilename.data(), dotIndex);
        copyStr(S(".dbg"), dbgFilename.data()+dotIndex);
        copyStr(dotPos, dbgFilename.data()+dotIndex+4);
        id = this->getLibraryManager()->load(dbgFilename.data(), errorDetails);
      }
    #endif
    if (id == 0) id = this->getLibraryManager()->load(sharedLibFileName, errorDetails);
    // Did we fail the loading?
    if (id == 0) return false;
  }
  return true;
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


Str RootManager::findAbsolutePath(Char const *filename)
{
  if (filename == 0 || *filename == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("filename"), S("Argument is null or empty string."));
  }

#if defined(__MINGW32__) || defined(__MINGW64__)
  std::wifstream fin;
#else
  std::ifstream fin;
#endif

  // Is the filename an absolute path already?
  if (std::filesystem::u8path(filename).is_absolute()) {
    // Check if the file exists.
#if defined(__MINGW32__) || defined(__MINGW64__)
    WStr tempStr = utf8Decode(std::string(filename));
    fin.open(tempStr.c_str());
#else
    fin.open(filename);
#endif
    if (fin.is_open()) {
      return std::filesystem::canonical(std::filesystem::u8path(filename)).string();
    }
  }

  // Try all current paths.
  Str fullPath;
  for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
    fullPath = this->searchPaths[i];
    if (fullPath.back() != std::filesystem::path::preferred_separator) fullPath += std::filesystem::path::preferred_separator;
    fullPath += filename;

    // Check if the file exists.
#if defined(__MINGW32__) || defined(__MINGW64__)
    fin.open(utf8Decode(fullPath).c_str());
#else
    fin.open(fullPath.c_str());
#endif
    if (fin.is_open()) {
      return std::filesystem::canonical(std::filesystem::u8path(fullPath.c_str())).string();
    }
  }

  // No file was found with that name.
  return Str();
}

} // namespace

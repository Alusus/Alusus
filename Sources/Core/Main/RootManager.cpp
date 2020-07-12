/**
 * @file Core/Main/RootManager.cpp
 * Contains the implementation of class Core::Main::RootManager.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <utility>
#include <set>
#include <vector>

#include "core.h"
#include "AlususCompileDefines.h"

#if defined(_WIN32)

#undef C

// For parsing archive files.
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/Errc.h>
#include <llvm/Support/Error.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Object/Archive.h>

#undef C
#define C(x)	u8##x

#include <system_error>
#include <memory>
#include <windows.h>

#include "Win32Helpers.h"
#endif

// #if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
// #if __has_include(<filesystem>)
// #define GHC_USE_STD_FS
// #include <filesystem>
// namespace fs = std::filesystem;
// #endif
// #endif
// #ifndef GHC_USE_STD_FS
// #include <ghc/filesystem.hpp>
// namespace fs = ghc::filesystem;
// #endif
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;


namespace Core::Main
{

Char const *sourceExtensions[] = {
  S(".alusus"),
  S(".source"),
  S(".الأسس"),
  S(".أسس"),
  S(".مصدر")
};

#if defined(_WIN32)
#define PATH_SEP ';'
#else
#define PATH_SEP ':'
#endif

std::vector<std::pair<Bool, Str>> RootManager::tokenizePathEnvVar(Str pathEnvVar) {
  std::vector<std::pair<Bool, Str>> tokens;
  size_t i = 0;
  while (i < pathEnvVar.size()) {
    Char currChar = pathEnvVar[i];
    if (currChar == PATH_SEP) {
      tokens.push_back(std::make_pair(true, Str())); // Setting "true" for the first item of the pair
                                                     // to indicate a separator.
      i++;
    }
#if defined(_WIN32)
    // Windows can also escape the path separator by enclosing the entire path token in double quotes.
    else if (currChar == '\"') {
      Str currToken;
      i++; // Skip the first double quote character.
      if (i < pathEnvVar.size()) {
        currChar = pathEnvVar[i];
        while (currChar != '\"' && i < pathEnvVar.size()) {
          currToken += currChar;
          i++;
          if (i < pathEnvVar.size()) {
            currChar = pathEnvVar[i];
          }
        }
        tokens.push_back(std::make_pair(false, currToken));
        i++; // Skip the second double quote character.
      }
    }
#endif
    else {
      Str currToken;
      while (currChar != PATH_SEP && i < pathEnvVar.size()) {
        currToken += currChar;
        i++;
        if (i < pathEnvVar.size()) {
          currChar = pathEnvVar[i];
        }
      }
      tokens.push_back(std::make_pair(false, currToken));
    }
  }
  return tokens;
}

std::vector<Str> RootManager::parsePathEnvVar(Str pathEnvVar, Str emptyPath) {
  auto tokens = tokenizePathEnvVar(pathEnvVar);
  std::vector<Str> paths;
  std::set<Str> addedPaths;
  Bool expectedPath = true;

  for (auto token : tokens) {
    // This is a separator token.
    if (token.first) {
      if (expectedPath) {
        if (addedPaths.find(emptyPath) == addedPaths.end()) {
          addedPaths.insert(emptyPath);
          paths.push_back(emptyPath);
        }
      }
      expectedPath = true;
    } 
    
    // This is a text token.
    else {
      if (addedPaths.find(token.second) == addedPaths.end()) {
        addedPaths.insert(token.second);
        paths.push_back(token.second);
      }
      expectedPath = false;
    }
  }
  return paths;
}

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

  Str path1 = fs::u8path(getModuleDirectory().c_str()).string();
  path1 = fs::absolute(fs::path(path1.c_str())).u8string();
  Str path2 = (fs::u8path(getModuleDirectory().c_str()) / ".." / ALUSUS_INSTALL_LIB_DIR).string();
  path2 = fs::absolute(fs::path(path2.c_str())).u8string();
  Str path3 = fs::u8path(getWorkingDirectory().c_str()).string();
  path3 = fs::absolute(fs::path(path3.c_str())).u8string();
  this->pushSearchPath(path1.c_str());
  this->pushSearchPath(path2.c_str());
  this->pushSearchPath(path3.c_str());
#if defined(_WIN32)
  Str path4 = (fs::u8path(getModuleDirectory().c_str()) / ".." / ALUSUS_INSTALL_BIN_DIR).string();
  this->binSearchPaths.push_back(fs::absolute(fs::path(path1.c_str())).u8string());
  this->binSearchPaths.push_back(fs::absolute(fs::path(path4.c_str())).u8string());
  this->binSearchPaths.push_back(fs::absolute(fs::path(path3.c_str())).u8string());
#endif

  // Add the paths from ALUSUS_LIBS environment variable.
#if defined(_WIN32)
  // Load the environment variable as wide characters, then convert them to multibyte characters, on Windows.
  const Char *alususLibs = nullptr;
  WChar *wAlususLibs = _wgetenv(L"ALUSUS_LIBS");
  Str alususLibsStr;
  if (wAlususLibs != nullptr) {
    alususLibsStr = utf8Encode(WStr(wAlususLibs));
    alususLibs = alususLibsStr.c_str();
  }
#else
  const Char *alususLibs = getenv(S("ALUSUS_LIBS"));
#endif

  if (alususLibs != nullptr) {
    // TODO: What do we do with empty paths in the environment variable
    // (e.g. "path1:::path2::path3" in Unix or "path1;;;path2;;path3" in Windows)?
    // Do we ignore it, for example, or add the current working directory in place of the empty path
    // (e.g. "path1:<cwd>:<cwd>:path2:<cwd>:path3" in Unix or "path1;<cwd>;<cwd>;path2;<cwd>;path3"
    // in Windows, where "<cwd>" is the current working directory)?
    std::vector<Str> paths = parsePathEnvVar(Str(alususLibs));
    for (auto path : paths) {
      // TODO: Ignore empty paths for now.
      if (path.size() > 0) {
        Str fullPath = fs::absolute(fs::path(path.c_str())).u8string();
        this->pushSearchPath(fullPath.c_str());
      }
    }
  }

#if defined(_WIN32)
  // Add the DLL paths in "ALUSUS_BINS" environment variable.
  // Load the environment variable as wide characters, then convert them to multibyte characters, on Windows.
  const Char *alususBins = nullptr;
  WChar *wAlususBins = _wgetenv(L"ALUSUS_BINS");
  Str alususBinsStr;
  if (wAlususBins != nullptr) {
    alususBinsStr = utf8Encode(WStr(wAlususBins));
    alususBins = alususBinsStr.c_str();
  }

  if (alususBins != nullptr) {
    // TODO: What do we do with empty paths in the environment variable
    // (e.g. "path1:::path2::path3" in Unix or "path1;;;path2;;path3" in Windows)?
    // Do we ignore it, for example, or add the current working directory in place of the empty path
    // (e.g. "path1:<cwd>:<cwd>:path2:<cwd>:path3" in Unix or "path1;<cwd>;<cwd>;path2;<cwd>;path3"
    // in Windows, where "<cwd>" is the current working directory)?
    std::vector<Str> paths = parsePathEnvVar(Str(alususBins));
    for (auto path : paths) {
      // TODO: Ignore empty paths for now.
      if (path.size() > 0) {
        this->binSearchPaths.push_back(fs::absolute(fs::path(path.c_str())).u8string());
      }
    }
  }
#endif

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
  Char const *dirEnd = strrchr(fullPath, fs::path::preferred_separator);
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
  Str newFileNameCppStr = fs::u8path(filename).make_preferred().string();
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
#if defined(_WIN32)
    // Load all DLLs found in the import library.


    // No need to reload the import library again.
    if (this->loadedImportLibs.find(newFileName) != this->loadedImportLibs.end()) {
      return true;
    }

    // Get DLL names from the import library.
    Bool errorCheck;
    auto dllNames = this->getDLLNames(newFileName, errorCheck);
    if (errorCheck) {
      errorDetails = "Couldn't parse import library.";
      return false;
    }

    for (auto dllName : dllNames) {
      if (this->loadedBins.find(dllName) != this->loadedBins.end()) {
        continue;
      }

      // Get the DLL's full path.
      if (!this->findBinFile(dllName.c_str(), resultFilename)) {
        errorDetails = "DLL not found: " + dllName;
        return false;
      }

      // Load the DLL.
      errorDetails = "";
      PtrWord id = this->getLibraryManager()->load(resultFilename.data(), errorDetails);
      if (id == 0) {
        return false;
      }

      // Cache the DLL name.
      this->loadedBins.insert(dllName);
    }

    // All DLLs pointed by this import library have been loaded successfully,
    // and we can cache it as it has been loaded so we don't need to load it again.
    this->loadedImportLibs.insert(newFileName);

    return true;
#else
    PtrWord id = this->getLibraryManager()->load(newFileName, errorDetails);
    return id != 0;
#endif
  }
}


void RootManager::pushSearchPath(Char const *path)
{
  if (path == 0 || *path == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("path"), S("Argument is null or empty string."));
  }

  fs::path thisPath = fs::u8path(path);

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

  fs::path thisPath = fs::u8path(path);

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


#if defined(_WIN32)

Bool RootManager::findBinFile(Char const *filename, std::array<Char,PATH_MAX> &resultFilename)
{
  if (filename == 0 || *filename == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("filename"), S("Argument is null or empty string."));
  }

  for (auto path : this->binSearchPaths) {
    fs::path toCheckPath(path.c_str());
    toCheckPath /= filename;
    if (fs::exists(toCheckPath)) {
      copyStr(toCheckPath.c_str(), resultFilename.data());
      return true;
    }
  }
  return false;
}

Bool RootManager::hasEnding(Str const &fullString, Str const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::vector<Str> RootManager::getDLLNames(Str const &filename, Bool &errorCheck) {
  std::vector<Str> dlls;
  std::set<Str> addedDLLs;

  // Create or open the archive object.
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf = llvm::MemoryBuffer::getFile(filename, -1, false);
  std::error_code ec = buf.getError();
  if (ec && ec != llvm::errc::no_such_file_or_directory) {
    errorCheck = true;
    return std::vector<Str>();
  }

  if (!ec) {
    // Iterate through all objects in the archive to get the DLL names from it.
    llvm::Error err = llvm::Error::success();
    llvm::object::Archive archive(buf.get()->getMemBufferRef(), err);
    for (auto &c : archive.children(err)) {
      llvm::Expected<llvm::StringRef> nameOrErr = c.getName();
      llvm::StringRef name = nameOrErr.get();
      if (name.str().size() > 0 && this->hasEnding(name.str(), ".dll")) {
        if (addedDLLs.find(name.str()) == addedDLLs.end()) {
          addedDLLs.insert(name.str());
          dlls.push_back(name.str());
        }
      }
    }
  } else {
    errorCheck = true;
    return std::vector<Str>();
  }
  return dlls;
}

#endif


Bool RootManager::findFile(Char const *filename, std::array<Char,PATH_MAX> &resultFilename)
{
  if (filename == 0 || *filename == C('\0')) {
    throw EXCEPTION(InvalidArgumentException, S("filename"), S("Argument is null or empty string."));
  }

  thread_local static std::array<Char,PATH_MAX> tmpFilename;

  // Is the filename an absolute path already?
  fs::path p(filename);
  if (p.is_absolute()) {
    return this->tryFileName(filename, resultFilename);
  } else {
    // Try all current paths.
    thread_local static std::array<Char,PATH_MAX> fullPath;
    for (Int i = this->searchPaths.size()-1; i >= 0; --i) {
      Int len = this->searchPaths[i].size();
      copyStr(this->searchPaths[i].c_str(), fullPath.data());
      if (fullPath.data()[len - 1] != fs::path::preferred_separator) {
#if defined(_WIN32)
        Str tmpStr = utf8Encode(WStr(&fs::path::preferred_separator));
        for (Char c : tmpStr) {
          copyStr(&c, fullPath.data() + len);
          ++len;
        }
#else       
        copyStr(&fs::path::preferred_separator, fullPath.data() + len);
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

  Char const *filename = strrchr(path, fs::path::preferred_separator);
  if (filename == 0) filename = path;
  else ++filename;
  Int fnIndex = filename - path;

  Int fnLen = pathLen - fnIndex;

#ifndef RELEASE
  // Try the debug library version first.
#if defined(_WIN32)
  copyStr(path, resultFilename.data(), fnIndex);
  copyStr(S("lib"), resultFilename.data()+fnIndex);
  copyStr(filename, resultFilename.data()+fnIndex+3);
  copyStr(S(".dbg.dll.lib"), resultFilename.data()+fnIndex+3+fnLen);
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
  copyStr(S(".dll.lib"), resultFilename.data()+fnIndex+3+fnLen);
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

#if defined(_WIN32)
// Try once more with ".dll.a" on Windows.
#ifndef RELEASE
  // Try debug library extension.
  copyStr(path, resultFilename.data(), fnIndex);
  copyStr(S("lib"), resultFilename.data()+fnIndex);
  copyStr(filename, resultFilename.data()+fnIndex+3);
  copyStr(S(".dbg.dll.a"), resultFilename.data()+fnIndex+3+fnLen);
  if (this->doesFileExist(resultFilename.data())) return true;
#endif
  // Try normal lib.
  copyStr(path, resultFilename.data(), fnIndex);
  copyStr(S("lib"), resultFilename.data()+fnIndex);
  copyStr(filename, resultFilename.data()+fnIndex+3);
  copyStr(S(".dll.a"), resultFilename.data()+fnIndex+3+fnLen);
  if (this->doesFileExist(resultFilename.data())) return true;
#endif

  return false;
}


Bool RootManager::doesFileExist(Char const *filename)
{
  fs::path p = fs::u8path(filename);
  return fs::exists(p);
}

} // namespace

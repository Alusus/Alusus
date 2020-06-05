/**
 * @file Core/Main/RootManager.h
 * Contains the header of class Core::Main::RootManager.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_MAIN_ROOTMANAGER_H
#define CORE_MAIN_ROOTMANAGER_H

namespace Core::Main
{

// TODO: DOC

class RootManager : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(RootManager, TiObject, "Core.Main", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: SharedPtr<Data::Ast::Scope> rootScope;
  private: SharedPtr<Data::Ast::Scope> exprRootScope;

  private: RootScopeHandler rootScopeHandler;
  private: LibraryManager libraryManager;

  private: SharedMap<TiObject> processedFiles;

  private: std::vector<Str> searchPaths;
  private: std::vector<Int> searchPathCounts;

  private: Data::Seeker seeker;

  private: Int minNoticeSeverityEncountered = -1;

  private: Bool interactive;
  private: Int processArgCount;
  private: Char const *const *processArgs;
  private: Str language;


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: SignalRelay<void, SharedPtr<Notices::Notice> const&> noticeSignal;

  private: Slot<void, SharedPtr<Notices::Notice> const&> noticeSlot = {
    [=](SharedPtr<Notices::Notice> const &notice)->void
    {
      if (this->minNoticeSeverityEncountered == -1 || notice->getSeverity() < this->minNoticeSeverityEncountered) {
        this->minNoticeSeverityEncountered = notice->getSeverity();
      }
    }
  };


  //============================================================================
  // Constructors / Destructor

  public: RootManager();

  public: virtual ~RootManager()
  {
    this->libraryManager.unloadAll();
  }


  //============================================================================
  // Member Functions

  public: virtual SharedPtr<Data::Ast::Scope> const& getRootScope()
  {
    return this->rootScope;
  }

  public: virtual SharedPtr<Data::Ast::Scope> const& getExprRootScope()
  {
    return this->exprRootScope;
  }

  public: virtual RootScopeHandler* getRootScopeHandler()
  {
    return &this->rootScopeHandler;
  }

  public: virtual LibraryManager* getLibraryManager()
  {
    return &this->libraryManager;
  }

  public: virtual Data::Seeker* getSeeker()
  {
    return &this->seeker;
  }

  public: virtual SharedPtr<TiObject> parseExpression(Char const *str);

  public: virtual SharedPtr<TiObject> processString(Char const *str, Char const *name);

  public: virtual SharedPtr<TiObject> processFile(Char const *filename, Bool allowReprocess = false);

  private: virtual SharedPtr<TiObject> _processFile(Char const *fullPath, Bool allowReprocess = false);

  public: virtual SharedPtr<TiObject> processStream(Processing::CharInStreaming *is, Char const *streamName);

  public: virtual Bool tryImportFile(Char const *filename, Str &errorDetails);

  public: virtual void pushSearchPath(Char const *path);

  public: virtual void popSearchPath(Char const *path);

  private: virtual Bool findFile(Char const *filename, std::array<Char,PATH_MAX> &resultFilename);

  private: virtual Bool tryFileName(Char const *path, std::array<Char,PATH_MAX> &resultFilename);

  private: virtual Bool doesFileExist(Char const *filename);

  public: void resetMinNoticeSeverityEncountered()
  {
    this->minNoticeSeverityEncountered = -1;
  }

  public: Int getMinNoticeSeverityEncountered() const
  {
    return this->minNoticeSeverityEncountered;
  }

  public: void setInteractive(Bool i)
  {
    this->interactive = i;
  }

  public: Bool isInteractive() const
  {
    return this->interactive;
  }

  public: void setProcessArgInfo(Int count, Char const *const *args)
  {
    this->processArgCount = count;
    this->processArgs = args;
  }

  public: Int getProcessArgCount() const
  {
    return this->processArgCount;
  }

  public: Char const* const* getProcessArgs() const
  {
    return this->processArgs;
  }

  public: void setLanguage(Char const *lang)
  {
    this->language = lang;
  }

  public: Str const& getLanguage() const
  {
    return this->language;
  }

}; // class

} // namespace

#endif

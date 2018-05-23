/**
 * @file Core/Main/RootManager.h
 * Contains the header of class Core::Main::RootManager.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_MAIN_ROOTMANAGER_H
#define CORE_MAIN_ROOTMANAGER_H

namespace Core::Main
{

// TODO: DOC

class RootManager : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(RootManager, TiObject, "Core.Main", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: SharedPtr<Data::Ast::Scope> rootScope;
  private: SharedPtr<Data::Ast::Scope> exprRootScope;

  private: LibraryManager libraryManager;

  private: std::vector<Str> searchPaths;
  private: std::vector<Int> searchPathCounts;

  private: Data::Seeker seeker;


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: SignalRelay<void, SharedPtr<Notices::Notice> const&> noticeSignal;


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

  public: virtual SharedPtr<TiObject> processFile(Char const *filename);

  public: virtual SharedPtr<TiObject> processStream(InStream *is, Char const *streamName);

  public: virtual void pushSearchPath(Char const *path);

  public: virtual void popSearchPath(Char const *path);

  public: virtual Str findAbsolutePath(Char const *filename);

}; // class

} // namespace

#endif

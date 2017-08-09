/**
 * @file Core/Standard/RootManager.h
 * Contains the header of class Core::Standard::RootManager.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_ROOTMANAGER_H
#define CORE_STANDARD_ROOTMANAGER_H

namespace Core { namespace Standard
{

// TODO: DOC

class RootManager : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(RootManager, TiObject, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: GrammarPlant grammarPlant;
  private: GrammarPlant exprGrammarPlant;

  private: LibraryManager libraryManager;

  private: SharedPtr<Data::Ast::Scope> rootScope;
  private: Data::Seeker seeker;

  private: std::vector<Str> searchPaths;
  private: std::vector<Int> searchPathCounts;


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: SignalRelay<void, SharedPtr<Processing::BuildMsg> const&> buildMsgNotifier;


  //============================================================================
  // Constructors / Destructor

  public: RootManager();

  public: virtual ~RootManager()
  {
    this->libraryManager.unloadAll();
  }


  //============================================================================
  // Member Functions

  public: GrammarPlant* getGrammarPlant()
  {
    return &this->grammarPlant;
  }

  public: virtual Data::GrammarRepository* getGrammarRepository()
  {
    return this->grammarPlant.getRepository();
  }

  public: GrammarPlant* getExprGrammarPlant()
  {
    return &this->exprGrammarPlant;
  }

  public: virtual Data::GrammarRepository* getExprGrammarRepository()
  {
    return this->exprGrammarPlant.getRepository();
  }

  public: virtual LibraryManager* getLibraryManager()
  {
    return &this->libraryManager;
  }

  public: virtual SharedPtr<Data::Ast::Scope> const& getRootScope()
  {
    return this->rootScope;
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

} } // namespace

#endif

/**
 * @file Core/Standard/RootManager.h
 * Contains the header of class Core::Standard::RootManager.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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

class RootManager : public SignalReceiver
{
  //============================================================================
  // Type Info

  TYPE_INFO(RootManager, SignalReceiver, "Core.Standard", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: GrammarPlant grammarPlant;

  private: LibraryManager libraryManager;

  private: Data::SharedRepository definitionsRepository;

  private: std::vector<Str> searchPaths;
  private: std::vector<Int> searchPathCounts;


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: RESIGNAL(buildMsgNotifier, (const SharedPtr<Processing::BuildMsg> &msg), (msg));


  //============================================================================
  // Constructors / Destructor

  public: RootManager();

  public: virtual ~RootManager()
  {
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

  public: virtual LibraryManager* getLibraryManager()
  {
    return &this->libraryManager;
  }

  public: virtual Data::SharedRepository* getDefinitionsRepository()
  {
    return &this->definitionsRepository;
  }

  public: virtual SharedPtr<IdentifiableObject> processString(Char const *str, Char const *name)
  {
    Processing::Engine engine(this->grammarPlant.getRepository(), &this->definitionsRepository);
    engine.buildMsgNotifier.connect(this, &RootManager::buildMsgNotifierRelay);
    return engine.processString(str, name);
  }

  public: virtual SharedPtr<IdentifiableObject> processFile(Char const *filename);

  public: virtual void pushSearchPath(Char const *path);

  public: virtual void popSearchPath(Char const *path);

  public: virtual Str findAbsolutePath(Char const *filename);

}; // class

} } // namespace

#endif

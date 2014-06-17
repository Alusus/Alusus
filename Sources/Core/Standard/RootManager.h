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

#ifndef STANDARD_ROOT_MANAGER_H
#define STANDARD_ROOT_MANAGER_H

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


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: RESIGNAL(buildMsgNotifier, (const SharedPtr<Common::BuildMsg> &msg), (msg));


  //============================================================================
  // Constructors / Destructor

  public: RootManager() : grammarPlant(this), libraryManager(this), definitionsRepository(10, 10)
  {
    this->definitionsRepository.pushLevel(STR("root"), Data::Module::create({}));
  }

  public: virtual ~RootManager()
  {
  }


  //============================================================================
  // Member Functions

  public: GrammarPlant* getGrammarPlant()
  {
    return &this->grammarPlant;
  }

  public: virtual const SharedPtr<Lexer::CharGroupDefinitionList>& getCharGroupDefinitions()
  {
    return this->grammarPlant.getCharGroupDefinitions();
  }

  public: virtual const SharedPtr<Lexer::TokenDefinitionList>& getTokenDefinitions()
  {
    return this->grammarPlant.getTokenDefinitions();
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

  public: virtual SharedPtr<IdentifiableObject> processString(Char const *str)
  {
    Main::Processor processor(this->grammarPlant.getCharGroupDefinitions(),
                              this->grammarPlant.getTokenDefinitions(),
                              this->grammarPlant.getRepository());
    processor.buildMsgNotifier.connect(this, &RootManager::buildMsgNotifierRelay);
    return processor.processString(str);
  }

  public: virtual SharedPtr<IdentifiableObject> processFile(Char const *filename)
  {
    Main::Processor processor(this->grammarPlant.getCharGroupDefinitions(),
                              this->grammarPlant.getTokenDefinitions(),
                              this->grammarPlant.getRepository());
    processor.buildMsgNotifier.connect(this, &RootManager::buildMsgNotifierRelay);
    return processor.processFile(filename);
  }

}; // class

} } // namespace

#endif

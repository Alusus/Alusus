/**
 * @file Core/Processing/Engine.h
 * Contains the header of class Core::Processing::Engine.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_ENGINE_H
#define CORE_PROCESSING_ENGINE_H

namespace Core { namespace Processing
{

// TODO: DOC

class Engine : public SignalReceiver
{
  //============================================================================
  // Type Info

  TYPE_INFO(Engine, SignalReceiver, "Core.Processing", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Processing::Lexer lexer;

  private: Processing::Parser parser;


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: RESIGNAL(buildMsgNotifier, (const SharedPtr<Processing::BuildMsg> &msg), (msg));


  //============================================================================
  // Constructors / Destructor

  public: Engine()
  {
  }

  public: Engine(Data::GrammarRepository *grammarRepo, Data::SharedRepository *definitionsRepo)
  {
    this->initialize(grammarRepo, definitionsRepo);
  }

  public: virtual ~Engine()
  {
  }


  //============================================================================
  // Member Functions

  public: void initialize(Data::GrammarRepository *grammarRepo, Data::SharedRepository *definitionsRepo);

  /// Parse the given string and return any resulting parsing data.
  public: SharedPtr<TiObject> processString(Char const *str, Char const *name);

  /// Parse the given file and return any resulting parsing data.
  public: SharedPtr<TiObject> processFile(Char const *filename);

}; // class

} } // namespace

#endif

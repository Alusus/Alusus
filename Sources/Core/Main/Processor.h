/**
 * @file Core/Main/Processor.h
 * Contains the header of class Main::Processor.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef MAIN_PROCESSOR_H
#define MAIN_PROCESSOR_H

namespace Core { namespace Main
{

// TODO: DOC

class Processor : public SignalReceiver
{
  //============================================================================
  // Type Info

  TYPE_INFO(Processor, SignalReceiver, "Core.Main", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Lexer::StateMachine lexer;

  private: Parser::StateMachine parser;


  //============================================================================
  // Signals

  /// Emitted when a build msg (error or warning) is generated.
  public: RESIGNAL(buildMsgNotifier, (const SharedPtr<Common::BuildMsg> &msg), (msg));


  //============================================================================
  // Constructors / Destructor

  public: Processor()
  {
  }

  public: Processor(const SharedPtr<Lexer::CharGroupDefinitionList> &charGroupDefs,
                    const SharedPtr<Lexer::TokenDefinitionList> &tokenDefs,
                    Data::Manager *grammarManager)
  {
    this->initialize(charGroupDefs, tokenDefs, grammarManager);
  }

  public: virtual ~Processor()
  {
  }


  //============================================================================
  // Member Functions

  public: void initialize(const SharedPtr<Lexer::CharGroupDefinitionList> &charGroupDefs,
                          const SharedPtr<Lexer::TokenDefinitionList> &tokenDefs,
                          Data::Manager *grammarManager);

  /// Parse the given string and return any resulting parsing data.
  public: const SharedPtr<IdentifiableObject> processString(const Char *str);

  /// Parse the given file and return any resulting parsing data.
  public: const SharedPtr<IdentifiableObject> processFile(const Char *filename);

}; // class

} } // namespace

#endif

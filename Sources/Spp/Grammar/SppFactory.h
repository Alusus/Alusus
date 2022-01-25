/**
 * @file Spp/SppFactory.h
 * Contains the header of class Spp::SppFactory.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_GRAMMAR_SPPFACTORY_H
#define SPP_GRAMMAR_SPPFACTORY_H

namespace Spp::Grammar
{

class SppFactory : public Core::Data::Grammar::Factory
{
  //============================================================================
  // Member Variables

  private: Core::Main::RootManager *rootManager;

  private: Array<Str> customCommandQualifiers;
  private: Array<Str> customCommandKeywords;
  private: Array<Str> customGrammarQualifiers;


  //============================================================================
  // Constructor & Destructor

  public: SppFactory()
  {
  }

  public: virtual ~SppFactory()
  {
  }


  //============================================================================
  // Member Functions

  public: void setRootManager(Core::Main::RootManager *root)
  {
    this->rootManager = root;
    Core::Data::Ast::Scope *rootScope = root->getRootScope().get();
    this->setRootScope(rootScope);
  }

  /// Create the entire core grammar.
  public: void createGrammar();

  public: void cleanGrammar();

  public: Bool createCustomCommand(
    Char const *qualifier, TiObject *ast, ParsingHandlerFunc func, Core::Notices::Store *noticeStore
  );

  public: Bool createCustomGrammar(
    Char const *qualifier, Char const *baseQualifier, TiObject *overridesAst, Core::Notices::Store *noticeStore
  );

  private: void cleanCustomGrammarAndCommands();

}; // class

} // namespace

#endif

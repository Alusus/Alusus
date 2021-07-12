/**
 * @file Spp/GrammarFactory.h
 * Contains the header of class Spp::GrammarFactory.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_GRAMMARFACTORY_H
#define SPP_GRAMMARFACTORY_H

namespace Spp
{

class GrammarFactory : public Core::Data::Grammar::Factory
{
  //============================================================================
  // Member Variables

  private: Core::Main::RootManager *rootManager;

  private: Array<Str> customCommandQualifiers;
  private: Array<Str> customCommandKeywords;


  //============================================================================
  // Constructor & Destructor

  public: GrammarFactory()
  {
  }

  public: virtual ~GrammarFactory()
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

  private: Bool parseCommandSection(
    TiObject *ast, Factory::CommandSection &section, Core::Notices::Store *noticeStore
  );

  private: Bool parseCommandKeywords(
    TiObject *ast, SharedPtr<Core::Data::Grammar::Map> &keywords, Core::Notices::Store *noticeStore
  );

  private: Bool parseCommandArg(
    TiObject *ast, Factory::CommandArg &arg, Core::Notices::Store *noticeStore
  );

  private: Bool parseMinMax(
    TiObject *ast, TiObject *&resultAst, SharedPtr<TiInt> &min, SharedPtr<TiInt> &max, Core::Notices::Store *noticeStore
  );

  private: Bool parseQualifier(
    TiObject *ast, Str &qualifier, Core::Notices::Store *noticeStore
  );

  private: void convertInfixOpIntoList(TiObject *ast, Char const *op, Array<TiObject*> &list);

  private: void cleanCustomCommands();

}; // class

} // namespace

#endif

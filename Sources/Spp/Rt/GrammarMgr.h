/**
 * @file Spp/Rt/GrammarMgr.h
 * Contains the header of class Spp::Rt::GrammarMgr.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_RT_GRAMMARMGR_H
#define SPP_RT_GRAMMARMGR_H

namespace Spp::Rt
{

class GrammarMgr : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(GrammarMgr, TiObject, "Spp.Rt", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: Core::Main::RootManager *rootManager;
  private: Spp::Grammar::SppFactory *grammarFactory;


  //============================================================================
  // Constructor & Destructor

  public: GrammarMgr(Core::Main::RootManager *rm, Spp::Grammar::SppFactory *gf) : rootManager(rm), grammarFactory(gf)
  {
  }

  public: GrammarMgr(GrammarMgr *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->rootManager = parent->getRootManager();
    this->grammarFactory = parent->getGrammarFactory();
  }

  public: virtual ~GrammarMgr()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: static void initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, GrammarMgr *grammarMgr);

  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: Spp::Grammar::SppFactory* getGrammarFactory() const
  {
    return this->grammarFactory;
  }

  /// @}

  /// @name Operations
  /// @{

  public: METHOD_BINDING_CACHE(addCustomCommand,
    void, (Char const* /* qualifier */, TiObject* /* ast */, ParsingHandlerFunc /* func */)
  );
  public: static void _addCustomCommand(
    TiObject *self, Char const *qualifier, TiObject *ast, ParsingHandlerFunc func
  );

  public: METHOD_BINDING_CACHE(addCustomGrammar,
    Bool, (Char const* /* qualifier */, Char const* /* baseQualifier */, TiObject* /* ast */)
  );
  public: static Bool _addCustomGrammar(
    TiObject *self, Char const *qualifier, Char const *baseQualifier, TiObject *ast
  );

  /// @}

}; // class

} // namespace

#endif

/**
 * @file Spp/Rt/GrammarMgr.h
 * Contains the header of class Spp::Rt::GrammarMgr.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
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

  private: GrammarFactory *grammarFactory;
  private: Core::Notices::Store *noticeStore;
  private: Core::Processing::Parser *parser;


  //============================================================================
  // Constructor & Destructor

  public: GrammarMgr(GrammarFactory *gf) : grammarFactory(gf)
  {
  }

  public: GrammarMgr(GrammarMgr *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
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

  public: GrammarFactory* getGrammarFactory() const
  {
    return this->grammarFactory;
  }

  public: void setNoticeStore(Core::Notices::Store *store)
  {
    this->noticeStore = store;
  }
  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->noticeStore;
  }

  public: void setParser(Core::Processing::Parser *p)
  {
    this->parser = p;
  }
  public: Core::Processing::Parser* getParser() const
  {
    return this->parser;
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

  /// @}

}; // class

} // namespace

#endif

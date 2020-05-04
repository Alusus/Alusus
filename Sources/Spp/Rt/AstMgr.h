/**
 * @file Spp/Rt/AstMgr.h
 * Contains the header of class Spp::Rt::AstMgr.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_RT_ASTMGR_H
#define SPP_RT_ASTMGR_H

namespace Spp::Rt
{

class AstMgr : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(AstMgr, TiObject, "Spp.Rt", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: Core::Notices::Store *noticeStore;
  private: Core::Processing::Parser *parser;


  //============================================================================
  // Constructor & Destructor

  public: AstMgr()
  {
  }

  public: AstMgr(AstMgr *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
  }

  public: virtual ~AstMgr()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: static void initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, AstMgr *astMgr);

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

  // TODO: Return Array[String] instead of C style array.
  public: METHOD_BINDING_CACHE(getModifierStrings,
    Bool, (TiObject*, Char const*, Char const***, Word*)
  );
  public: static Bool _getModifierStrings(
    TiObject *self, TiObject *element, Char const *modifierKwd, Char const **resultStrs[], Word *resultCount
  );

  /// @}

}; // class

} // namespace

#endif

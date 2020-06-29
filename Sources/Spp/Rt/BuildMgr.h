/**
 * @file Spp/Rt/BuildMgr.h
 * Contains the header of class Spp::Rt::BuildMgr.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_RT_BUILDMGR_H
#define SPP_RT_BUILDMGR_H

namespace Spp::Rt
{

class BuildMgr : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildMgr, TiObject, "Spp.Rt", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Member Variables

  private: BuildManager *buildManager;
  private: Core::Notices::Store *noticeStore;
  private: Core::Processing::Parser *parser;


  //============================================================================
  // Constructor & Destructor

  public: BuildMgr(BuildManager *bm) : buildManager(bm)
  {
  }

  public: BuildMgr(BuildMgr *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->buildManager = parent->getBuildManager();
  }

  public: virtual ~BuildMgr()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  private: void initBindingCaches();
  private: void initBindings();

  public: static void initializeRuntimePointers(CodeGen::GlobalItemRepo *globalItemRepo, BuildMgr *buildMgr);

  public: BuildManager* getBuildManager() const
  {
    return this->buildManager;
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

  public: METHOD_BINDING_CACHE(dumpLlvmIrForElement, void, (TiObject*));
  public: static void _dumpLlvmIrForElement(TiObject *self, TiObject *element);

  public: METHOD_BINDING_CACHE(buildObjectFileForElement, Bool, (TiObject*, Char const*));
  public: static Bool _buildObjectFileForElement(TiObject *self, TiObject *element, Char const *objectFilename);

  /// @}

}; // class

} // namespace

#endif

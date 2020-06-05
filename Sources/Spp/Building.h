/**
 * @file Spp/Building.h
 * Contains the header of class Spp::Building.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_BUILDING_H
#define SPP_BUILDING_H

namespace Spp
{

class Building : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(Building, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: Building(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->prepareExecution,
      &this->prepareBuild,
      &this->addElementToBuild,
      &this->finalizeBuild,
      &this->execute,
      &this->deleteTempFunctions
    });
  }


  //============================================================================
  // Member Functions

  /// @name ObjTiInterface Implementation
  /// @{

  public: virtual TiObject* getTiObject()
  {
    return this->owner;
  }

  public: virtual TiObject const* getTiObject() const
  {
    return this->owner;
  }

  /// @}

  /// @name Building Functions
  /// @{

  public: METHOD_BINDING_CACHE(prepareExecution,
    void, (
      Core::Notices::Store* /* noticeStore */, TiObject* /* globalFuncElement */, BuildSession& /* buildSession */
    )
  );

  public: METHOD_BINDING_CACHE(prepareBuild,
    void, (
      Core::Notices::Store* /* noticeStore */, TiObject* /* globalFuncElement */,
      Bool /* offlineExecution */, BuildSession& /* buildSession */
    )
  );

  public: METHOD_BINDING_CACHE(addElementToBuild, Bool, (TiObject* /* element */, BuildSession& /* buildSession */));

  public: METHOD_BINDING_CACHE(finalizeBuild,
    void, (Core::Notices::Store* /* noticeStore */, TiObject* /* globalFuncElement */, BuildSession& /* buildSession */)
  );

  public: METHOD_BINDING_CACHE(execute,
    Bool, (Core::Notices::Store* /* noticeStore */, BuildSession& /* buildSession */)
  );

  public: METHOD_BINDING_CACHE(deleteTempFunctions, void, (BuildSession& /* buildSession */));

  /// @}

}; // class

} // namespace

#endif

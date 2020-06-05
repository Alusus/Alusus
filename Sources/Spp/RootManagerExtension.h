/**
 * @file Spp/RootManagerExtension.h
 * Contains the header of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_ROOTMANAGEREXTENSION_H
#define SPP_ROOTMANAGEREXTENSION_H

namespace Spp
{

class RootManagerExtension : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(RootManagerExtension, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Types

  public: struct Overrides
  {
    TiFunctionBase *importFileRef;
  };


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: RootManagerExtension(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->importFile,
      &this->jitBuildManager,
      &this->rtAstMgr,
      &this->rtBuildMgr
    });
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(jitBuildManager, BuildManager);
  public: BINDING_CACHE(rtAstMgr, Rt::AstMgr);
  public: BINDING_CACHE(rtBuildMgr, Rt::BuildMgr);


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

  /// @name Setup Functions
  /// @{

  public: static Overrides* extend(
    Core::Main::RootManager *rootManager,
    SharedPtr<BuildManager> const &jitBuildManager,
    SharedPtr<Rt::AstMgr> const &astM,
    SharedPtr<Rt::BuildMgr> const &buildM
  );
  public: static void unextend(Core::Main::RootManager *rootManager, Overrides *overrides);

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(importFile, void, (Char const*));
  public: static void _importFile(TiObject *self, Char const *filename);

  /// @}

}; // class

} // namespace

#endif

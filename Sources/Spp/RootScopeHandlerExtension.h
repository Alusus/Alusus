/**
 * @file Spp/RootScopeHandlerExtension.h
 * Contains the header of class Spp::RootScopeHandlerExtension.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_ROOTSCOPEHANDLEREXTENSION_H
#define SPP_ROOTSCOPEHANDLEREXTENSION_H

namespace Spp
{

class RootScopeHandlerExtension : public ObjTiInterface
{
  //============================================================================
  // Type Info

  OBJ_INTERFACE_INFO(RootScopeHandlerExtension, ObjTiInterface, "Spp", "Spp", "alusus.org");


  //============================================================================
  // Types

  public: struct Overrides
  {
    TiFunctionBase *addNewElementRef;
  };


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: RootScopeHandlerExtension(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->rootManagerBox,
    });
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(rootManagerBox, Box<Core::Main::RootManager*>);


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
    Core::Main::RootScopeHandler *handler, Core::Main::RootManager *rootManager
  );
  public: static void unextend(Core::Main::RootScopeHandler *handler, Overrides *overrides);

  /// @}

  /// @name Main Functions
  /// @{

  private: static void _addNewElement(
    TiFunctionBase *base, TiObject *self, TioSharedPtr const &data,
    Core::Processing::Parser *parser, Core::Processing::ParserState *state
  );

  /// @}

}; // class

} // namespace

#endif

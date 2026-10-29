/**
 * @file Spp/RootScopeHandlerExtension.h
 * Contains the header of class Spp::RootScopeHandlerExtension.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: RootScopeHandlerExtension(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->rootManagerBox
    });
    this->addNewElement.init(this->owner, &RootScopeHandlerExtension::_addNewElement);
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(rootManagerBox, TiBox<Core::Main::RootManager*>);


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

  public: static void extend(Core::Main::RootScopeHandler *handler, Core::Main::RootManager *rootManager);
  public: static void unextend(Core::Main::RootScopeHandler *handler);

  /// @}

  /// @name Main Functions
  /// @{

  // Overrides the RootScopeHandler's binding. The override gets removed automatically when this extension is
  // destroyed.
  public: METHOD_BINDING_CACHE(addNewElement,
    void, (SharedPtr<Core::Ast::Node> const&, Core::Processing::Parser*, Core::Processing::ParserState*)
  );
  private: static void _addNewElement(
    TiObject *self, SharedPtr<Core::Ast::Node> const &data,
    Core::Processing::Parser *parser, Core::Processing::ParserState *state
  );

  /// @}

}; // class

} // namespace

#endif

/**
 * @file Spp/RootScopeHandlerExtension.h
 * Contains the header of class Spp::RootScopeHandlerExtension.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  OBJ_INTERFACE_INFO(RootScopeHandlerExtension, ObjTiInterface, "Spp", "Spp", "alusus.net");


  //============================================================================
  // Types

  public: struct Overrides
  {
    TiFunctionBase *addNewElementRef;
    TiFunctionBase *executeElementRef;
  };


  //============================================================================
  // Member Variables

  private: TiObject *owner;


  //============================================================================
  // Constructor

  public: RootScopeHandlerExtension(TiObject *o) : owner(o)
  {
    Basic::initBindingCaches(this->owner, {
      &this->executeElement,
      &this->rootStatementIndex,
      &this->rootManagerBox,
      &this->astHelper,
      &this->macroProcessor,
      &this->generator,
      &this->targetGenerator
    });
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(rootStatementIndex, TiInt);
  public: BINDING_CACHE(rootManagerBox, Box<Core::Main::RootManager*>);
  public: BINDING_CACHE(astHelper, Ast::Helper);
  public: BINDING_CACHE(macroProcessor, CodeGen::MacroProcessor);
  public: BINDING_CACHE(generator, CodeGen::Generator);
  public: BINDING_CACHE(targetGenerator, LlvmCodeGen::TargetGenerator);


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
    Core::Main::RootScopeHandler *handler, Core::Main::RootManager *rootManager,
    SharedPtr<Ast::Helper> const &astHelper, SharedPtr<CodeGen::MacroProcessor> const &macroProcessor,
    SharedPtr<CodeGen::Generator> const &generator, SharedPtr<LlvmCodeGen::TargetGenerator> const &targetGenerator
  );
  public: static void unextend(Core::Main::RootScopeHandler *handler, Overrides *overrides);

  /// @}

  /// @name Main Functions
  /// @{

  private: static void _addNewElement(
    TiFunctionBase *base, TiObject *self, TioSharedPtr const &data, Core::Processing::ParserState *state
  );

  public: METHOD_BINDING_CACHE(executeElement,
    void, (TioSharedPtr const&, Core::Processing::ParserState*)
  );
  private: static void _executeElement(
    TiObject *self, TioSharedPtr const &element, Core::Processing::ParserState *state
  );

  /// @}

}; // class

} // namespace

#endif

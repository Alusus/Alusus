/**
 * @file Spp/RootManagerExtension.h
 * Contains the header of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
    TiFunctionBase *executeRootElementRef;
    TiFunctionBase *dumpLlvmIrForElementRef;
    TiFunctionBase *buildObjectFileForElementRef;
    TiFunctionBase *resetBuildDataRef;
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
      &this->executeRootElement,
      &this->dumpLlvmIrForElement,
      &this->buildObjectFileForElement,
      &this->resetBuildData,
      &this->importFile,
      &this->astHelper,
      &this->macroProcessor,
      &this->generator,
      &this->targetGenerator,
      &this->rootStmtTgFunc
    });
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(astHelper, Ast::Helper);
  public: BINDING_CACHE(macroProcessor, CodeGen::MacroProcessor);
  public: BINDING_CACHE(generator, CodeGen::Generator);
  public: BINDING_CACHE(targetGenerator, LlvmCodeGen::TargetGenerator);
  public: BINDING_CACHE(rootStmtTgFunc, TiObject);


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
    SharedPtr<Ast::Helper> const &astHelper, SharedPtr<CodeGen::MacroProcessor> const &macroProcessor,
    SharedPtr<CodeGen::Generator> const &generator, SharedPtr<LlvmCodeGen::TargetGenerator> const &targetGenerator
  );
  public: static void unextend(Core::Main::RootManager *rootManager, Overrides *overrides);

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(executeRootElement, void, (TioSharedPtr const&, Core::Notices::Store*));
  private: static void _executeRootElement(
    TiObject *self, TioSharedPtr const &element, Core::Notices::Store *noticeStore
  );

  public: METHOD_BINDING_CACHE(dumpLlvmIrForElement,
    void, (TiObject*, Core::Notices::Store*, Core::Processing::Parser*)
  );
  public: static void _dumpLlvmIrForElement(
    TiObject *self, TiObject *element, Core::Notices::Store *noticeStore, Core::Processing::Parser *parser
  );

  public: METHOD_BINDING_CACHE(buildObjectFileForElement,
    void, (TiObject*, Char const*, Core::Notices::Store*, Core::Processing::Parser*)
  );
  public: static Bool _buildObjectFileForElement(
    TiObject *self, TiObject *element, Char const *objectFilename, Core::Notices::Store *noticeStore,
    Core::Processing::Parser *parser
  );

  public: METHOD_BINDING_CACHE(resetBuildData, void, (TiObject*));
  private: static void _resetBuildData(TiObject *self, TiObject *obj);

  public: METHOD_BINDING_CACHE(importFile, void, (Char const*));
  public: static void _importFile(TiObject *self, Char const *filename);

  /// @}

}; // class

} // namespace

#endif

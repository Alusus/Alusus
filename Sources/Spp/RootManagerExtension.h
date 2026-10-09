/**
 * @file Spp/RootManagerExtension.h
 * Contains the header of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
    TiFunctionBase *prefixAlususTemplateClassFuncExpNamesRef;
    TiFunctionBase *insertClassPaddingRef;
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
      &this->prefixAlususTemplateClassFuncExpNames,
      &this->insertClassPadding,
      &this->buildManager,
      &this->astProcessor,
      &this->rtGrammarMgr,
      &this->rtAstMgr,
      &this->rtBuildMgr
    });
  }


  //============================================================================
  // Member Properties

  public: BINDING_CACHE(buildManager, BuildManager);
  public: BINDING_CACHE(astProcessor, CodeGen::AstProcessor);
  public: BINDING_CACHE(rtGrammarMgr, Rt::GrammarMgr);
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
    SharedPtr<BuildManager> const &buildManager,
    SharedPtr<CodeGen::AstProcessor> const &astProcessor,
    SharedPtr<Rt::GrammarMgr> const &grammarM,
    SharedPtr<Rt::AstMgr> const &astM,
    SharedPtr<Rt::BuildMgr> const &buildM
  );
  public: static void unextend(Core::Main::RootManager *rootManager, Overrides *overrides);

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(importFile, void, (Char const*));
  public: static void _importFile(TiObject *self, Char const *filename);

  public: METHOD_BINDING_CACHE(prefixAlususTemplateClassFuncExpNames,
    void, (Core::Ast::Node* /* classAst */, Core::Ast::Node* /* argAst */)
  );
  public: static void _prefixAlususTemplateClassFuncExpNames(
    TiObject *self, Core::Ast::Node *classAst, Core::Ast::Node *argAst
  );

  /**
   * @brief Insert a padding array at the current preprocess position of a class.
   *
   * Classes defined on Alusus side to mirror C++ classes don't always define every member that exists on the C++
   * side. This function inserts a byte array member that fills the difference between the given size, which is the
   * size of the members defined on Alusus side, and the size of the C++ class.
   */
  public: METHOD_BINDING_CACHE(insertClassPadding, void, (ArchInt /* definedSize */, Char const* /* classUniqueName */));
  public: static void _insertClassPadding(TiObject *self, ArchInt definedSize, Char const *classUniqueName);

  /// @}

}; // class

} // namespace

#endif

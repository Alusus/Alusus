/**
 * @file Spp/Rt/AstMgr.h
 * Contains the header of class Spp::Rt::AstMgr.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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

  private: Ast::Helper *astHelper;
  private: Core::Main::RootManager *rootManager;
  private: Spp::CodeGen::AstProcessor *astProcessor;


  //============================================================================
  // Constructor & Destructor

  public: AstMgr()
  {
    this->initBindingCaches();
    this->initBindings();
  }

  public: AstMgr(AstMgr *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->setAstHelper(parent->getAstHelper());
    this->setRootManager(parent->getRootManager());
    this->setAstProcessor(parent->getAstProcessor());
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

  public: void setAstHelper(Ast::Helper *h)
  {
    this->astHelper = h;
  }
  public: Ast::Helper* getAstHelper() const
  {
    return this->astHelper;
  }

  public: void setRootManager(Core::Main::RootManager *rm)
  {
    this->rootManager = rm;
  }
  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: void setAstProcessor(Spp::CodeGen::AstProcessor *astP)
  {
    this->astProcessor = astP;
  }
  public: Spp::CodeGen::AstProcessor* getAstProcessor() const
  {
    return this->astProcessor;
  }

  /// @}

  /// @name Operations
  /// @{

  public: METHOD_BINDING_CACHE(findElements,
    Array<TiObject*>, (TiObject* /* ref */, TiObject* /* target */, Word /* flags */)
  );
  public: static Array<TiObject*> _findElements(TiObject *self, TiObject *ref, TiObject *target, Word flags);

  public: METHOD_BINDING_CACHE(getModifiers, Containing<TiObject>*, (TiObject* /* element */));
  public: static Containing<TiObject>* _getModifiers(TiObject *self, TiObject *element);

  public: METHOD_BINDING_CACHE(findModifier, TiObject*, (Containing<TiObject>* /* modifiers */, Char const* /* kwd */));
  public: static TiObject* _findModifier(TiObject *self, Containing<TiObject> *modifiers, Char const *kwd);

  public: METHOD_BINDING_CACHE(findModifierForElement, TiObject*, (TiObject* /* element */, Char const* /* kwd */));
  public: static TiObject* _findModifierForElement(TiObject *self, TiObject *element, Char const *kwd);

  public: METHOD_BINDING_CACHE(getModifierKeyword, String, (TiObject* /* modifier */));
  public: static String _getModifierKeyword(TiObject *self, TiObject *modifier);

  public: METHOD_BINDING_CACHE(getModifierStringParams, Bool, (TiObject* /* modifier */, Array<String>& /* result */));
  public: static Bool _getModifierStringParams(TiObject *self, TiObject *modifier, Array<String> &result);

  public: METHOD_BINDING_CACHE(getSourceFullPathForElement, String, (TiObject* /* element */));
  public: static String _getSourceFullPathForElement(TiObject *self, TiObject *element);

  public: METHOD_BINDING_CACHE(insertAst, Bool, (TiObject*));
  public: static Bool _insertAst(TiObject *self, TiObject* ast);

  public: METHOD_BINDING_CACHE(insertAst_plain, Bool, (TiObject*, Map<Str, TiObject*>*));
  public: static Bool _insertAst_plain(TiObject *self, TiObject* ast, Map<Str, TiObject*> *interpolations);

  public: METHOD_BINDING_CACHE(insertAst_shared, Bool, (TiObject*, Map<Str, SharedPtr<TiObject>>*));
  public: static Bool _insertAst_shared(TiObject *self, TiObject* ast, Map<Str, SharedPtr<TiObject>> *interpolations);

  public: METHOD_BINDING_CACHE(buildAst_plain, Bool, (TiObject*, Map<Str, TiObject*>*, TioSharedPtr&));
  public: static Bool _buildAst_plain(
    TiObject *self, TiObject* ast, Map<Str, TiObject*> *interpolations, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(buildAst_shared, Bool, (TiObject*, Map<Str, SharedPtr<TiObject>>*, TioSharedPtr&));
  public: static Bool _buildAst_shared(
    TiObject *self, TiObject* ast, Map<Str, SharedPtr<TiObject>> *interpolations, TioSharedPtr &result
  );

  public: METHOD_BINDING_CACHE(getCurrentPreprocessOwner, TiObject*);
  private: static TiObject* _getCurrentPreprocessOwner(TiObject *self);

  public: METHOD_BINDING_CACHE(getCurrentPreprocessInsertionPosition, Int);
  private: static Int _getCurrentPreprocessInsertionPosition(TiObject *self);

  public: METHOD_BINDING_CACHE(getVariableDomain, Int, (TiObject*));
  public: static Int _getVariableDomain(TiObject *self, TiObject* ast);

  /// @}

}; // class

} // namespace

#endif

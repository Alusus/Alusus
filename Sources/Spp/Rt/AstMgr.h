/**
 * @file Spp/Rt/AstMgr.h
 * Contains the header of class Spp::Rt::AstMgr.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

  private: Ast::Helper *astHelper = 0;
  private: ExpressionComputation *expressionComputation = 0;
  private: Core::Main::RootManager *rootManager = 0;
  private: Spp::CodeGen::AstProcessor *astProcessor = 0;


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
    this->setExpressionComputation(parent->getExpressionComputation());
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

  public: void setExpressionComputation(ExpressionComputation *ec)
  {
    this->expressionComputation = ec;
  }
  public: ExpressionComputation* getExpressionComputation() const
  {
    return this->expressionComputation;
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
    Array<Core::Ast::Node*>,
    (Core::Ast::Node* /* ref */, Core::Ast::Node* /* target */, Word /* flags */)
  );
  private: static Array<Core::Ast::Node*> _findElements(
    TiObject *self, Core::Ast::Node *ref, Core::Ast::Node *target, Word flags
  );

  public: METHOD_BINDING_CACHE(getModifiers, Containing<Core::Ast::Node>*, (Core::Ast::Node* /* element */));
  private: static Containing<Core::Ast::Node>* _getModifiers(TiObject *self, Core::Ast::Node *element);

  public: METHOD_BINDING_CACHE(findModifier, Core::Ast::Node*, (Containing<Core::Ast::Node>* /* modifiers */, Char const* /* kwd */));
  private: static Core::Ast::Node* _findModifier(TiObject *self, Containing<Core::Ast::Node> *modifiers, Char const *kwd);

  public: METHOD_BINDING_CACHE(findModifierForElement, Core::Ast::Node*, (Core::Ast::Node* /* element */, Char const* /* kwd */));
  private: static Core::Ast::Node* _findModifierForElement(TiObject *self, Core::Ast::Node *element, Char const *kwd);

  public: METHOD_BINDING_CACHE(getModifierKeyword, String, (Core::Ast::Node* /* modifier */));
  private: static String _getModifierKeyword(TiObject *self, Core::Ast::Node *modifier);

  public: METHOD_BINDING_CACHE(getModifierParams, Bool, (Core::Ast::Node* /* modifier */, Array<Core::Ast::Node*>& /* result */));
  private: static Bool _getModifierParams(TiObject *self, Core::Ast::Node *modifier, Array<Core::Ast::Node*> &result);

  public: METHOD_BINDING_CACHE(getModifierStringParams, Bool, (Core::Ast::Node* /* modifier */, Array<String>& /* result */));
  private: static Bool _getModifierStringParams(TiObject *self, Core::Ast::Node *modifier, Array<String> &result);

  public: METHOD_BINDING_CACHE(getStringsFromStringParams, Bool, (Core::Ast::Node* /* params */, Array<String>& /* result */));
  private: static Bool _getStringsFromStringParams(TiObject *self, Core::Ast::Node *params, Array<String> &result);

  public: METHOD_BINDING_CACHE(getSourceFullPathForElement, String, (Core::Ast::Node* /* element */));
  private: static String _getSourceFullPathForElement(TiObject *self, Core::Ast::Node *element);

  public: METHOD_BINDING_CACHE(addPossiblyMergeableElement,
    Bool, (Core::Ast::Node* /* src */, DynamicContaining<Core::Ast::Node>* /* target */, Int& /* index */)
  );
  private: static Bool _addPossiblyMergeableElement(
    TiObject *self, Core::Ast::Node *src, DynamicContaining<Core::Ast::Node> *target, Int &index
  );

  public: METHOD_BINDING_CACHE(insertAst, Bool, (Core::Ast::Node*));
  private: static Bool _insertAst(TiObject *self, Core::Ast::Node* ast);

  public: METHOD_BINDING_CACHE(insertAst_plain, Bool, (Core::Ast::Node*, Map<Str, Core::Ast::Node*>*));
  private: static Bool _insertAst_plain(
    TiObject *self, Core::Ast::Node* ast, Map<Str, Core::Ast::Node*> *interpolations
  );

  public: METHOD_BINDING_CACHE(insertAst_shared,
    Bool, (Core::Ast::Node*, Map<Str, SharedPtr<Core::Ast::Node>>*)
  );
  private: static Bool _insertAst_shared(
    TiObject *self, Core::Ast::Node* ast, Map<Str, SharedPtr<Core::Ast::Node>> *interpolations
  );

  public: METHOD_BINDING_CACHE(buildAst_plain,
    Bool, (Core::Ast::Node*, Map<Str, Core::Ast::Node*>*, SharedPtr<Core::Ast::Node>&)
  );
  private: static Bool _buildAst_plain(
    TiObject *self, Core::Ast::Node* ast, Map<Str, Core::Ast::Node*> *interpolations,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(buildAst_shared,
    Bool, (
      Core::Ast::Node*, Map<Str, SharedPtr<Core::Ast::Node>>*, SharedPtr<Core::Ast::Node>&
    )
  );
  private: static Bool _buildAst_shared(
    TiObject *self, Core::Ast::Node* ast, Map<Str, SharedPtr<Core::Ast::Node>> *interpolations,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(getCurrentPreprocessOwner, Core::Ast::Node*);
  private: static Core::Ast::Node* _getCurrentPreprocessOwner(TiObject *self);

  public: METHOD_BINDING_CACHE(preprocessTypeBody, Bool, (Spp::Ast::UserType*));
  private: static Bool _preprocessTypeBody(TiObject *self, Spp::Ast::UserType *type);

  public: METHOD_BINDING_CACHE(getCurrentPreprocessInsertionPosition, Int);
  private: static Int _getCurrentPreprocessInsertionPosition(TiObject *self);

  public: METHOD_BINDING_CACHE(getVariableDomain, Int, (Core::Ast::Node*));
  private: static Int _getVariableDomain(TiObject *self, Core::Ast::Node* ast);

  public: METHOD_BINDING_CACHE(traceType, Spp::Ast::Type*, (Core::Ast::Node*));
  private: static Spp::Ast::Type* _traceType(TiObject *self, Core::Ast::Node *astNode);

  public: METHOD_BINDING_CACHE(isCastableTo, Bool, (Core::Ast::Node*, Core::Ast::Node*, Bool));
  private: static Bool _isCastableTo(TiObject *self, Core::Ast::Node *srcTypeRef, Core::Ast::Node *targetTypeRef, Bool implicit);

  public: METHOD_BINDING_CACHE(matchTemplateInstance,
    Bool, (Spp::Ast::Template*, Core::Ast::Node*, SharedPtr<Core::Ast::Node>&)
  );
  private: static Bool _matchTemplateInstance(
    TiObject *self, Spp::Ast::Template *tmplt, Core::Ast::Node *templateInputs,
    SharedPtr<Core::Ast::Node> &result
  );

  public: METHOD_BINDING_CACHE(computeResultType,
    Bool, (Core::Ast::Node* /* astNode */, Core::Ast::Node*& /* result */, Bool& /* resultIsValue */)
  );
  private: static Bool _computeResultType(
    TiObject *self, Core::Ast::Node *astNode, Core::Ast::Node *&result, Bool &resultIsValue
  );

  public: METHOD_BINDING_CACHE(cloneAst,
    SharedPtr<Core::Ast::Node>, (Core::Ast::Node*, Core::Ast::Node*)
  );
  private: static SharedPtr<Core::Ast::Node> _cloneAst(
    TiObject *self, Core::Ast::Node *nodeToCopy, Core::Ast::Node *nodeForSourceLocation
  );

  public: METHOD_BINDING_CACHE(dumpAst, void, (Core::Ast::Node*));
  private: static void _dumpAst(TiObject *self, Core::Ast::Node *obj);

  public: METHOD_BINDING_CACHE(getReferenceTypeFor, Spp::Ast::ReferenceType*, (Core::Ast::Node*));
  private: static Spp::Ast::ReferenceType* _getReferenceTypeFor(TiObject *self, Core::Ast::Node *type);

  public: METHOD_BINDING_CACHE(tryGetDeepReferenceContentType, Spp::Ast::Type*, (Spp::Ast::Type*));
  private: static Spp::Ast::Type* _tryGetDeepReferenceContentType(TiObject *self, Spp::Ast::Type *type);

  public: METHOD_BINDING_CACHE(isInjection, Bool, (Core::Ast::Node*));
  private: static Bool _isInjection(TiObject *self, Core::Ast::Node *obj);

  /// @}

}; // class

} // namespace

#endif

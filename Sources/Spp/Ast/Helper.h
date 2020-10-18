/**
 * @file Spp/Ast/Helper.h
 * Contains the header of class Spp::Ast::Helper.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_HELPER_H
#define SPP_AST_HELPER_H

namespace Spp { namespace Ast
{

class Helper : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(Helper, TiObject, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Member Variables

  private: Core::Main::RootManager *rootManager;
  private: NodePathResolver *nodePathResolver;

  private: Core::Notices::Store *noticeStore = 0;
  private: Template *refTemplate = 0;
  private: Template *irefTemplate = 0;
  private: Template *ndrefTemplate = 0;
  private: Template *ptrTemplate = 0;
  private: Template *arrayTemplate = 0;
  private: IntegerType *nullType = 0;
  private: IntegerType *boolType = 0;
  private: IntegerType *charType = 0;
  private: IntegerType *int64Type = 0;
  private: IntegerType *word64Type = 0;
  private: VoidType *voidType = 0;
  private: SharedPtr<Core::Data::Ast::ParamPass> integerTypeRef;
  private: SharedPtr<Core::Data::Ast::ParamPass> wordTypeRef;
  private: SharedPtr<Core::Data::Ast::ParamPass> floatTypeRef;
  private: SharedPtr<Core::Data::Ast::ParamPass> charArrayTypeRef;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Constructor

  Helper(Core::Main::RootManager *rm, NodePathResolver *npr) : rootManager(rm), nodePathResolver(npr)
  {
    this->initBindingCaches();
    this->initBindings();
  }

  Helper(Helper *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
    this->rootManager = parent->getRootManager();
    this->nodePathResolver = parent->getNodePathResolver();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  public: void prepare(Core::Notices::Store *ns)
  {
    this->noticeStore = ns;
    this->refTemplate = 0;
  }

  /// @}

  /// @name Property Getters
  /// @{

  public: Core::Main::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: NodePathResolver* getNodePathResolver() const
  {
    return this->nodePathResolver;
  }

  public: Core::Data::Seeker* getSeeker() const
  {
    return this->rootManager->getSeeker();
  }

  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->noticeStore;
  }

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(isAstReference, Bool, (TiObject*));
  private: static Bool _isAstReference(TiObject *self, TiObject *obj);

  public: Bool lookupCalleeInScopeByName(
    Char const *name, SharedPtr<Core::Data::SourceLocation> const &sl, Core::Data::Node *astNode, Bool searchOwners,
    TiObject *thisType, Containing<TiObject> *types, ExecutionContext const *ec, CalleeLookupResult &result
  );

  public: METHOD_BINDING_CACHE(lookupCalleeInScope,
    Bool, (
      TiObject* /* ref */, Core::Data::Node* /* astNode */, Bool /* searchOwners */,
      TiObject* /* thisType */, Containing<TiObject>* /* types */, ExecutionContext const* /* ec */,
      CalleeLookupResult& /* result */
    )
  );
  private: static Bool _lookupCalleeInScope(
    TiObject *self, TiObject *ref, Core::Data::Node *astNode, Bool searchOwners,
    TiObject *thisType, Containing<TiObject> *types, ExecutionContext const *ec, CalleeLookupResult &result
  );

  public: METHOD_BINDING_CACHE(lookupCalleeOnObject,
    Bool, (
      TiObject*, TiObject*, Containing<TiObject>*, ExecutionContext const*, Word, CalleeLookupResult&
    )
  );
  private: static Bool _lookupCalleeOnObject(
    TiObject *self, TiObject *obj, TiObject *thisType, Containing<TiObject> *types, ExecutionContext const *ec,
    Word currentStackSize, CalleeLookupResult &result
  );

  public: METHOD_BINDING_CACHE(lookupCustomCaster,
    Function*, (
      Type* /* srcType */, Type* /* targetType */, ExecutionContext const* /* ec */
    )
  );
  private: static Function* _lookupCustomCaster(
    TiObject *self, Type *srcType, Type *targetType, ExecutionContext const *ec
  );

  public: METHOD_BINDING_CACHE(lookupReferenceTarget,
    Bool, (TiObject*, Core::Data::Ast::Identifier*, Bool, PlainList<TiObject>&)
  );
  private: static Bool _lookupReferenceTarget(
    TiObject *self, TiObject *astNode, Core::Data::Ast::Identifier *ref, Bool searchOwners, PlainList<TiObject> &stack
  );

  public: METHOD_BINDING_CACHE(traceType, Type*, (TiObject*));
  private: static Type* _traceType(TiObject *self, TiObject *ref);

  public: METHOD_BINDING_CACHE(isVoid, Bool, (TiObject const*));
  private: static Bool _isVoid(TiObject *self, TiObject const *ref);

  public: METHOD_BINDING_CACHE(isImplicitlyCastableTo, Bool, (TiObject*, TiObject*, ExecutionContext const*));
  private: static Bool _isImplicitlyCastableTo(
    TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef, ExecutionContext const *ec
  );

  public: METHOD_BINDING_CACHE(isExplicitlyCastableTo, Bool, (TiObject*, TiObject*, ExecutionContext const*));
  private: static Bool _isExplicitlyCastableTo(
    TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef, ExecutionContext const *ec
  );

  public: METHOD_BINDING_CACHE(matchTargetType,
    TypeMatchStatus, (
      TiObject* /* srcTypeRef */, TiObject* /* targetTypeRef */, ExecutionContext const* /* ec */,
      Function*& /* caster */
    )
  );
  private: static TypeMatchStatus _matchTargetType(
    TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef, ExecutionContext const *ec, Function *&caster
  );

  public: METHOD_BINDING_CACHE(isReferenceTypeFor, Bool, (Type*, Type*, ExecutionContext const*));
  private: static Bool _isReferenceTypeFor(
    TiObject *self, Type *refType, Type *contentType, ExecutionContext const *ec
  );

  public: METHOD_BINDING_CACHE(getReferenceTypeFor, ReferenceType*, (TiObject*, ReferenceMode const&));
  private: static ReferenceType* _getReferenceTypeFor(TiObject *self, TiObject *type, ReferenceMode const &mode);

  public: ReferenceType* getReferenceTypeForPointerType(PointerType *type, ReferenceMode const &mode);

  public: METHOD_BINDING_CACHE(getPointerTypeFor, PointerType*, (TiObject*));
  private: static PointerType* _getPointerTypeFor(TiObject *self, TiObject *type);

  public: METHOD_BINDING_CACHE(getArrayTypeFor, ArrayType*, (TiObject*));
  private: static ArrayType* _getArrayTypeFor(TiObject *self, TiObject *type);

  public: Type* swichInnerReferenceTypeWithPointerType(ReferenceType *type);

  public: Type* swichOuterPointerTypeWithReferenceType(Type *type, ReferenceMode const &mode);

  public: METHOD_BINDING_CACHE(getValueTypeFor, Type*, (TiObject*));
  private: static Type* _getValueTypeFor(TiObject *self, TiObject *type);

  public: METHOD_BINDING_CACHE(getNullType, IntegerType*);
  private: static IntegerType* _getNullType(TiObject *self);

  public: METHOD_BINDING_CACHE(getBoolType, IntegerType*);
  private: static IntegerType* _getBoolType(TiObject *self);

  public: METHOD_BINDING_CACHE(getCharType, IntegerType*);
  private: static IntegerType* _getCharType(TiObject *self);

  public: METHOD_BINDING_CACHE(getCharArrayType, ArrayType*, (Word));
  private: static ArrayType* _getCharArrayType(TiObject *self, Word size);

  public: METHOD_BINDING_CACHE(getInt64Type, IntegerType*);
  private: static IntegerType* _getInt64Type(TiObject *self);

  public: METHOD_BINDING_CACHE(getIntType, IntegerType*, (Word));
  private: static IntegerType* _getIntType(TiObject *self, Word size);

  public: METHOD_BINDING_CACHE(getWord64Type, IntegerType*);
  private: static IntegerType* _getWord64Type(TiObject *self);

  public: METHOD_BINDING_CACHE(getWordType, IntegerType*, (Word));
  private: static IntegerType* _getWordType(TiObject *self, Word size);

  public: METHOD_BINDING_CACHE(getFloatType, FloatType*, (Word));
  private: static FloatType* _getFloatType(TiObject *self, Word size);

  public: METHOD_BINDING_CACHE(getVoidType, VoidType*);
  private: static VoidType* _getVoidType(TiObject *self);

  public: template<class T> Bool isTypeOrRefTypeOf(TiObject *type)
  {
    if (!type->isDerivedFrom<Type>()) {
      return this->isTypeOrRefTypeOf<T>(this->traceType(type));
    }

    if (type->isDerivedFrom<T>()) return true;

    auto refType = ti_cast<ReferenceType>(type);
    if (refType == 0) return false;
    else return this->isTypeOrRefTypeOf<T>(refType->getContentType(this));
  }

  public: template<class T> T* tryGetPointerContentType(TiObject *type)
  {
    if (!type->isDerivedFrom<Type>()) {
      return this->tryGetPointerContentType<T>(this->traceType(type));
    }

    auto ptrType = ti_cast<PointerType>(type);
    if (ptrType == 0) return 0;
    else return ti_cast<T>(ptrType->getContentType(this));
  }

  public: Type* tryGetDeepReferenceContentType(Type *type)
  {
    auto refType = ti_cast<ReferenceType>(type);
    if (refType == 0 || !refType->isAutoDeref()) return type;
    else return this->tryGetDeepReferenceContentType(refType->getContentType(this));
  }

  public: METHOD_BINDING_CACHE(resolveNodePath, Str, (Core::Data::Node const*));
  private: static Str _resolveNodePath(TiObject *self, Core::Data::Node const *node);

  public: METHOD_BINDING_CACHE(getFunctionName, Str const&, (Function*));
  private: static Str const& _getFunctionName(TiObject *self, Function *astFunc);

  public: METHOD_BINDING_CACHE(getNeededIntSize, Word, (LongInt));
  private: static Word _getNeededIntSize(TiObject *self, LongInt value);

  public: METHOD_BINDING_CACHE(getNeededWordSize, Word, (LongWord));
  private: static Word _getNeededWordSize(TiObject *self, LongWord value);

  public: METHOD_BINDING_CACHE(getVariableDomain, DefinitionDomain, (TiObject const*));
  private: static DefinitionDomain _getVariableDomain(TiObject *self, TiObject const *def);

  public: METHOD_BINDING_CACHE(getFunctionDomain, DefinitionDomain, (TiObject const*));
  private: static DefinitionDomain _getFunctionDomain(TiObject *self, TiObject const *def);

  public: Bool doesModifierExistOnDef(Core::Data::Ast::Definition const *def, Char const *name);
  public: Bool isSharedDef(Core::Data::Ast::Definition const *def)
  {
    return this->doesModifierExistOnDef(def, S("shared"));
  }
  public: Bool isNoBindDef(Core::Data::Ast::Definition const *def)
  {
    return this->doesModifierExistOnDef(def, S("no_bind"));
  }

  public: METHOD_BINDING_CACHE(validateUseStatement, Bool, (Core::Data::Ast::Bridge* /* bridge */));
  private: static Bool _validateUseStatement(TiObject *self, Core::Data::Ast::Bridge *bridge);

  /// @}

  /// @name Helper Functions
  /// @{

  private: Template* getReferenceTemplate(ReferenceMode const &mode);

  private: Template* getPointerTemplate();

  private: Template* getArrayTemplate();

  /// @}

}; // class

} } // namespace

#endif

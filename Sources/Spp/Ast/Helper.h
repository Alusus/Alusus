/**
 * @file Spp/Ast/Helper.h
 * Contains the header of class Spp::Ast::Helper.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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
  private: ExpressionComputation *expressionComputation = 0;

  private: Template *refTemplate = 0;
  private: Template *trefTemplate = 0;
  private: Template *irefTemplate = 0;
  private: Template *ndrefTemplate = 0;
  private: Template *ptrTemplate = 0;
  /// (Source content type, target type) pairs for which a custom caster lookup is in progress, used to break cycles
  /// in caster chains.
  private: std::vector<Type*> activeCasterLookups;
  private: Template *lptrTemplate = 0;
  private: Template *arrayTemplate = 0;
  private: IntegerType *nullType = 0;
  private: IntegerType *boolType = 0;
  private: IntegerType *charType = 0;
  private: IntegerType *archIntType = 0;
  private: IntegerType *word64Type = 0;
  private: VoidType *voidType = 0;
  private: UserType *nodeType = 0;
  private: SharedPtr<Core::Ast::ParamPass> integerTypeRef;
  private: SharedPtr<Core::Ast::ParamPass> wordTypeRef;
  private: SharedPtr<Core::Ast::ParamPass> floatTypeRef;
  private: SharedPtr<Core::Ast::ParamPass> charArrayTypeRef;


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
    this->expressionComputation = parent->getExpressionComputation();
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  public: void prepare()
  {
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

  public: void setExpressionComputation(ExpressionComputation *ec)
  {
    this->expressionComputation = ec;
  }

  public: ExpressionComputation* getExpressionComputation() const
  {
    return this->expressionComputation;
  }

  public: Core::Ast::Seeker* getSeeker() const
  {
    return this->rootManager->getSeeker();
  }

  public: Core::Notices::Store* getNoticeStore() const
  {
    return this->rootManager->getNoticeStore();
  }

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(isAstReference, Bool, (Core::Ast::Node*));
  private: static Bool _isAstReference(TiObject *self, Core::Ast::Node *obj);

  public: METHOD_BINDING_CACHE(isVariable, Bool, (Core::Ast::Node*));
  private: static Bool _isVariable(TiObject *self, Core::Ast::Node *obj);

  public: METHOD_BINDING_CACHE(isInMemVariable, Bool, (Core::Ast::Node*));
  private: static Bool _isInMemVariable(TiObject *self, Core::Ast::Node *obj);

  public: METHOD_BINDING_CACHE(isValueOnlyVariable, Bool, (Core::Ast::Node*));
  private: static Bool _isValueOnlyVariable(TiObject *self, Core::Ast::Node *obj);

  public: METHOD_BINDING_CACHE(lookupCustomCaster,
    TypeMatchStatus, (
      Type* /* srcType */, Type* /* targetType */, Function *& /* caster */
    )
  );
  private: static TypeMatchStatus _lookupCustomCaster(
    TiObject *self, Type *srcType, Type *targetType, Function *&caster
  );

  public: METHOD_BINDING_CACHE(_traceType, Type*, (Core::Ast::Node*, Bool));
  private: static Type* __traceType(TiObject *self, Core::Ast::Node *ref, Bool skipErrors);

  public: Type* traceType(Core::Ast::Node *ref, Bool skipErrors = false)
  {
    return this->_traceType(ref, skipErrors);
  }

  private: Bool resolveVarProps(Variable *var, Bool skipErrors);
  private: Bool deduceVarType(Variable *var);

  private: Type* deduceIntLiteralType(Core::Ast::Node *astExpr);

  public: void parseIntLiteral(Char const *src, LongWord &value, Int &size, Bool &signedNum, Bool &typeRequested);

  /// Returns the args to pass to the constructor of the given var, if any. astTypeRef is set to the node from which
  /// the var's type should be traced.
  public: Core::Ast::Node* getVarInitArgs(Core::Ast::Node *astVar, Core::Ast::Node *&astTypeRef);

  /// Returns true if this var should be initialized by copy.
  public: Bool isVarInitByCopy(Core::Ast::Node *astVar)
  {
    auto var = ti_cast<Variable>(astVar);
    if (var == 0) return false;
    this->resolveVarPropsIfNeeded(var);
    return var->getInitMode() == VarInitMode::COPY;
  }

  /// Determining the var's type can move things between its type ref and init args: with `var expression` the init
  /// args might be the var's type, and with `var name: type(args)` the args are moved from the type ref into the init
  /// args. This traces the type to make sure the var's init args are final.
  private: void resolveVarPropsIfNeeded(Variable *var)
  {
    if (var->getType() == 0) this->resolveVarProps(var, true);
  }

  /// Adds an auto `this` definition to the given body, or updates the existing one. Returns the definition's var and
  /// sets index to the definition's index within the body.
  public: Variable* addThisDefinition(
    Block *body, Char const *thisName, Type *astThisType, Bool skipInjection, Int &index
  );

  public: METHOD_BINDING_CACHE(isVoid, Bool, (Core::Ast::Node const*));
  private: static Bool _isVoid(TiObject *self, Core::Ast::Node const *ref);

  public: METHOD_BINDING_CACHE(isCastableTo, Bool, (Core::Ast::Node*, Core::Ast::Node*, Bool));
  private: static Bool _isCastableTo(
    TiObject *self, Core::Ast::Node *srcTypeRef, Core::Ast::Node *targetTypeRef, Bool implicit
  );

  public: METHOD_BINDING_CACHE(matchTargetType,
    TypeMatchStatus, (
      Core::Ast::Node* /* srcTypeRef */, Core::Ast::Node* /* targetTypeRef */, Function*& /* caster */
    )
  );
  private: static TypeMatchStatus _matchTargetType(
    TiObject *self, Core::Ast::Node *srcTypeRef, Core::Ast::Node *targetTypeRef, Function *&caster
  );

  public: METHOD_BINDING_CACHE(isReferenceTypeFor, Bool, (Type*, Type*));
  private: static Bool _isReferenceTypeFor(TiObject *self, Type *refType, Type *contentType);

  public: METHOD_BINDING_CACHE(getReferenceTypeFor, ReferenceType*, (Core::Ast::Node*, ReferenceMode const&));
  private: static ReferenceType* _getReferenceTypeFor(
    TiObject *self, Core::Ast::Node *type, ReferenceMode const &mode
  );

  public: ReferenceType* getReferenceTypeForPointerType(PointerType *type, ReferenceMode const &mode);

  public: METHOD_BINDING_CACHE(getPointerTypeFor, PointerType*, (Core::Ast::Node*));
  private: static PointerType* _getPointerTypeFor(TiObject *self, Core::Ast::Node *type);

  public: METHOD_BINDING_CACHE(getLiteralPointerTypeFor, PointerType*, (Core::Ast::Node*));
  public: static PointerType* _getLiteralPointerTypeFor(TiObject *self, Core::Ast::Node *type);

  public: METHOD_BINDING_CACHE(getArrayTypeFor, ArrayType*, (Core::Ast::Node*));
  private: static ArrayType* _getArrayTypeFor(TiObject *self, Core::Ast::Node *type);

  public: Type* swichInnerReferenceTypeWithPointerType(ReferenceType *type);

  public: Type* swichOuterPointerTypeWithReferenceType(Type *type, ReferenceMode const &mode);

  public: METHOD_BINDING_CACHE(getValueTypeFor, Type*, (Core::Ast::Node*));
  private: static Type* _getValueTypeFor(TiObject *self, Core::Ast::Node *type);

  public: METHOD_BINDING_CACHE(getNullType, IntegerType*);
  private: static IntegerType* _getNullType(TiObject *self);

  public: METHOD_BINDING_CACHE(getBoolType, IntegerType*);
  private: static IntegerType* _getBoolType(TiObject *self);

  public: METHOD_BINDING_CACHE(getCharType, IntegerType*);
  private: static IntegerType* _getCharType(TiObject *self);

  public: METHOD_BINDING_CACHE(getCharArrayType, ArrayType*, (Word));
  private: static ArrayType* _getCharArrayType(TiObject *self, Word size);

  public: METHOD_BINDING_CACHE(getArchIntType, IntegerType*);
  private: static IntegerType* _getArchIntType(TiObject *self);

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

  public: METHOD_BINDING_CACHE(getNodeType, UserType*);
  private: static UserType* _getNodeType(TiObject *self);

  public: template<class T> Bool isTypeOrRefTypeOf(Core::Ast::Node *type)
  {
    if (!type->isDerivedFrom<Type>()) {
      return this->isTypeOrRefTypeOf<T>(this->traceType(type));
    }

    if (type->isDerivedFrom<T>()) return true;

    auto refType = ti_cast<ReferenceType>(type);
    if (refType == 0) return false;
    else return this->isTypeOrRefTypeOf<T>(refType->getContentType(this));
  }

  public: template<class T> T* tryGetPointerContentType(Core::Ast::Node *type)
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

  public: METHOD_BINDING_CACHE(resolveNodePath, Str, (Core::Ast::Node const*));
  private: static Str _resolveNodePath(TiObject *self, Core::Ast::Node const *node);

  public: METHOD_BINDING_CACHE(getFunctionName, Str const&, (Function*));
  private: static Str const& _getFunctionName(TiObject *self, Function *astFunc);

  public: METHOD_BINDING_CACHE(getNeededIntSize, Word, (LongInt));
  private: static Word _getNeededIntSize(TiObject *self, LongInt value);

  public: METHOD_BINDING_CACHE(getNeededWordSize, Word, (LongWord));
  private: static Word _getNeededWordSize(TiObject *self, LongWord value);

  public: METHOD_BINDING_CACHE(getVariableDomain, DefinitionDomain, (Core::Ast::Node const*));
  private: static DefinitionDomain _getVariableDomain(TiObject *self, Core::Ast::Node const *def);

  public: Bool doesModifierExistOnDef(Core::Ast::Definition const *def, Char const *name);
  public: Bool isSharedDef(Core::Ast::Definition const *def)
  {
    return def->getMetadata(S("shared")) != 0;
  }
  public: Bool isNoBindDef(Core::Ast::Definition const *def)
  {
    return def->getMetadata(S("no_bind")) != 0;
  }

  public: METHOD_BINDING_CACHE(validateUseStatement, Bool, (Core::Ast::Bridge* /* bridge */));
  private: static Bool _validateUseStatement(TiObject *self, Core::Ast::Bridge *bridge);

  /// @}

  /// @name Helper Functions
  /// @{

  private: Template* getReferenceTemplate(ReferenceMode const &mode);

  private: Template* getPointerTemplate();
  private: Template* getLiteralPointerTemplate();
  private: PointerType* instantiatePointerType(Template *tpl, Core::Ast::Node *type);

  private: Template* getArrayTemplate();

  /// @}

}; // class

} } // namespace

#endif

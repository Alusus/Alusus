/**
 * @file Spp/Ast/Helper.h
 * Contains the header of class Spp::Ast::Helper.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_HELPER_H
#define SPP_AST_HELPER_H

namespace Spp { namespace Ast
{

class Helper : public TiObject, public virtual DynamicBindings, public virtual DynamicInterfaces
{
  //============================================================================
  // Type Info

  TYPE_INFO(Helper, TiObject, "Spp.Ast", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(DynamicBindings, DynamicInterfaces),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Member Variables

  private: Core::Standard::RootManager *rootManager;

  private: Core::Processing::NoticeStore *noticeStore = 0;
  private: Spp::ExecutionContext *executionContext = 0;
  private: Template *refTemplate = 0;
  private: IntegerType *boolType = 0;
  private: IntegerType *int64Type = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACES(interfaceList);


  //============================================================================
  // Constructor

  Helper(Core::Standard::RootManager *rm) : rootManager(rm)
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
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  public: void prepare(Core::Processing::NoticeStore *ns, Spp::ExecutionContext *ec)
  {
    this->noticeStore = ns;
    this->executionContext = ec;
    this->refTemplate = 0;
  }

  /// @}

  /// @name Property Getters
  /// @{

  public: Core::Standard::RootManager* getRootManager() const
  {
    return this->rootManager;
  }

  public: Core::Data::Seeker* getSeeker() const
  {
    return this->rootManager->getSeeker();
  }

  public: Spp::ExecutionContext* getExecutionContext() const
  {
    return this->executionContext;
  }

  public: Core::Processing::NoticeStore* getNoticeStore() const
  {
    return this->noticeStore;
  }

  /// @}

  /// @name Main Functions
  /// @{

  public: METHOD_BINDING_CACHE(isVarDefinition, Bool, (TiObject*));
  private: static Bool _isVarDefinition(TiObject *self, TiObject *obj);

  public: Bool lookupCalleeByName(
    Char const *name, Core::Data::Node *astNode, Core::Basic::Container<TiObject> *types,
    TiObject *&callee, Type *&calleeType
  );

  public: METHOD_BINDING_CACHE(lookupCallee,
    Bool, (TiObject*, Core::Data::Node*, Core::Basic::Container<TiObject>*, TiObject*&, Type*&)
  );
  private: static Bool _lookupCallee(
    TiObject *self, TiObject *ref, Core::Data::Node *astNode, Core::Basic::Container<TiObject> *types,
    TiObject *&callee, Type *&calleeType
  );

  public: METHOD_BINDING_CACHE(lookupCallee_iteration,
    Core::Data::Seeker::Verb, (
      TiObject*, Core::Basic::Container<TiObject> *, CallMatchStatus&, Int&,
      SharedPtr<Core::Data::Notice>&, TiObject*&, Type*&
    )
  );
  private: static Core::Data::Seeker::Verb _lookupCallee_iteration(
    TiObject *self, TiObject *obj, Core::Basic::Container<TiObject> *types,
    CallMatchStatus &matchStatus, Int &matchCount, SharedPtr<Core::Data::Notice> &notice,
    TiObject *&callee, Type *&calleeType
  );

  public: METHOD_BINDING_CACHE(traceType, Type*, (TiObject*));
  private: static Type* _traceType(TiObject *self, TiObject *ref);

  public: METHOD_BINDING_CACHE(isVoid, Bool, (TiObject*));
  private: static Bool _isVoid(TiObject *self, TiObject *ref);

  public: METHOD_BINDING_CACHE(isImplicitlyCastableTo, Bool, (TiObject*, TiObject*));
  private: static Bool _isImplicitlyCastableTo(TiObject *self, TiObject *srcTypeRef, TiObject *targetTypeRef);

  public: METHOD_BINDING_CACHE(isReferenceTypeFor, Bool, (Type*, Type*));
  private: static Bool _isReferenceTypeFor(TiObject *self, Type *refType, Type *contentType);

  public: METHOD_BINDING_CACHE(getReferenceTypeFor, ReferenceType*, (TiObject*));
  private: static ReferenceType* _getReferenceTypeFor(TiObject *self, TiObject *type);

  public: METHOD_BINDING_CACHE(getBoolType, IntegerType*);
  private: static IntegerType* _getBoolType(TiObject *self);

  public: METHOD_BINDING_CACHE(getInt64Type, IntegerType*);
  private: static IntegerType* _getInt64Type(TiObject *self);

  public: template<class T> Bool isTypeOrRefTypeOf(TiObject *type)
  {
    if (!type->isDerivedFrom<Type>()) {
      return this->isTypeOrRefTypeOf<T>(this->traceType(type));
    }

    if (type->isDerivedFrom<T>()) return true;

    auto refType = ti_cast<ReferenceType>(type);
    if (refType == 0) return false;
    else {
      auto contentType = refType->getContentType(this);
      return contentType->isDerivedFrom<T>();
    }
  }

  /// @}

  /// @name Helper Functions
  /// @{

  private: Template* getReferenceTemplate();

  private: IntegerType* getIntType(Word bitCount);

  /// @}

}; // class

} } // namespace

#endif

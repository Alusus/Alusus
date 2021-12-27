/**
 * @file Core/Data/Seeker.h
 * Contains the header of class Core::Data::Seeker.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_SEEKER_H
#define CORE_DATA_SEEKER_H

namespace Core::Data
{

class Seeker : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(Seeker, TiObject, "Core.Data", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Types

  public: ti_s_enum(Verb, TiInt, "Core.Data", "Core", "alusus.org",
    STOP = 0,
    MOVE = 1,
    PERFORM = 2,
    PERFORM_AND_MOVE = 2 | 1,
    SKIP = 4 | 1,
    SKIP_GROUP = 8 | 4 | 1
  );
  public: ti_s_enum(Action, TiInt, "Core.Data", "Core", "alusus.org",
    TARGET_MATCH,
    OWNER_SCOPE,
    USE_SCOPE,
    USE_SCOPES_START,
    USE_SCOPES_END,
    ALIAS_TRACE_START,
    ALIAS_TRACE_END,
    ERROR
  );
  public: s_enum(Flags,
    SKIP_OWNERS = 1,
    SKIP_OWNED = 2,
    SKIP_USES = 4,
    SKIP_USES_FOR_ALIASES = 8
  );
  public: typedef std::function<Verb(TiInt action, TiObject *&obj)> SetCallback;
  public: typedef std::function<Verb(TiInt action, TiObject *obj)> RemoveCallback;
  public: typedef std::function<Verb(TiInt action, TiObject *obj)> ForeachCallback;


  //============================================================================
  // Implementations

  IMPLEMENT_DYNAMIC_BINDINGS(bindingMap);
  IMPLEMENT_DYNAMIC_INTERFACING(interfaceList);


  //============================================================================
  // Constructors

  Seeker()
  {
    this->initBindingCaches();
    this->initBindings();
  }

  Seeker(Seeker *parent)
  {
    this->initBindingCaches();
    this->inheritBindings(parent);
    this->inheritInterfaces(parent);
  }


  //============================================================================
  // Member Functions

  /// @name Initialization
  /// @{

  private: void initBindingCaches();

  private: void initBindings();

  /// @}

  /// @name Helper Functions
  /// @{

  public: Bool trySet(TiObject const *ref, TiObject *target, TiObject *val, Word flags = 0);

  public: void doSet(TiObject const *ref, TiObject *target, TiObject *val, Word flags = 0)
  {
    if (!this->trySet(ref, target, val, flags)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryRemove(TiObject const *ref, TiObject *target, Word flags = 0);

  public: void doRemove(TiObject const *ref, TiObject *target, Word flags = 0)
  {
    if (!this->tryRemove(ref, target, flags)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryGet(TiObject const *ref, TiObject *target, TiObject *&retVal, Word flags = 0);

  public: TiObject* tryGet(TiObject const *ref, TiObject *target, Word flags = 0)
  {
    TiObject *result = 0;
    this->tryGet(ref, target, result, flags);
    return result;
  }

  public: TiObject* doGet(TiObject const *ref, TiObject *target, Word flags = 0)
  {
    TiObject *retVal = this->tryGet(ref, target, flags);
    if (retVal == 0) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
    return retVal;
  }

  public: Bool find(TiObject const *ref, TiObject *target, TypeInfo const *ti, TiObject *&retVal, Word flags);

  public: template<class T> Bool find(TiObject const *ref, TiObject *target, TiObject *&retVal, Word flags)
  {
    return this->find(ref, target, T::getTypeInfo(), retVal, flags);
  }

  public: static Bool isPerform(Verb verb)
  {
    return (verb & Verb::PERFORM) != 0;
  }

  public: static Bool isMove(Verb verb)
  {
    return (verb & Verb::MOVE) != 0;
  }

  /// @}

  /// @name Set Functions
  /// @{

  public: METHOD_BINDING_CACHE(set, Verb, (TiObject const*, TiObject*, SetCallback const&, Word));
  private: static Verb _set(
    TiObject *self, TiObject const *ref, TiObject *target, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifier,
    Verb, (Data::Ast::Identifier const*, TiObject*, SetCallback const&, Word)
  );
  private: static Verb _set_identifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifierLevel,
    Verb, (Data::Ast::Identifier const*, TiObject*, SetCallback const&, Word)
  );
  private: static Verb _set_identifierLevel(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifierOnScope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, SetCallback const&, Word)
  );
  private: static Verb _set_identifierOnScope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifierOnMap,
    Verb, (Data::Ast::Identifier const*, MapContaining<TiObject>*, SetCallback const&, Word)
  );
  private: static Verb _set_identifierOnMap(
    TiObject *self, Data::Ast::Identifier const *identifier, MapContaining<TiObject> *map, SetCallback const &cb,
    Word flags
  );

  public: METHOD_BINDING_CACHE(set_linkOperator,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, SetCallback const&, Word)
  );
  private: static Verb _set_linkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_linkOperatorRouting,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, SetCallback const&, Word)
  );
  private: static Verb _set_linkOperatorRouting(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SetCallback const &cb, Word flags
  );

  /// @}

  /// @name Remove Functions
  /// @{

  public: METHOD_BINDING_CACHE(remove, Verb, (TiObject const*, TiObject*, RemoveCallback const&, Word));
  private: static Verb _remove(
    TiObject *self, TiObject const *ref, TiObject *target, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifier,
    Verb, (Data::Ast::Identifier const*, TiObject*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifierLevel,
    Verb, (Data::Ast::Identifier const*, TiObject*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifierLevel(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifierOnScope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifierOnScope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifierOnMap,
    Verb, (Data::Ast::Identifier const*, DynamicMapContaining<TiObject>*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifierOnMap(
    TiObject *self, Data::Ast::Identifier const *identifier, DynamicMapContaining<TiObject> *map,
    RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_linkOperator,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_linkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_linkOperatorRouting,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_linkOperatorRouting(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, RemoveCallback const &cb, Word flags
  );

  /// @}

  /// @name Foreach Functions
  /// @{

  public: METHOD_BINDING_CACHE(foreach, Verb, (TiObject const*, TiObject*, ForeachCallback const&, Word));
  private: static Verb _foreach(
    TiObject *self, TiObject const *ref, TiObject *target, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifier,
    Verb, (Data::Ast::Identifier const*, TiObject*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierLevel,
    Verb, (Data::Ast::Identifier const*, TiObject*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierLevel(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierAtScope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierAtScope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnScope,
    Verb, (Data::Ast::Identifier*, Data::Ast::Scope*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierOnScope(
    TiObject *self, Data::Ast::Identifier *identifier, Data::Ast::Scope *scope, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnMap,
    Verb, (Data::Ast::Identifier const*, MapContaining<TiObject>*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierOnMap(
    TiObject *_self, Data::Ast::Identifier const *identifier, MapContaining<TiObject> *map,
    ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_linkOperator,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_linkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_linkOperatorRouting,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_linkOperatorRouting(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, ForeachCallback const &cb, Word flags
  );

  /// @}

  /// @name Other Functions
  /// @{

  public: METHOD_BINDING_CACHE(extForeach, Verb, (TiObject const*, TiObject*, ForeachCallback const&, Word));
  private: static Verb _extForeach(
    TiObject *self, TiObject const *ref, TiObject *target, ForeachCallback const &cb, Word flags
  );

  /// @}

}; // class

} // namespace

#endif

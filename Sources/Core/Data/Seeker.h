/**
 * @file Core/Data/Seeker.h
 * Contains the header of class Core::Data::Seeker.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SEEKER_H
#define CORE_DATA_SEEKER_H

namespace Core { namespace Data
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
    MOVE, STOP, PERFORM_AND_MOVE, PERFORM_AND_STOP
  );
  public: s_enum(Flags, SKIP_OWNERS = 1, SKIP_OWNED = 2, SKIP_USES = 4);
  public: typedef std::function<Verb(TiObject *&obj, Notices::Notice *notice)> SetCallback;
  public: typedef std::function<Verb(TiObject *obj, Notices::Notice *notice)> RemoveCallback;
  public: typedef std::function<Verb(TiObject *obj, Notices::Notice *notice)> ForeachCallback;


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
    return verb == Verb::PERFORM_AND_STOP || verb == Verb::PERFORM_AND_MOVE;
  }

  public: static Bool isMove(Verb verb)
  {
    return verb == Verb::MOVE || verb == Verb::PERFORM_AND_MOVE;
  }

  /// @}

  /// @name Main Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(set, Verb, (TiObject const*, TiObject*, SetCallback const&, Word));
  public: METHOD_BINDING_CACHE(remove, Verb, (TiObject const*, TiObject*, RemoveCallback const&, Word));
  public: METHOD_BINDING_CACHE(foreach, Verb, (TiObject const*, TiObject*, ForeachCallback const&, Word));

  private: static Verb _set(
    TiObject *self, TiObject const *ref, TiObject *target, SetCallback const &cb, Word flags
  );
  private: static Verb _remove(
    TiObject *self, TiObject const *ref, TiObject *target, RemoveCallback const &cb, Word flags
  );
  private: static Verb _foreach(
    TiObject *self, TiObject const *ref, TiObject *target, ForeachCallback const &cb, Word flags
  );

  /// @}

  /// @name Identifier Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(setByIdentifier,
    Verb, (Data::Ast::Identifier const*, TiObject*, SetCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(setByIdentifier_level,
    Verb, (Data::Ast::Identifier const*, TiObject*, SetCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(setByIdentifier_scope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, SetCallback const&, Word)
  );

  private: static Verb _setByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SetCallback const &cb, Word flags
  );
  private: static Verb _setByIdentifier_level(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, SetCallback const &cb, Word flags
  );
  private: static Verb _setByIdentifier_scope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(removeByIdentifier,
    Verb, (Data::Ast::Identifier const*, TiObject*, RemoveCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(removeByIdentifier_level,
    Verb, (Data::Ast::Identifier const*, TiObject*, RemoveCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(removeByIdentifier_scope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, RemoveCallback const&, Word)
  );

  private: static Verb _removeByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, RemoveCallback const &cb, Word flags
  );
  private: static Verb _removeByIdentifier_level(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, RemoveCallback const &cb, Word flags
  );
  private: static Verb _removeByIdentifier_scope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByIdentifier,
    Verb, (Data::Ast::Identifier const*, TiObject*, ForeachCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(foreachByIdentifier_level,
    Verb, (Data::Ast::Identifier const*, TiObject*, ForeachCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(foreachByIdentifier_scope,
    Verb, (Data::Ast::Identifier const*, Ast::Scope*, ForeachCallback const&, Word)
  );

  private: static Verb _foreachByIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, ForeachCallback const &cb, Word flags
  );
  private: static Verb _foreachByIdentifier_level(
    TiObject *self, Data::Ast::Identifier const *identifier, TiObject *data, ForeachCallback const &cb, Word flags
  );
  private: static Verb _foreachByIdentifier_scope(
    TiObject *self, Data::Ast::Identifier const *identifier, Ast::Scope *scope, ForeachCallback const &cb, Word flags
  );

  /// @}

  /// @name LinkOperator Seek Functions
  /// @{

  public: METHOD_BINDING_CACHE(setByLinkOperator,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, SetCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_routing,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, SetCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_scopeDotIdentifier,
    Verb, (Data::Ast::Identifier const*, Data::Ast::Scope*, SetCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(setByLinkOperator_mapDotIdentifier,
    Verb, (Data::Ast::Identifier const*, MapContaining<TiObject>*, SetCallback const&, Word)
  );

  private: static Verb _setByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SetCallback const &cb, Word flags
  );
  private: static Verb _setByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, SetCallback const &cb, Word flags
  );
  private: static Verb _setByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::Ast::Scope *scope, SetCallback const &cb, Word flags
  );
  private: static Verb _setByLinkOperator_mapDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, MapContaining<TiObject> *map, SetCallback const &cb,
    Word flags
  );

  public: METHOD_BINDING_CACHE(removeByLinkOperator,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, RemoveCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_routing,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, RemoveCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_scopeDotIdentifier,
    Verb, (Data::Ast::Identifier const*, Data::Ast::Scope*, RemoveCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(removeByLinkOperator_mapDotIdentifier,
    Verb, (Data::Ast::Identifier const*, DynamicMapContaining<TiObject>*, RemoveCallback const&, Word)
  );

  private: static Verb _removeByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, RemoveCallback const &cb, Word flags
  );
  private: static Verb _removeByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, RemoveCallback const &cb, Word flags
  );
  private: static Verb _removeByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, Data::Ast::Scope *scope, RemoveCallback const &cb,
    Word flags
  );
  private: static Verb _removeByLinkOperator_mapDotIdentifier(
    TiObject *self, Data::Ast::Identifier const *identifier, DynamicMapContaining<TiObject> *map,
    RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreachByLinkOperator,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, ForeachCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_routing,
    Verb, (Data::Ast::LinkOperator const*, TiObject*, ForeachCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_scopeDotIdentifier,
    Verb, (Data::Ast::Identifier*, Data::Ast::Scope*, ForeachCallback const&, Word)
  );
  public: METHOD_BINDING_CACHE(foreachByLinkOperator_mapDotIdentifier,
    Verb, (Data::Ast::Identifier const*, MapContaining<TiObject>*, ForeachCallback const&, Word)
  );

  private: static Verb _foreachByLinkOperator(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, ForeachCallback const &cb, Word flags
  );
  private: static Verb _foreachByLinkOperator_routing(
    TiObject *self, Data::Ast::LinkOperator const *link, TiObject *data, ForeachCallback const &cb, Word flags
  );
  private: static Verb _foreachByLinkOperator_scopeDotIdentifier(
    TiObject *self, Data::Ast::Identifier *identifier, Data::Ast::Scope *scope, ForeachCallback const &cb, Word flags
  );
  private: static Verb _foreachByLinkOperator_mapDotIdentifier(
    TiObject *_self, Data::Ast::Identifier const *identifier, MapContaining<TiObject> *map,
    ForeachCallback const &cb, Word flags
  );

  /// @}

}; // class

} } // namespace

#endif

/**
 * @file Core/Ast/Seeker.h
 * Contains the header of class Core::Ast::Seeker.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_SEEKER_H
#define CORE_AST_SEEKER_H

namespace Core::Ast
{

class Seeker : public TiObject, public DynamicBinding, public DynamicInterfacing
{
  //============================================================================
  // Type Info

  TYPE_INFO(Seeker, TiObject, "Core.Ast", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(DynamicBinding, DynamicInterfacing),
    OBJECT_INTERFACE_LIST(interfaceList)
  ));


  //============================================================================
  // Types

  public: ti_s_enum(Verb, TiInt, "Core.Ast", "Core", "alusus.org",
    STOP = 0,
    MOVE = 1,
    PERFORM = 2,
    PERFORM_AND_MOVE = 2 | 1,
    SKIP = 4 | 1,
    SKIP_GROUP = 8 | 4 | 1
  );
  public: ti_s_enum(Action, TiInt, "Core.Ast", "Core", "alusus.org",
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
  /// The notice passed to the callbacks. It's only set with the ERROR action.
  public: typedef SharedPtr<Notices::Notice> NoticePtr;

  public: typedef std::function<Verb(TiInt action, Node *&obj, NoticePtr const &notice)> SetCallback;
  public: typedef std::function<Verb(TiInt action, Node *obj, NoticePtr const &notice)> RemoveCallback;
  public: typedef std::function<Verb(TiInt action, Node *obj, NoticePtr const &notice)> ForeachCallback;


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

  public: Bool trySet(Node const *ref, Node *target, Node *val, Word flags = 0);

  public: void doSet(Node const *ref, Node *target, Node *val, Word flags = 0)
  {
    if (!this->trySet(ref, target, val, flags)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryRemove(Node const *ref, Node *target, Word flags = 0);

  public: void doRemove(Node const *ref, Node *target, Word flags = 0)
  {
    if (!this->tryRemove(ref, target, flags)) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryGet(Node const *ref, Node *target, Node *&retVal, Word flags = 0);

  public: Node* tryGet(Node const *ref, Node *target, Word flags = 0)
  {
    Node *result = 0;
    this->tryGet(ref, target, result, flags);
    return result;
  }

  public: Node* doGet(Node const *ref, Node *target, Word flags = 0)
  {
    Node *retVal = this->tryGet(ref, target, flags);
    if (retVal == 0) {
      throw EXCEPTION(GenericException, S("Reference pointing to a missing element/tree."));
    }
    return retVal;
  }

  public: Bool find(Node const *ref, Node *target, TypeInfo const *ti, Node *&retVal, Word flags);

  public: template<class T> Bool find(Node const *ref, Node *target, Node *&retVal, Word flags)
  {
    Node *ret;
    if (!this->find(ref, target, T::getTypeInfo(), ret, flags)) return false;
    retVal = static_cast<T*>(ret);
    return true;
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

  public: METHOD_BINDING_CACHE(set, Verb, (Node const*, Node*, SetCallback const&, Word));
  private: static Verb _set(
    TiObject *self, Node const *ref, Node *target, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifier,
    Verb, (Identifier const*, Node*, SetCallback const&, Word)
  );
  private: static Verb _set_identifier(
    TiObject *self, Identifier const *identifier, Node *data, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifierLevel,
    Verb, (Identifier const*, Node*, SetCallback const&, Word)
  );
  private: static Verb _set_identifierLevel(
    TiObject *self, Identifier const *identifier, Node *data, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifierOnScope,
    Verb, (Identifier const*, Scope*, SetCallback const&, Word)
  );
  private: static Verb _set_identifierOnScope(
    TiObject *self, Identifier const *identifier, Scope *scope, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_identifierOnMap,
    Verb, (Identifier const*, MapContaining<Node>*, SetCallback const&, Word)
  );
  private: static Verb _set_identifierOnMap(
    TiObject *self, Identifier const *identifier, MapContaining<Node> *map, SetCallback const &cb,
    Word flags
  );

  public: METHOD_BINDING_CACHE(set_linkOperator,
    Verb, (LinkOperator const*, Node*, SetCallback const&, Word)
  );
  private: static Verb _set_linkOperator(
    TiObject *self, LinkOperator const *link, Node *data, SetCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(set_linkOperatorRouting,
    Verb, (LinkOperator const*, Node*, SetCallback const&, Word)
  );
  private: static Verb _set_linkOperatorRouting(
    TiObject *self, LinkOperator const *link, Node *data, SetCallback const &cb, Word flags
  );

  /// @}

  /// @name Remove Functions
  /// @{

  public: METHOD_BINDING_CACHE(remove, Verb, (Node const*, Node*, RemoveCallback const&, Word));
  private: static Verb _remove(
    TiObject *self, Node const *ref, Node *target, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifier,
    Verb, (Identifier const*, Node*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifier(
    TiObject *self, Identifier const *identifier, Node *data, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifierLevel,
    Verb, (Identifier const*, Node*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifierLevel(
    TiObject *self, Identifier const *identifier, Node *data, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifierOnScope,
    Verb, (Identifier const*, Scope*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifierOnScope(
    TiObject *self, Identifier const *identifier, Scope *scope, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_identifierOnMap,
    Verb, (Identifier const*, DynamicMapContaining<Node>*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_identifierOnMap(
    TiObject *self, Identifier const *identifier, DynamicMapContaining<Node> *map,
    RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_linkOperator,
    Verb, (LinkOperator const*, Node*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_linkOperator(
    TiObject *self, LinkOperator const *link, Node *data, RemoveCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(remove_linkOperatorRouting,
    Verb, (LinkOperator const*, Node*, RemoveCallback const&, Word)
  );
  private: static Verb _remove_linkOperatorRouting(
    TiObject *self, LinkOperator const *link, Node *data, RemoveCallback const &cb, Word flags
  );

  /// @}

  /// @name Foreach Functions
  /// @{

  public: METHOD_BINDING_CACHE(foreach, Verb, (Node const*, Node*, ForeachCallback const&, Word));
  private: static Verb _foreach(
    TiObject *self, Node const *ref, Node *target, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifier,
    Verb, (Identifier const*, Node*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifier(
    TiObject *self, Identifier const *identifier, Node *data, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierLevel,
    Verb, (Identifier const*, Node*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierLevel(
    TiObject *self, Identifier const *identifier, Node *data, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierAtScope,
    Verb, (Identifier const*, Scope*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierAtScope(
    TiObject *self, Identifier const *identifier, Scope *scope, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnScope,
    Verb, (Identifier*, Scope*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierOnScope(
    TiObject *self, Identifier *identifier, Scope *scope, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_identifierOnMap,
    Verb, (Identifier const*, MapContaining<Node>*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_identifierOnMap(
    TiObject *_self, Identifier const *identifier, MapContaining<Node> *map,
    ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_linkOperator,
    Verb, (LinkOperator const*, Node*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_linkOperator(
    TiObject *self, LinkOperator const *link, Node *data, ForeachCallback const &cb, Word flags
  );

  public: METHOD_BINDING_CACHE(foreach_linkOperatorRouting,
    Verb, (LinkOperator const*, Node*, ForeachCallback const&, Word)
  );
  private: static Verb _foreach_linkOperatorRouting(
    TiObject *self, LinkOperator const *link, Node *data, ForeachCallback const &cb, Word flags
  );

  /// @}

  /// @name Other Functions
  /// @{

  public: METHOD_BINDING_CACHE(extForeach, Verb, (Node const*, Node*, ForeachCallback const&, Word));
  private: static Verb _extForeach(
    TiObject *self, Node const *ref, Node *target, ForeachCallback const &cb, Word flags
  );

  /// @}

}; // class

} // namespace

#endif

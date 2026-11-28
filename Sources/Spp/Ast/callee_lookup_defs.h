/**
 * @file Spp/Ast/callee_lookup_defs.h
 * Contains definitions used by CalleeTracer.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_CALLEELOOKUPDEFS_H
#define SPP_AST_CALLEELOOKUPDEFS_H

namespace Spp::Ast
{

/**
 * @enum CalleeLookupMode
 * @ingroup spp_test
 * Specifies what sort of callees are you looking for.
 *
 * The mode is independent from CalleeLookupRequest::thisType, which only specifies the `this` that member callees
 * bind to, if any. An object member lookup can have no `this` (for example when the object is a function pointer that
 * is to be called, or a value of a built-in type).
 *
 * @var CalleeLookupMode::DIRECTLY_ACCESSIBLE
 * Specifies that we are looking for elements that are directly accessible from the current scope. These can be members
 * of the current scope or members of outer scopes, and can include local variables. Scope bridges are also searched.
 *
 * @var CalleeLookupMode::OBJECT_MEMBER
 * Specifies that the target is a type that represents an object of that type rather than the type itself. When a ref
 * is given we are looking for a member of that object (only the target is searched, not its owners). When no ref is
 * given the operation is applied on the object itself (calling a function pointer, a custom or built-in operator,
 * etc.).
 *
 * @var CalleeLookupMode::SCOPE_MEMBER
 * Specifies that we are looking for a non-object element (a global, a type, a function, etc.) defined in a specific
 * scope or type. Only the target is searched, not its owners. Without a ref this behaves like DIRECTLY_ACCESSIBLE,
 * i.e. the target is treated as the element itself rather than an object.
 */
s_enum(CalleeLookupMode, DIRECTLY_ACCESSIBLE, OBJECT_MEMBER, SCOPE_MEMBER);

/// @ingroup spp_ast
struct CalleeLookupRequest
{
  Core::Ast::Node *astNode = 0;
  Core::Ast::Node *target = 0;
  Core::Ast::Node *injectionChainStartTarget = 0;
  CalleeLookupMode mode = CalleeLookupMode::DIRECTLY_ACCESSIBLE;
  // How many more levels of injections can be searched from the target (-1 for unlimited, 0 to skip injections). This
  // is set from the limit of an injection definition (`@injection[N]`) when searching through that injection.
  Int remainingInjectionLevels = -1;
  // Whether the lookup is going through an injection, in which case definitions marked with `not_injected` are skipped.
  Bool throughInjection = false;
  // Allows directly accessing object member vars without an object, which is valid when only computing types.
  Bool treatObjectMembersAsGlobals = false;
  // The definition of the variable whose type is being looked up, if any. The variable's operation modifier and no_bind
  // modifier affect how the operation is applied on the variable's value.
  Core::Ast::Definition *varTargetDef = 0;
  Core::Ast::Node *ref = 0;
  Str op;
  // The `this` that member callees bind to, or 0 if there is none.
  Spp::Ast::Type *thisType = 0;
  Containing<Spp::Ast::Type> *argTypes = 0;
  Core::Ast::Node *templateParam = 0;

  /// Makes this a request for applying the operation on an object of the given type, with the given `this` for
  /// member callees to bind to (0 for none). State belonging to the previous target is reset.
  /// Defined in CalleeTracer.cpp since Type isn't complete at this point.
  void setObjectTarget(Spp::Ast::Type *type, Spp::Ast::Type *thisType);
};

s_enum(CalleeLookupResultStackEntryType,
  OBJECT = 0,
  THIS_MARKER = 1,
  FUNCTION_CALL = 2
);

struct CalleeLookupResultStackEntry
{
  CalleeLookupResultStackEntryType type;
  Core::Ast::Node *obj;

  CalleeLookupResultStackEntry() : type(CalleeLookupResultStackEntryType::OBJECT), obj(0) {}
  CalleeLookupResultStackEntry(CalleeLookupResultStackEntryType t, Core::Ast::Node *o) : type(t), obj(o) {}

  friend Bool operator==(CalleeLookupResultStackEntry const &entry1, CalleeLookupResultStackEntry const &entry2) {
    return entry1.type == entry2.type && entry1.obj == entry2.obj;
  }

  friend Bool operator!=(CalleeLookupResultStackEntry const &entry1, CalleeLookupResultStackEntry const &entry2) {
    return entry1.type != entry2.type || entry1.obj != entry2.obj;
  }
};

/// @ingroup spp_ast
struct CalleeLookupResult
{
  // The stack is processed from the first entry. A THIS_MARKER makes the value generated so far the `this` for what
  // follows. If the stack ends with a THIS_MARKER then the final value binds to itself, otherwise it binds to the last
  // `this` in effect (which, for callees, is the `this` arg to pass to the call).
  TypeMatchStatus matchStatus = TypeMatchStatus::NONE;
  Array<CalleeLookupResultStackEntry> stack;
  SharedPtr<Core::Notices::Notice> notice;
  Int injectionLevel = 0;

  Bool isSuccessful() const
  {
    return this->matchStatus >= TypeMatchStatus::CUSTOM_CASTER && this->notice == 0;
  }
  Bool isFailure() const
  {
    return this->notice != 0;
  }
  Bool isNew() const
  {
    return this->matchStatus == TypeMatchStatus::NONE && this->notice == 0;
  }
  Bool isNameMatched() const
  {
    return !this->isNew() && (this->notice == 0 || !this->notice->isA<Spp::Notices::UnknownSymbolNotice>());
  }

  void pushObject(Core::Ast::Node *obj) {
    this->stack.insert(0, CalleeLookupResultStackEntry(CalleeLookupResultStackEntryType::OBJECT, obj));
  }

  void pushThisMarker() {
    this->stack.insert(0, CalleeLookupResultStackEntry(CalleeLookupResultStackEntryType::THIS_MARKER, 0));
  }

  void pushFunctionCall() {
    this->stack.insert(0, CalleeLookupResultStackEntry(CalleeLookupResultStackEntryType::FUNCTION_CALL, 0));
  }
};

} // namespace


//==============================================================================
// Type Names

DEFINE_TYPE_NAME(Spp::Ast::CalleeLookupRequest, "alusus.org/Spp/Spp.Ast.CalleeLookupRequest");
DEFINE_TYPE_NAME(Spp::Ast::CalleeLookupResult, "alusus.org/Spp/Spp.Ast.CalleeLookupResult");

#endif

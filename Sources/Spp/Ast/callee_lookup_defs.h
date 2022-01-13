/**
 * @file Spp/Ast/callee_lookup_defs.h
 * Contains definitions used by CalleeTracer.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
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

s_enum(CalleeLookupMode, DIRECTLY_ACCESSIBLE, OBJECT_MEMBER, SCOPE_MEMBER);

/// @ingroup spp_ast
struct CalleeLookupRequest
{
  Core::Data::Node *astNode = 0;
  TiObject *target = 0;
  CalleeLookupMode mode = CalleeLookupMode::DIRECTLY_ACCESSIBLE;
  Bool skipInjections = false;
  Char const *varTargetOp = 0;
  TiObject *ref = 0;
  Str op;
  TiObject *thisType = 0;
  Containing<TiObject> *argTypes = 0;
  TiObject *templateParam = 0;
  ExecutionContext const *ec;
};

s_enum(CalleeLookupResultStackEntryType,
  OBJECT = 0,
  THIS_MARKER = 1,
  FUNCTION_CALL = 2
);

struct CalleeLookupResultStackEntry
{
  CalleeLookupResultStackEntryType type;
  TiObject *obj;

  CalleeLookupResultStackEntry() : type(CalleeLookupResultStackEntryType::OBJECT), obj(0) {}
  CalleeLookupResultStackEntry(CalleeLookupResultStackEntryType t, TiObject *o) : type(t), obj(o) {}

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
  TypeMatchStatus matchStatus = TypeMatchStatus::NONE;
  Array<CalleeLookupResultStackEntry> stack;
  SharedPtr<Core::Notices::Notice> notice;

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

  void pushObject(TiObject *obj) {
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

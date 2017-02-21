/**
 * @file Core/Basic/bindings_helpers.h
 * Contains Bindings related helper definitions.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BINDINGSHELPERS_H
#define CORE_BASIC_BINDINGSHELPERS_H

namespace Core { namespace Basic
{

//==============================================================================
// Functions

template <class RT, class ...ARGS> RT call(TiObject *callee, ARGS ...args)
{
  VALIDATE_NOT_NULL(callee);
  auto tifn = ti_cast<TiFunction<RT(TiFunctionBase*, ARGS...)>>(callee);
  if (tifn != 0) {
    return tifn->fn(tifn->getSuper(), args...);
  } else {
    auto tifn2 = ti_cast<TiFunction<RT(ARGS...)>>(callee);
    if (tifn2 == 0) {
      throw EXCEPTION(GenericException, STR("Function signature does not match the call."));
    }
    return tifn2->fn(args...);
  }
}


template <class RT, class ...ARGS> RT call(Bindings *self, Char const *name, ARGS... args)
{
  VALIDATE_NOT_NULL(self, name);
  auto index = self->findMemberIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto obj = self->getMember(index);
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto tifnBase = ti_cast<TiFunctionBase>(obj);
  if (tifnBase == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not a function."), name);
  }
  return call<RT>(tifnBase, args...);
}


template <class RT, class ...ARGS> RT call(BindingMap *bindingMap, Char const *name, ARGS... args)
{
  VALIDATE_NOT_NULL(bindingMap, name);
  auto index = bindingMap->findIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto obj = bindingMap->get(index).get();
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto tifnBase = ti_cast<TiFunctionBase>(obj);
  if (tifnBase == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not a function."), name);
  }
  return call<RT>(tifnBase, args...);
}


inline TiObject* tryGetMember(TiObject *obj, Char const *name)
{
  auto bindings = ti_cast<Bindings>(obj);
  if (bindings == 0) return 0;
  auto index = bindings->findMemberIndex(name);
  if (index == -1) return 0;
  return bindings->getMember(index);
}


//==============================================================================
// Macros

// Convert Bindings* to a typed self.
#define PREPARE_SELF(type) \
  type *self = ti_cast<type>(_self); \
  if (self == 0) { \
    throw EXCEPTION(InvalidArgumentException, STR("_self"), STR("_self is null or of invalid type.")); \
  }

// Set a single member
#define _SET_MEMBER(var) \
  auto value = ti_cast<VARTYPE_FROM_TUPLE var>(val); VARSETTER_FROM_TUPLE var

// Remove a single member
#define _REMOVE_MEMBER(var) \
  auto value = (VARTYPE_FROM_TUPLE var*)0; VARSETTER_FROM_TUPLE var

// Get a single member
#define _GET_MEMBER(var) \
  VARGETTER_FROM_TUPLE var;

// Get the type of a member
#define _GET_MEMBER_TYPE(var) \
  VARTYPE_FROM_TUPLE var::getTypeInfo()

// Get the hold method of a member
#define _GET_MEMBER_HOLDMETHOD(var) \
  HoldMethod::VARHOLDMETHOD_FROM_TUPLE var

// Cases for setMember(key)
#define _IMPLEMENT_BINDINGS_KEYSET_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { \
    _SET_MEMBER(var); return varIndex; \
  }
#define _IMPLEMENT_BINDINGS_KEYSET1(var1) \
  _IMPLEMENT_BINDINGS_KEYSET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDINGS_KEYSET2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYSET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDINGS_KEYSET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYSET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYSET2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYSET_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_BINDINGS_KEYSET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYSET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYSET_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDINGS_KEYSET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_KEYSET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYSET_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDINGS_KEYSET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_KEYSET5, \
               _IMPLEMENT_BINDINGS_KEYSET4, \
               _IMPLEMENT_BINDINGS_KEYSET3, \
               _IMPLEMENT_BINDINGS_KEYSET2, \
               _IMPLEMENT_BINDINGS_KEYSET1)(__VA_ARGS__)

// Cases for setMember(index)
#define _IMPLEMENT_BINDINGS_INDEXSET_CASE(varIndex, var) \
  if (index == varIndex) { _SET_MEMBER(var); return; }
#define _IMPLEMENT_BINDINGS_INDEXSET1(var1) \
  _IMPLEMENT_BINDINGS_INDEXSET_CASE(0, var1)
#define _IMPLEMENT_BINDINGS_INDEXSET2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXSET_CASE(0, var1) \
  _IMPLEMENT_BINDINGS_INDEXSET_CASE(1, var2)
#define _IMPLEMENT_BINDINGS_INDEXSET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXSET2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXSET_CASE(2, var3)
#define _IMPLEMENT_BINDINGS_INDEXSET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXSET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXSET_CASE(3, var4)
#define _IMPLEMENT_BINDINGS_INDEXSET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_INDEXSET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXSET_CASE(4, var5)
#define _IMPLEMENT_BINDINGS_INDEXSET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_INDEXSET5, \
               _IMPLEMENT_BINDINGS_INDEXSET4, \
               _IMPLEMENT_BINDINGS_INDEXSET3, \
               _IMPLEMENT_BINDINGS_INDEXSET2, \
               _IMPLEMENT_BINDINGS_INDEXSET1)(__VA_ARGS__)

// Cases for removeMember(key)
#define _IMPLEMENT_BINDINGS_KEYREMOVE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { \
    _REMOVE_MEMBER(var); return; \
  }
#define _IMPLEMENT_BINDINGS_KEYREMOVE1(var1) \
  _IMPLEMENT_BINDINGS_KEYREMOVE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDINGS_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYREMOVE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDINGS_KEYREMOVE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYREMOVE_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_BINDINGS_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYREMOVE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDINGS_KEYREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYREMOVE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDINGS_KEYREMOVE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_KEYREMOVE5, \
               _IMPLEMENT_BINDINGS_KEYREMOVE4, \
               _IMPLEMENT_BINDINGS_KEYREMOVE3, \
               _IMPLEMENT_BINDINGS_KEYREMOVE2, \
               _IMPLEMENT_BINDINGS_KEYREMOVE1)(__VA_ARGS__)

// Cases for removeMember(index)
#define _IMPLEMENT_BINDINGS_INDEXREMOVE_CASE(varIndex, var) \
  if (index == varIndex) { _REMOVE_MEMBER(var); return; }
#define _IMPLEMENT_BINDINGS_INDEXREMOVE1(var1) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE_CASE(0, var1)
#define _IMPLEMENT_BINDINGS_INDEXREMOVE2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE_CASE(0, var1) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE_CASE(1, var2)
#define _IMPLEMENT_BINDINGS_INDEXREMOVE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE_CASE(2, var3)
#define _IMPLEMENT_BINDINGS_INDEXREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE_CASE(3, var4)
#define _IMPLEMENT_BINDINGS_INDEXREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXREMOVE_CASE(4, var5)
#define _IMPLEMENT_BINDINGS_INDEXREMOVE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_INDEXREMOVE5, \
               _IMPLEMENT_BINDINGS_INDEXREMOVE4, \
               _IMPLEMENT_BINDINGS_INDEXREMOVE3, \
               _IMPLEMENT_BINDINGS_INDEXREMOVE2, \
               _IMPLEMENT_BINDINGS_INDEXREMOVE1)(__VA_ARGS__)

// Cases for getMember(key)
#define _IMPLEMENT_BINDINGS_KEYGET_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_MEMBER(var); }
#define _IMPLEMENT_BINDINGS_KEYGET1(var1) \
  _IMPLEMENT_BINDINGS_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDINGS_KEYGET2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDINGS_KEYGET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYGET2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYGET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYGET_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDINGS_KEYGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYGET_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDINGS_KEYGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_KEYGET5, \
               _IMPLEMENT_BINDINGS_KEYGET4, \
               _IMPLEMENT_BINDINGS_KEYGET3, \
               _IMPLEMENT_BINDINGS_KEYGET2, \
               _IMPLEMENT_BINDINGS_KEYGET1)(__VA_ARGS__)

// Cases for getMember(index)
#define _IMPLEMENT_BINDINGS_INDEXGET_CASE(varIndex, var) \
  if (index == varIndex) { return _GET_MEMBER(var); }
#define _IMPLEMENT_BINDINGS_INDEXGET1(var1) \
  _IMPLEMENT_BINDINGS_INDEXGET_CASE(0, var1)
#define _IMPLEMENT_BINDINGS_INDEXGET2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXGET_CASE(0, var1) \
  _IMPLEMENT_BINDINGS_INDEXGET_CASE(1, var2)
#define _IMPLEMENT_BINDINGS_INDEXGET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXGET2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXGET_CASE(2, var3)
#define _IMPLEMENT_BINDINGS_INDEXGET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXGET3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXGET_CASE(3, var4)
#define _IMPLEMENT_BINDINGS_INDEXGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_INDEXGET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXGET_CASE(4, var5)
#define _IMPLEMENT_BINDINGS_INDEXGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_INDEXGET5, \
               _IMPLEMENT_BINDINGS_INDEXGET4, \
               _IMPLEMENT_BINDINGS_INDEXGET3, \
               _IMPLEMENT_BINDINGS_INDEXGET2, \
               _IMPLEMENT_BINDINGS_INDEXGET1)(__VA_ARGS__)

// Cases for getMemberNeededType(key)
#define _IMPLEMENT_BINDINGS_KEYGETTYPE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_MEMBER_TYPE(var); }
#define _IMPLEMENT_BINDINGS_KEYGETTYPE1(var1) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDINGS_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDINGS_KEYGETTYPE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYGETTYPE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDINGS_KEYGETTYPE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_KEYGETTYPE5, \
               _IMPLEMENT_BINDINGS_KEYGETTYPE4, \
               _IMPLEMENT_BINDINGS_KEYGETTYPE3, \
               _IMPLEMENT_BINDINGS_KEYGETTYPE2, \
               _IMPLEMENT_BINDINGS_KEYGETTYPE1)(__VA_ARGS__)

// Cases for getMemberNeededType(index)
#define _IMPLEMENT_BINDINGS_INDEXGETTYPE_CASE(varIndex, var) \
  if (index == varIndex) { return _GET_MEMBER_TYPE(var); }
#define _IMPLEMENT_BINDINGS_INDEXGETTYPE1(var1) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE_CASE(0, var1)
#define _IMPLEMENT_BINDINGS_INDEXGETTYPE2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE_CASE(0, var1) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE_CASE(1, var2)
#define _IMPLEMENT_BINDINGS_INDEXGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE_CASE(2, var3)
#define _IMPLEMENT_BINDINGS_INDEXGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE_CASE(3, var4)
#define _IMPLEMENT_BINDINGS_INDEXGETTYPE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXGETTYPE_CASE(4, var5)
#define _IMPLEMENT_BINDINGS_INDEXGETTYPE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_INDEXGETTYPE5, \
               _IMPLEMENT_BINDINGS_INDEXGETTYPE4, \
               _IMPLEMENT_BINDINGS_INDEXGETTYPE3, \
               _IMPLEMENT_BINDINGS_INDEXGETTYPE2, \
               _IMPLEMENT_BINDINGS_INDEXGETTYPE1)(__VA_ARGS__)

// Cases for getMemberHoldMethod(key)
#define _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_MEMBER_HOLDMETHOD(var); }
#define _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD1(var1) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD5, \
               _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD4, \
               _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD3, \
               _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD2, \
               _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD1)(__VA_ARGS__)

// Cases for getMemberHoldMethod(index)
#define _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD_CASE(varIndex, var) \
  if (index == varIndex) { return _GET_MEMBER_HOLDMETHOD(var); }
#define _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD1(var1) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD_CASE(0, var1)
#define _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD_CASE(0, var1) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD_CASE(1, var2)
#define _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD_CASE(2, var3)
#define _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD_CASE(3, var4)
#define _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD_CASE(4, var5)
#define _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD5, \
               _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD4, \
               _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD3, \
               _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD2, \
               _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD1)(__VA_ARGS__)

// Cases for getMemberKey
#define _IMPLEMENT_BINDINGS_GETKEY_CASE(varIndex, varNameStr) \
  if (varIndex == index) return SBSTR(varNameStr);
#define _IMPLEMENT_BINDINGS_GETKEY1(var1) \
  _IMPLEMENT_BINDINGS_GETKEY_CASE(0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_BINDINGS_GETKEY2(var1, var2) \
  _IMPLEMENT_BINDINGS_GETKEY_CASE(0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_BINDINGS_GETKEY_CASE(1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_BINDINGS_GETKEY3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_GETKEY2(var1, var2) \
  _IMPLEMENT_BINDINGS_GETKEY_CASE(2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_BINDINGS_GETKEY4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_GETKEY3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_GETKEY_CASE(3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_BINDINGS_GETKEY5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_GETKEY4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_GETKEY_CASE(4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_BINDINGS_GETKEY(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_GETKEY5, \
               _IMPLEMENT_BINDINGS_GETKEY4, \
               _IMPLEMENT_BINDINGS_GETKEY3, \
               _IMPLEMENT_BINDINGS_GETKEY2, \
               _IMPLEMENT_BINDINGS_GETKEY1)(__VA_ARGS__)

// Cases for findMemberIndex
#define _IMPLEMENT_BINDINGS_FINDINDEX_CASE(varIndex, varNameStr) \
  if (SBSTR(key) == varNameStr) return varIndex;
#define _IMPLEMENT_BINDINGS_FINDINDEX1(var1) \
  _IMPLEMENT_BINDINGS_FINDINDEX_CASE(0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_BINDINGS_FINDINDEX2(var1, var2) \
  _IMPLEMENT_BINDINGS_FINDINDEX_CASE(0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_BINDINGS_FINDINDEX_CASE(1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_BINDINGS_FINDINDEX3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_FINDINDEX2(var1, var2) \
  _IMPLEMENT_BINDINGS_FINDINDEX_CASE(2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_BINDINGS_FINDINDEX4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_FINDINDEX3(var1, var2, var3) \
  _IMPLEMENT_BINDINGS_FINDINDEX_CASE(3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_BINDINGS_FINDINDEX5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDINGS_FINDINDEX4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDINGS_FINDINDEX_CASE(4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_BINDINGS_FINDINDEX(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDINGS_FINDINDEX5, \
               _IMPLEMENT_BINDINGS_FINDINDEX4, \
               _IMPLEMENT_BINDINGS_FINDINDEX3, \
               _IMPLEMENT_BINDINGS_FINDINDEX2, \
               _IMPLEMENT_BINDINGS_FINDINDEX1)(__VA_ARGS__)

// MapContainer Implementation Macro
#define IMPLEMENT_BINDINGS(...) \
  public: using Bindings::setMember; \
  public: virtual Int setMember(Char const *key, TiObject *val) \
  { \
    _IMPLEMENT_BINDINGS_KEYSET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual void setMember(Int index, TiObject *val) \
  { \
    _IMPLEMENT_BINDINGS_INDEXSET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual void removeMember(Char const *key) \
  { \
    _IMPLEMENT_BINDINGS_KEYREMOVE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual void removeMember(Int index) \
  { \
    _IMPLEMENT_BINDINGS_INDEXREMOVE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual Word getMemberCount() const \
  { \
    return SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, 5, 4, 3, 2, 1); \
  } \
  public: using Bindings::getMember; \
  public: virtual TiObject* getMember(Char const *key) \
  { \
    _IMPLEMENT_BINDINGS_KEYGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual TiObject* getMember(Int index) \
  { \
    _IMPLEMENT_BINDINGS_INDEXGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual TiObject const* getMember(Char const *key) const \
  { \
    _IMPLEMENT_BINDINGS_KEYGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual TiObject const* getMember(Int index) const \
  { \
    _IMPLEMENT_BINDINGS_INDEXGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual TypeInfo* getMemberNeededType(Char const *key) const \
  { \
    _IMPLEMENT_BINDINGS_KEYGETTYPE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual TypeInfo* getMemberNeededType(Int index) const \
  { \
    _IMPLEMENT_BINDINGS_INDEXGETTYPE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual HoldMethod getMemberHoldMethod(Char const *key) const \
  { \
    _IMPLEMENT_BINDINGS_KEYGETHOLDMETHOD(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual HoldMethod getMemberHoldMethod(Int index) const \
  { \
    _IMPLEMENT_BINDINGS_INDEXGETHOLDMETHOD(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual const SbStr& getMemberKey(Int index) const \
  { \
    _IMPLEMENT_BINDINGS_GETKEY(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual Int findMemberIndex(Char const *key) const \
  { \
    if (key == 0) { \
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("key is null")); \
    } \
    _IMPLEMENT_BINDINGS_FINDINDEX(__VA_ARGS__); \
    return -1; \
  }

} } // namespace

#endif

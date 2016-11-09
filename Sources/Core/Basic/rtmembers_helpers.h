/**
 * @file Core/Basic/rtmembers_helpers.h
 * Contains RtMembers related helper definitions.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_RTMEMBERSHELPERS_H
#define CORE_BASIC_RTMEMBERSHELPERS_H

namespace Core { namespace Basic
{

//==============================================================================
// Functions

template <class RT, class SELF, class ...ARGS> RT callRtMethod(SELF *self, Char const *name, ARGS... args)
{
  auto index = self->findRtMemberIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto obj = self->getRtMember(index);
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto tifnBase = ti_cast<TiFunctionBase>(obj);
  if (tifnBase == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not a function."), name);
  }
  return callRtMethod<RT>(self, tifnBase, args...);
}


template <class SELF, class ...ARGS> void callVoidRtMethod(SELF *self, Char const *name, ARGS... args)
{
  auto index = self->findRtMemberIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto obj = self->getRtMember(index);
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member not found."), name);
  }
  auto tifnBase = ti_cast<TiFunctionBase>(obj);
  if (tifnBase == 0) {
    throw EXCEPTION(InvalidArgumentException, STR("name"), STR("Member is not a function."), name);
  }
  callVoidRtMethod(self, tifnBase, args...);
}


template <class RT, class SELF, class ...ARGS> RT callRtMethod(SELF *self, TiFunctionBase *tifnBase, ARGS... args)
{
  auto tifn = ti_cast<TiFunction<RT(SELF*, TiFunctionBase*, ARGS...)>>(tifnBase);
  if (tifn != 0) {
    auto tifnOverride = ti_cast<TiFunctionOverride<RT(SELF*, TiFunctionBase*, ARGS...)>>(tifnBase);
    if (tifnOverride != 0) {
      return tifn->fn(self, tifnOverride->superFn.get(), args...);
    } else {
      return tifn->fn(self, 0, args...);
    }
  } else {
    auto tifn2 = ti_cast<TiFunction<RT(SELF*, ARGS...)>>(tifnBase);
    if (tifn2 == 0) {
      throw EXCEPTION(GenericException, STR("Function signature does not match the call."));
    }
    return tifn2->fn(self, args...);
  }
}


template <class SELF, class ...ARGS> void callVoidRtMethod(SELF *self, TiFunctionBase *tifnBase, ARGS... args)
{
  auto tifn = ti_cast<TiFunction<void(SELF*, TiFunctionBase*, ARGS...)>>(tifnBase);
  if (tifn != 0) {
    auto tifnOverride = ti_cast<TiFunctionOverride<void(SELF*, TiFunctionBase*, ARGS...)>>(tifnBase);
    if (tifnOverride != 0) {
      tifn->fn(self, tifnOverride->superFn.get(), args...);
    } else {
      tifn->fn(self, 0, args...);
    }
  } else {
    auto tifn2 = ti_cast<TiFunction<void(SELF*, ARGS...)>>(tifnBase);
    if (tifn2 == 0) {
      throw EXCEPTION(GenericException, STR("Function signature does not match the call."));
    }
    tifn2->fn(self, args...);
  }
}


inline TiObject* tryGetRtMember(TiObject *obj, Char const *name)
{
  auto rtMembers = ti_cast<RtMembers>(obj);
  if (rtMembers == 0) return 0;
  auto index = rtMembers->findRtMemberIndex(name);
  if (index == -1) return 0;
  return rtMembers->getRtMember(index);
}


//==============================================================================
// Macros

// Set a single member
#define _SET_RTMEMBER(var) \
  auto value = ti_cast<VARTYPE_FROM_TUPLE var>(val); VARSETTER_FROM_TUPLE var

// Remove a single member
#define _REMOVE_RTMEMBER(var) \
  auto value = (VARTYPE_FROM_TUPLE var*)0; VARSETTER_FROM_TUPLE var

// Get a single member
#define _GET_RTMEMBER(var) \
  VARGETTER_FROM_TUPLE var;

// Get the type of a member
#define _GET_RTMEMBER_TYPE(var) \
  VARTYPE_FROM_TUPLE var::getTypeInfo()

// Get the hold method of a member
#define _GET_RTMEMBER_HOLDMETHOD(var) \
  HoldMethod::VARHOLDMETHOD_FROM_TUPLE var

// Cases for setRtMember(key)
#define _IMPLEMENT_RTMEMBERS_KEYSET_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { \
    _SET_RTMEMBER(var); return varIndex; \
  }
#define _IMPLEMENT_RTMEMBERS_KEYSET1(var1) \
  _IMPLEMENT_RTMEMBERS_KEYSET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_RTMEMBERS_KEYSET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYSET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_RTMEMBERS_KEYSET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYSET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYSET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYSET_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_RTMEMBERS_KEYSET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYSET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYSET_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_RTMEMBERS_KEYSET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_KEYSET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYSET_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_RTMEMBERS_KEYSET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_KEYSET5, \
               _IMPLEMENT_RTMEMBERS_KEYSET4, \
               _IMPLEMENT_RTMEMBERS_KEYSET3, \
               _IMPLEMENT_RTMEMBERS_KEYSET2, \
               _IMPLEMENT_RTMEMBERS_KEYSET1)(__VA_ARGS__)

// Cases for setRtMember(index)
#define _IMPLEMENT_RTMEMBERS_INDEXSET_CASE(varIndex, var) \
  if (index == varIndex) { _SET_RTMEMBER(var); return; }
#define _IMPLEMENT_RTMEMBERS_INDEXSET1(var1) \
  _IMPLEMENT_RTMEMBERS_INDEXSET_CASE(0, var1)
#define _IMPLEMENT_RTMEMBERS_INDEXSET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXSET_CASE(0, var1) \
  _IMPLEMENT_RTMEMBERS_INDEXSET_CASE(1, var2)
#define _IMPLEMENT_RTMEMBERS_INDEXSET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXSET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXSET_CASE(2, var3)
#define _IMPLEMENT_RTMEMBERS_INDEXSET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXSET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXSET_CASE(3, var4)
#define _IMPLEMENT_RTMEMBERS_INDEXSET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_INDEXSET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXSET_CASE(4, var5)
#define _IMPLEMENT_RTMEMBERS_INDEXSET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_INDEXSET5, \
               _IMPLEMENT_RTMEMBERS_INDEXSET4, \
               _IMPLEMENT_RTMEMBERS_INDEXSET3, \
               _IMPLEMENT_RTMEMBERS_INDEXSET2, \
               _IMPLEMENT_RTMEMBERS_INDEXSET1)(__VA_ARGS__)

// Cases for removeRtMember(key)
#define _IMPLEMENT_RTMEMBERS_KEYREMOVE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { \
    _REMOVE_RTMEMBER(var); return; \
  }
#define _IMPLEMENT_RTMEMBERS_KEYREMOVE1(var1) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_RTMEMBERS_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_RTMEMBERS_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_RTMEMBERS_KEYREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYREMOVE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_RTMEMBERS_KEYREMOVE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_KEYREMOVE5, \
               _IMPLEMENT_RTMEMBERS_KEYREMOVE4, \
               _IMPLEMENT_RTMEMBERS_KEYREMOVE3, \
               _IMPLEMENT_RTMEMBERS_KEYREMOVE2, \
               _IMPLEMENT_RTMEMBERS_KEYREMOVE1)(__VA_ARGS__)

// Cases for removeRtMember(index)
#define _IMPLEMENT_RTMEMBERS_INDEXREMOVE_CASE(varIndex, var) \
  if (index == varIndex) { _REMOVE_RTMEMBER(var); return; }
#define _IMPLEMENT_RTMEMBERS_INDEXREMOVE1(var1) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE_CASE(0, var1)
#define _IMPLEMENT_RTMEMBERS_INDEXREMOVE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE_CASE(0, var1) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE_CASE(1, var2)
#define _IMPLEMENT_RTMEMBERS_INDEXREMOVE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE_CASE(2, var3)
#define _IMPLEMENT_RTMEMBERS_INDEXREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE_CASE(3, var4)
#define _IMPLEMENT_RTMEMBERS_INDEXREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXREMOVE_CASE(4, var5)
#define _IMPLEMENT_RTMEMBERS_INDEXREMOVE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_INDEXREMOVE5, \
               _IMPLEMENT_RTMEMBERS_INDEXREMOVE4, \
               _IMPLEMENT_RTMEMBERS_INDEXREMOVE3, \
               _IMPLEMENT_RTMEMBERS_INDEXREMOVE2, \
               _IMPLEMENT_RTMEMBERS_INDEXREMOVE1)(__VA_ARGS__)

// Cases for getRtMember(key)
#define _IMPLEMENT_RTMEMBERS_KEYGET_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_RTMEMBER(var); }
#define _IMPLEMENT_RTMEMBERS_KEYGET1(var1) \
  _IMPLEMENT_RTMEMBERS_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_RTMEMBERS_KEYGET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_RTMEMBERS_KEYGET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYGET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYGET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYGET_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_RTMEMBERS_KEYGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYGET_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_RTMEMBERS_KEYGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_KEYGET5, \
               _IMPLEMENT_RTMEMBERS_KEYGET4, \
               _IMPLEMENT_RTMEMBERS_KEYGET3, \
               _IMPLEMENT_RTMEMBERS_KEYGET2, \
               _IMPLEMENT_RTMEMBERS_KEYGET1)(__VA_ARGS__)

// Cases for getRtMember(index)
#define _IMPLEMENT_RTMEMBERS_INDEXGET_CASE(varIndex, var) \
  if (index == varIndex) { return _GET_RTMEMBER(var); }
#define _IMPLEMENT_RTMEMBERS_INDEXGET1(var1) \
  _IMPLEMENT_RTMEMBERS_INDEXGET_CASE(0, var1)
#define _IMPLEMENT_RTMEMBERS_INDEXGET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXGET_CASE(0, var1) \
  _IMPLEMENT_RTMEMBERS_INDEXGET_CASE(1, var2)
#define _IMPLEMENT_RTMEMBERS_INDEXGET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXGET2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXGET_CASE(2, var3)
#define _IMPLEMENT_RTMEMBERS_INDEXGET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXGET3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXGET_CASE(3, var4)
#define _IMPLEMENT_RTMEMBERS_INDEXGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_INDEXGET4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXGET_CASE(4, var5)
#define _IMPLEMENT_RTMEMBERS_INDEXGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_INDEXGET5, \
               _IMPLEMENT_RTMEMBERS_INDEXGET4, \
               _IMPLEMENT_RTMEMBERS_INDEXGET3, \
               _IMPLEMENT_RTMEMBERS_INDEXGET2, \
               _IMPLEMENT_RTMEMBERS_INDEXGET1)(__VA_ARGS__)

// Cases for getRtMemberNeededType(key)
#define _IMPLEMENT_RTMEMBERS_KEYGETTYPE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_RTMEMBER_TYPE(var); }
#define _IMPLEMENT_RTMEMBERS_KEYGETTYPE1(var1) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_RTMEMBERS_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_RTMEMBERS_KEYGETTYPE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYGETTYPE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_RTMEMBERS_KEYGETTYPE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_KEYGETTYPE5, \
               _IMPLEMENT_RTMEMBERS_KEYGETTYPE4, \
               _IMPLEMENT_RTMEMBERS_KEYGETTYPE3, \
               _IMPLEMENT_RTMEMBERS_KEYGETTYPE2, \
               _IMPLEMENT_RTMEMBERS_KEYGETTYPE1)(__VA_ARGS__)

// Cases for getRtMemberNeededType(index)
#define _IMPLEMENT_RTMEMBERS_INDEXGETTYPE_CASE(varIndex, var) \
  if (index == varIndex) { return _GET_RTMEMBER_TYPE(var); }
#define _IMPLEMENT_RTMEMBERS_INDEXGETTYPE1(var1) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE_CASE(0, var1)
#define _IMPLEMENT_RTMEMBERS_INDEXGETTYPE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE_CASE(0, var1) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE_CASE(1, var2)
#define _IMPLEMENT_RTMEMBERS_INDEXGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE_CASE(2, var3)
#define _IMPLEMENT_RTMEMBERS_INDEXGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE_CASE(3, var4)
#define _IMPLEMENT_RTMEMBERS_INDEXGETTYPE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXGETTYPE_CASE(4, var5)
#define _IMPLEMENT_RTMEMBERS_INDEXGETTYPE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_INDEXGETTYPE5, \
               _IMPLEMENT_RTMEMBERS_INDEXGETTYPE4, \
               _IMPLEMENT_RTMEMBERS_INDEXGETTYPE3, \
               _IMPLEMENT_RTMEMBERS_INDEXGETTYPE2, \
               _IMPLEMENT_RTMEMBERS_INDEXGETTYPE1)(__VA_ARGS__)

// Cases for getRtMemberHoldMethod(key)
#define _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_RTMEMBER_HOLDMETHOD(var); }
#define _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD1(var1) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD5, \
               _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD4, \
               _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD3, \
               _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD2, \
               _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD1)(__VA_ARGS__)

// Cases for getRtMemberHoldMethod(index)
#define _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD_CASE(varIndex, var) \
  if (index == varIndex) { return _GET_RTMEMBER_HOLDMETHOD(var); }
#define _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD1(var1) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD_CASE(0, var1)
#define _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD_CASE(0, var1) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD_CASE(1, var2)
#define _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD_CASE(2, var3)
#define _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD_CASE(3, var4)
#define _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD_CASE(4, var5)
#define _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD5, \
               _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD4, \
               _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD3, \
               _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD2, \
               _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD1)(__VA_ARGS__)

// Cases for getRtMemberKey
#define _IMPLEMENT_RTMEMBERS_GETKEY_CASE(varIndex, varNameStr) \
  if (varIndex == index) return SBSTR(varNameStr);
#define _IMPLEMENT_RTMEMBERS_GETKEY1(var1) \
  _IMPLEMENT_RTMEMBERS_GETKEY_CASE(0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_RTMEMBERS_GETKEY2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_GETKEY_CASE(0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_RTMEMBERS_GETKEY_CASE(1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_RTMEMBERS_GETKEY3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_GETKEY2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_GETKEY_CASE(2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_RTMEMBERS_GETKEY4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_GETKEY3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_GETKEY_CASE(3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_RTMEMBERS_GETKEY5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_GETKEY4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_GETKEY_CASE(4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_RTMEMBERS_GETKEY(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_GETKEY5, \
               _IMPLEMENT_RTMEMBERS_GETKEY4, \
               _IMPLEMENT_RTMEMBERS_GETKEY3, \
               _IMPLEMENT_RTMEMBERS_GETKEY2, \
               _IMPLEMENT_RTMEMBERS_GETKEY1)(__VA_ARGS__)

// Cases for findRtMemberIndex
#define _IMPLEMENT_RTMEMBERS_FINDINDEX_CASE(varIndex, varNameStr) \
  if (SBSTR(key) == varNameStr) return varIndex;
#define _IMPLEMENT_RTMEMBERS_FINDINDEX1(var1) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX_CASE(0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_RTMEMBERS_FINDINDEX2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX_CASE(0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX_CASE(1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_RTMEMBERS_FINDINDEX3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX2(var1, var2) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX_CASE(2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_RTMEMBERS_FINDINDEX4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX3(var1, var2, var3) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX_CASE(3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_RTMEMBERS_FINDINDEX5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX4(var1, var2, var3, var4) \
  _IMPLEMENT_RTMEMBERS_FINDINDEX_CASE(4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_RTMEMBERS_FINDINDEX(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_RTMEMBERS_FINDINDEX5, \
               _IMPLEMENT_RTMEMBERS_FINDINDEX4, \
               _IMPLEMENT_RTMEMBERS_FINDINDEX3, \
               _IMPLEMENT_RTMEMBERS_FINDINDEX2, \
               _IMPLEMENT_RTMEMBERS_FINDINDEX1)(__VA_ARGS__)

// MapContainer Implementation Macro
#define IMPLEMENT_RTMEMBERS(...) \
  public: using RtMembers::setRtMember; \
  public: virtual Int setRtMember(Char const *key, TiObject *val) \
  { \
    _IMPLEMENT_RTMEMBERS_KEYSET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual void setRtMember(Int index, TiObject *val) \
  { \
    _IMPLEMENT_RTMEMBERS_INDEXSET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual void removeRtMember(Char const *key) \
  { \
    _IMPLEMENT_RTMEMBERS_KEYREMOVE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual void removeRtMember(Int index) \
  { \
    _IMPLEMENT_RTMEMBERS_INDEXREMOVE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual Word getRtMemberCount() const \
  { \
    return SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, 5, 4, 3, 2, 1); \
  } \
  public: using RtMembers::getRtMember; \
  public: virtual TiObject* getRtMember(Char const *key) \
  { \
    _IMPLEMENT_RTMEMBERS_KEYGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual TiObject* getRtMember(Int index) \
  { \
    _IMPLEMENT_RTMEMBERS_INDEXGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual TiObject const* getRtMember(Char const *key) const \
  { \
    _IMPLEMENT_RTMEMBERS_KEYGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual TiObject const* getRtMember(Int index) const \
  { \
    _IMPLEMENT_RTMEMBERS_INDEXGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual TypeInfo* getRtMemberNeededType(Char const *key) const \
  { \
    _IMPLEMENT_RTMEMBERS_KEYGETTYPE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual TypeInfo* getRtMemberNeededType(Int index) const \
  { \
    _IMPLEMENT_RTMEMBERS_INDEXGETTYPE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual HoldMethod getRtMemberHoldMethod(Char const *key) const \
  { \
    _IMPLEMENT_RTMEMBERS_KEYGETHOLDMETHOD(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual HoldMethod getRtMemberHoldMethod(Int index) const \
  { \
    _IMPLEMENT_RTMEMBERS_INDEXGETHOLDMETHOD(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual const SbStr& getRtMemberKey(Int index) const \
  { \
    _IMPLEMENT_RTMEMBERS_GETKEY(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual Int findRtMemberIndex(Char const *key) const \
  { \
    if (key == 0) { \
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("key is null")); \
    } \
    _IMPLEMENT_RTMEMBERS_FINDINDEX(__VA_ARGS__); \
    return -1; \
  }

} } // namespace

#endif

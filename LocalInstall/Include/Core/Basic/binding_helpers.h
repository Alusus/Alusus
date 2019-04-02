/**
 * @file Core/Basic/bindings_helpers.h
 * Contains Binding related helper definitions.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BINDINGHELPERS_H
#define CORE_BASIC_BINDINGHELPERS_H

namespace Core::Basic
{

//==============================================================================
// Functions

template <class RT, class ...ARGS> RT call(TiObject *callee, ARGS... args)
{
  VALIDATE_NOT_NULL(callee);
  auto tifn = ti_cast<TiFunction<RT(TiFunctionBase*, ARGS...)>>(callee);
  if (tifn != 0) {
    return tifn->fn(tifn->getSuper(), args...);
  } else {
    auto tifn2 = ti_cast<TiFunction<RT(ARGS...)>>(callee);
    if (tifn2 == 0) {
      throw EXCEPTION(GenericException, S("Function signature does not match the call."));
    }
    return tifn2->fn(args...);
  }
}


template <class RT, class ...ARGS> RT call(Binding *self, Char const *name, ARGS... args)
{
  VALIDATE_NOT_NULL(self, name);
  auto index = self->findMemberIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
  }
  auto obj = self->getMember(index);
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
  }
  auto tifnBase = ti_cast<TiFunctionBase>(obj);
  if (tifnBase == 0) {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member is not a function."), name);
  }
  return call<RT, ARGS...>(tifnBase, args...);
}


template <class RT, class ...ARGS> RT call(BindingMap *bindingMap, Char const *name, ARGS... args)
{
  VALIDATE_NOT_NULL(bindingMap, name);
  auto index = bindingMap->findIndex(name);
  if (index == -1) {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
  }
  auto obj = bindingMap->get(index).get();
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member not found."), name);
  }
  auto tifnBase = ti_cast<TiFunctionBase>(obj);
  if (tifnBase == 0) {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Member is not a function."), name);
  }
  return call<RT, ARGS...>(tifnBase, args...);
}


inline TiObject* tryGetMember(TiObject *obj, Char const *name)
{
  auto bindings = ti_cast<Binding>(obj);
  if (bindings == 0) return 0;
  auto index = bindings->findMemberIndex(name);
  if (index == -1) return 0;
  return bindings->getMember(index);
}


//==============================================================================
// Macros

// Set a single member
#define _SET_MEMBER(var) \
  auto value = ti_cast<VARTYPE_FROM_TUPLE var>(val); \
  if (value == 0 && val != 0) { \
    Str msg = S("Invalid argument type for member `"); \
    msg += VARNAMESTR_FROM_TUPLE var; \
    msg += S("`"); \
    throw EXCEPTION(InvalidArgumentException, S("val"), msg.c_str(), val->getMyTypeInfo()->getUniqueName()); \
  } \
  VARSETTER_FROM_TUPLE var

// Get a single member
#define _GET_MEMBER(var) \
  VARGETTER_FROM_TUPLE var

// Get the type of a member
#define _GET_MEMBER_TYPE(var) \
  VARTYPE_FROM_TUPLE var

// Get the type info of a member
#define _GET_MEMBER_TYPE_INFO(var) \
  VARTYPE_FROM_TUPLE var::getTypeInfo()

// Get the hold method of a member
#define _GET_MEMBER_HOLDMODE(var) \
  HoldMode::VARHOLDMODE_FROM_TUPLE var

// Cases for setMember(key)
#define _IMPLEMENT_BINDING_KEYSET_CASE(parent, varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { \
    _SET_MEMBER(var); return varIndex + parent::getMemberCount(); \
  }
#define _IMPLEMENT_BINDING_KEYSET1(parent, var1) \
  _IMPLEMENT_BINDING_KEYSET_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDING_KEYSET2(parent, var1, var2) \
  _IMPLEMENT_BINDING_KEYSET_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDING_KEYSET_CASE(parent, 1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDING_KEYSET3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYSET2(parent, var1, var2) \
  _IMPLEMENT_BINDING_KEYSET_CASE(parent, 2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_BINDING_KEYSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYSET3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYSET_CASE(parent, 3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDING_KEYSET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_KEYSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYSET_CASE(parent, 4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDING_KEYSET(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_KEYSET5, \
               _IMPLEMENT_BINDING_KEYSET4, \
               _IMPLEMENT_BINDING_KEYSET3, \
               _IMPLEMENT_BINDING_KEYSET2, \
               _IMPLEMENT_BINDING_KEYSET1)(parent, __VA_ARGS__)

// Cases for setMember(index)
#define _IMPLEMENT_BINDING_INDEXSET_CASE(parent, varIndex, var) \
  if (index == parent::getMemberCount() + varIndex) { _SET_MEMBER(var); return; }
#define _IMPLEMENT_BINDING_INDEXSET1(parent, var1) \
  _IMPLEMENT_BINDING_INDEXSET_CASE(parent, 0, var1)
#define _IMPLEMENT_BINDING_INDEXSET2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXSET_CASE(parent, 0, var1) \
  _IMPLEMENT_BINDING_INDEXSET_CASE(parent, 1, var2)
#define _IMPLEMENT_BINDING_INDEXSET3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXSET2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXSET_CASE(parent, 2, var3)
#define _IMPLEMENT_BINDING_INDEXSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXSET3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXSET_CASE(parent, 3, var4)
#define _IMPLEMENT_BINDING_INDEXSET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_INDEXSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXSET_CASE(parent, 4, var5)
#define _IMPLEMENT_BINDING_INDEXSET(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_INDEXSET5, \
               _IMPLEMENT_BINDING_INDEXSET4, \
               _IMPLEMENT_BINDING_INDEXSET3, \
               _IMPLEMENT_BINDING_INDEXSET2, \
               _IMPLEMENT_BINDING_INDEXSET1)(parent, __VA_ARGS__)

// Cases for getMember(key)
#define _IMPLEMENT_BINDING_KEYGET_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return const_cast<_GET_MEMBER_TYPE(var)*>(_GET_MEMBER(var)); }
#define _IMPLEMENT_BINDING_KEYGET1(var1) \
  _IMPLEMENT_BINDING_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDING_KEYGET2(var1, var2) \
  _IMPLEMENT_BINDING_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDING_KEYGET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDING_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYGET2(var1, var2) \
  _IMPLEMENT_BINDING_KEYGET_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_BINDING_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYGET_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDING_KEYGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYGET_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDING_KEYGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_KEYGET5, \
               _IMPLEMENT_BINDING_KEYGET4, \
               _IMPLEMENT_BINDING_KEYGET3, \
               _IMPLEMENT_BINDING_KEYGET2, \
               _IMPLEMENT_BINDING_KEYGET1)(__VA_ARGS__)

// Cases for getMember(index)
#define _IMPLEMENT_BINDING_INDEXGET_CASE(parent, varIndex, var) \
  if (index == parent::getMemberCount() + varIndex) { return const_cast<_GET_MEMBER_TYPE(var)*>(_GET_MEMBER(var)); }
#define _IMPLEMENT_BINDING_INDEXGET1(parent, var1) \
  _IMPLEMENT_BINDING_INDEXGET_CASE(parent, 0, var1)
#define _IMPLEMENT_BINDING_INDEXGET2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXGET_CASE(parent, 0, var1) \
  _IMPLEMENT_BINDING_INDEXGET_CASE(parent, 1, var2)
#define _IMPLEMENT_BINDING_INDEXGET3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXGET2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXGET_CASE(parent, 2, var3)
#define _IMPLEMENT_BINDING_INDEXGET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXGET3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXGET_CASE(parent, 3, var4)
#define _IMPLEMENT_BINDING_INDEXGET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_INDEXGET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXGET_CASE(parent, 4, var5)
#define _IMPLEMENT_BINDING_INDEXGET(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_INDEXGET5, \
               _IMPLEMENT_BINDING_INDEXGET4, \
               _IMPLEMENT_BINDING_INDEXGET3, \
               _IMPLEMENT_BINDING_INDEXGET2, \
               _IMPLEMENT_BINDING_INDEXGET1)(parent, __VA_ARGS__)

// Cases for getMemberNeededType(key)
#define _IMPLEMENT_BINDING_KEYGETTYPE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_MEMBER_TYPE_INFO(var); }
#define _IMPLEMENT_BINDING_KEYGETTYPE1(var1) \
  _IMPLEMENT_BINDING_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDING_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_BINDING_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDING_KEYGETTYPE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDING_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_BINDING_KEYGETTYPE_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_BINDING_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYGETTYPE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDING_KEYGETTYPE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYGETTYPE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDING_KEYGETTYPE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_KEYGETTYPE5, \
               _IMPLEMENT_BINDING_KEYGETTYPE4, \
               _IMPLEMENT_BINDING_KEYGETTYPE3, \
               _IMPLEMENT_BINDING_KEYGETTYPE2, \
               _IMPLEMENT_BINDING_KEYGETTYPE1)(__VA_ARGS__)

// Cases for getMemberNeededType(index)
#define _IMPLEMENT_BINDING_INDEXGETTYPE_CASE(parent, varIndex, var) \
  if (index == parent::getMemberCount() + varIndex) { return _GET_MEMBER_TYPE_INFO(var); }
#define _IMPLEMENT_BINDING_INDEXGETTYPE1(parent, var1) \
  _IMPLEMENT_BINDING_INDEXGETTYPE_CASE(parent, 0, var1)
#define _IMPLEMENT_BINDING_INDEXGETTYPE2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXGETTYPE_CASE(parent, 0, var1) \
  _IMPLEMENT_BINDING_INDEXGETTYPE_CASE(parent, 1, var2)
#define _IMPLEMENT_BINDING_INDEXGETTYPE3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXGETTYPE2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXGETTYPE_CASE(parent, 2, var3)
#define _IMPLEMENT_BINDING_INDEXGETTYPE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXGETTYPE3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXGETTYPE_CASE(parent, 3, var4)
#define _IMPLEMENT_BINDING_INDEXGETTYPE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_INDEXGETTYPE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXGETTYPE_CASE(parent, 4, var5)
#define _IMPLEMENT_BINDING_INDEXGETTYPE(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_INDEXGETTYPE5, \
               _IMPLEMENT_BINDING_INDEXGETTYPE4, \
               _IMPLEMENT_BINDING_INDEXGETTYPE3, \
               _IMPLEMENT_BINDING_INDEXGETTYPE2, \
               _IMPLEMENT_BINDING_INDEXGETTYPE1)(parent, __VA_ARGS__)

// Cases for getMemberHoldMode(key)
#define _IMPLEMENT_BINDING_KEYGETHOLDMODE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_MEMBER_HOLDMODE(var); }
#define _IMPLEMENT_BINDING_KEYGETHOLDMODE1(var1) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_BINDING_KEYGETHOLDMODE2(var1, var2) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_BINDING_KEYGETHOLDMODE3(var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE2(var1, var2) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_BINDING_KEYGETHOLDMODE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE3(var1, var2, var3) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_BINDING_KEYGETHOLDMODE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE4(var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_KEYGETHOLDMODE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_BINDING_KEYGETHOLDMODE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_KEYGETHOLDMODE5, \
               _IMPLEMENT_BINDING_KEYGETHOLDMODE4, \
               _IMPLEMENT_BINDING_KEYGETHOLDMODE3, \
               _IMPLEMENT_BINDING_KEYGETHOLDMODE2, \
               _IMPLEMENT_BINDING_KEYGETHOLDMODE1)(__VA_ARGS__)

// Cases for getMemberHoldMode(index)
#define _IMPLEMENT_BINDING_INDEXGETHOLDMODE_CASE(parent, varIndex, var) \
  if (index == parent::getMemberCount() + varIndex) { return _GET_MEMBER_HOLDMODE(var); }
#define _IMPLEMENT_BINDING_INDEXGETHOLDMODE1(parent, var1) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE_CASE(parent, 0, var1)
#define _IMPLEMENT_BINDING_INDEXGETHOLDMODE2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE_CASE(parent, 0, var1) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE_CASE(parent, 1, var2)
#define _IMPLEMENT_BINDING_INDEXGETHOLDMODE3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE2(parent, var1, var2) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE_CASE(parent, 2, var3)
#define _IMPLEMENT_BINDING_INDEXGETHOLDMODE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE_CASE(parent, 3, var4)
#define _IMPLEMENT_BINDING_INDEXGETHOLDMODE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_INDEXGETHOLDMODE_CASE(parent, 4, var5)
#define _IMPLEMENT_BINDING_INDEXGETHOLDMODE(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_INDEXGETHOLDMODE5, \
               _IMPLEMENT_BINDING_INDEXGETHOLDMODE4, \
               _IMPLEMENT_BINDING_INDEXGETHOLDMODE3, \
               _IMPLEMENT_BINDING_INDEXGETHOLDMODE2, \
               _IMPLEMENT_BINDING_INDEXGETHOLDMODE1)(parent, __VA_ARGS__)

// Cases for getMemberKey
#define _IMPLEMENT_BINDING_GETKEY_CASE(parent, varIndex, varNameStr) \
  if (parent::getMemberCount() + varIndex == index) return SBSTR(varNameStr);
#define _IMPLEMENT_BINDING_GETKEY1(parent, var1) \
  _IMPLEMENT_BINDING_GETKEY_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_BINDING_GETKEY2(parent, var1, var2) \
  _IMPLEMENT_BINDING_GETKEY_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_BINDING_GETKEY_CASE(parent, 1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_BINDING_GETKEY3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_GETKEY2(parent, var1, var2) \
  _IMPLEMENT_BINDING_GETKEY_CASE(parent, 2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_BINDING_GETKEY4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_GETKEY3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_GETKEY_CASE(parent, 3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_BINDING_GETKEY5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_GETKEY4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_GETKEY_CASE(parent, 4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_BINDING_GETKEY(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_GETKEY5, \
               _IMPLEMENT_BINDING_GETKEY4, \
               _IMPLEMENT_BINDING_GETKEY3, \
               _IMPLEMENT_BINDING_GETKEY2, \
               _IMPLEMENT_BINDING_GETKEY1)(parent, __VA_ARGS__)

// Cases for findMemberIndex
#define _IMPLEMENT_BINDING_FINDINDEX_CASE(parent, varIndex, varNameStr) \
  if (SBSTR(key) == varNameStr) return parent::getMemberCount() + varIndex;
#define _IMPLEMENT_BINDING_FINDINDEX1(parent, var1) \
  _IMPLEMENT_BINDING_FINDINDEX_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_BINDING_FINDINDEX2(parent, var1, var2) \
  _IMPLEMENT_BINDING_FINDINDEX_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_BINDING_FINDINDEX_CASE(parent, 1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_BINDING_FINDINDEX3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_FINDINDEX2(parent, var1, var2) \
  _IMPLEMENT_BINDING_FINDINDEX_CASE(parent, 2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_BINDING_FINDINDEX4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_FINDINDEX3(parent, var1, var2, var3) \
  _IMPLEMENT_BINDING_FINDINDEX_CASE(parent, 3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_BINDING_FINDINDEX5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_BINDING_FINDINDEX4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_BINDING_FINDINDEX_CASE(parent, 4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_BINDING_FINDINDEX(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_BINDING_FINDINDEX5, \
               _IMPLEMENT_BINDING_FINDINDEX4, \
               _IMPLEMENT_BINDING_FINDINDEX3, \
               _IMPLEMENT_BINDING_FINDINDEX2, \
               _IMPLEMENT_BINDING_FINDINDEX1)(parent, __VA_ARGS__)

// Binding Implementation Macro
#define IMPLEMENT_BINDING(parent, ...) \
  public: using Binding::setMember; \
  public: virtual Int setMember(Char const *key, TiObject *val) \
  { \
    _IMPLEMENT_BINDING_KEYSET(parent, __VA_ARGS__); \
    return parent::setMember(key, val); \
  } \
  public: virtual void setMember(Int index, TiObject *val) \
  { \
    _IMPLEMENT_BINDING_INDEXSET(parent, __VA_ARGS__); \
    parent::setMember(index, val); \
  } \
  public: virtual Word getMemberCount() const \
  { \
    return SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, 5, 4, 3, 2, 1) + parent::getMemberCount(); \
  } \
  public: using Binding::getMember; \
  public: virtual TiObject* getMember(Char const *key) const \
  { \
    _IMPLEMENT_BINDING_KEYGET(__VA_ARGS__); \
    return parent::getMember(key); \
  } \
  public: virtual TiObject* getMember(Int index) const \
  { \
    _IMPLEMENT_BINDING_INDEXGET(parent, __VA_ARGS__); \
    return parent::getMember(index); \
  } \
  public: virtual TypeInfo* getMemberNeededType(Char const *key) const \
  { \
    _IMPLEMENT_BINDING_KEYGETTYPE(__VA_ARGS__); \
    return parent::getMemberNeededType(key); \
  } \
  public: virtual TypeInfo* getMemberNeededType(Int index) const \
  { \
    _IMPLEMENT_BINDING_INDEXGETTYPE(parent, __VA_ARGS__); \
    return parent::getMemberNeededType(index); \
  } \
  public: virtual HoldMode getMemberHoldMode(Char const *key) const \
  { \
    _IMPLEMENT_BINDING_KEYGETHOLDMODE(__VA_ARGS__); \
    return parent::getMemberHoldMode(key); \
  } \
  public: virtual HoldMode getMemberHoldMode(Int index) const \
  { \
    _IMPLEMENT_BINDING_INDEXGETHOLDMODE(parent, __VA_ARGS__); \
    return parent::getMemberHoldMode(index); \
  } \
  public: virtual const SbStr& getMemberKey(Int index) const \
  { \
    _IMPLEMENT_BINDING_GETKEY(parent, __VA_ARGS__); \
    return parent::getMemberKey(index); \
  } \
  public: virtual Int findMemberIndex(Char const *key) const \
  { \
    if (key == 0) { \
      throw EXCEPTION(InvalidArgumentException, S("key"), S("key is null")); \
    } \
    _IMPLEMENT_BINDING_FINDINDEX(parent, __VA_ARGS__); \
    return parent::findMemberIndex(key); \
  }

} // namespace

#endif

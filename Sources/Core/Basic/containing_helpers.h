/**
 * @file Core/Basic/containing_helpers.h
 * Contains MapContaining related helper definitions.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_CONTAININGHELPERS_H
#define CORE_BASIC_CONTAININGHELPERS_H

namespace Core { namespace Basic
{

//==============================================================================
// Macros

// Set a single element
#define _SET_ELEMENT(var) \
  auto value = ti_cast<VARTYPE_FROM_TUPLE var>(val); \
  if (value == 0 && val != 0) { \
    Str msg = S("Invalid argument type for element `"); \
    msg += VARNAMESTR_FROM_TUPLE var; \
    msg += S("`"); \
    throw EXCEPTION(InvalidArgumentException, S("val"), msg.c_str(), val->getMyTypeInfo()->getUniqueName()); \
  } \
  VARSETTER_FROM_TUPLE var

// Get a single element
#define _GET_ELEMENT(var) \
  ti_cast<TiObject>(const_cast<_GET_ELEMENT_TYPE(var)*>(VARGETTER_FROM_TUPLE var))

// Get the type of a element
#define _GET_ELEMENT_TYPE(var) \
  VARTYPE_FROM_TUPLE var

// Get the type info of a element
#define _GET_ELEMENT_TYPE_INFO(var) \
  VARTYPE_FROM_TUPLE var::getTypeInfo()

// Get the hold method of a element
#define _GET_ELEMENT_HOLDMODE(var) \
  HoldMode::VARHOLDMODE_FROM_TUPLE var

// Cases for setElement(key)
#define _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { \
    _SET_ELEMENT(var); return varIndex + parent::getElementCount(); \
  }
#define _IMPLEMENT_MAP_CONTAINING_KEYSET1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_MAP_CONTAINING_KEYSET2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, 1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_MAP_CONTAINING_KEYSET3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, 2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_MAP_CONTAINING_KEYSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, 3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_MAP_CONTAINING_KEYSET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, 4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_MAP_CONTAINING_KEYSET6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYSET_CASE(parent, 5, VARNAMESTR_FROM_TUPLE var6, var6)
#define _IMPLEMENT_MAP_CONTAINING_KEYSET(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_KEYSET6, \
               _IMPLEMENT_MAP_CONTAINING_KEYSET5, \
               _IMPLEMENT_MAP_CONTAINING_KEYSET4, \
               _IMPLEMENT_MAP_CONTAINING_KEYSET3, \
               _IMPLEMENT_MAP_CONTAINING_KEYSET2, \
               _IMPLEMENT_MAP_CONTAINING_KEYSET1)(parent, __VA_ARGS__)

// Cases for setElement(index)
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, varIndex, var) \
  if (index == parent::getElementCount() + varIndex) { _SET_ELEMENT(var); return; }
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, 0, var1)
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, 0, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, 1, var2)
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, 2, var3)
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, 3, var4)
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, 4, var5)
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXSET_CASE(parent, 5, var6)
#define _IMPLEMENT_MAP_CONTAINING_INDEXSET(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_INDEXSET6, \
               _IMPLEMENT_MAP_CONTAINING_INDEXSET5, \
               _IMPLEMENT_MAP_CONTAINING_INDEXSET4, \
               _IMPLEMENT_MAP_CONTAINING_INDEXSET3, \
               _IMPLEMENT_MAP_CONTAINING_INDEXSET2, \
               _IMPLEMENT_MAP_CONTAINING_INDEXSET1)(parent, __VA_ARGS__)

// Cases for getElement(key)
#define _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_ELEMENT(var); }
#define _IMPLEMENT_MAP_CONTAINING_KEYGET1(var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_MAP_CONTAINING_KEYGET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_MAP_CONTAINING_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_MAP_CONTAINING_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_MAP_CONTAINING_KEYGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_MAP_CONTAINING_KEYGET6(var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYGET_CASE(5, VARNAMESTR_FROM_TUPLE var6, var6)
#define _IMPLEMENT_MAP_CONTAINING_KEYGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_KEYGET6, \
               _IMPLEMENT_MAP_CONTAINING_KEYGET5, \
               _IMPLEMENT_MAP_CONTAINING_KEYGET4, \
               _IMPLEMENT_MAP_CONTAINING_KEYGET3, \
               _IMPLEMENT_MAP_CONTAINING_KEYGET2, \
               _IMPLEMENT_MAP_CONTAINING_KEYGET1)(__VA_ARGS__)

// Cases for getElement(index)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, varIndex, var) \
  if (index == parent::getElementCount() + varIndex) { return _GET_ELEMENT(var); }
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, 0, var1)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, 0, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, 1, var2)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, 2, var3)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, 3, var4)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, 4, var5)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGET_CASE(parent, 5, var6)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGET(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGET6, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGET5, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGET4, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGET3, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGET2, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGET1)(parent, __VA_ARGS__)

// Cases for getElementNeededType(key)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_ELEMENT_TYPE_INFO(var); }
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE1(var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE6(var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE_CASE(5, VARNAMESTR_FROM_TUPLE var6, var6)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE6, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE5, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE4, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE3, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE2, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE1)(__VA_ARGS__)

// Cases for getElementNeededType(index)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, varIndex, var) \
  if (index == parent::getElementCount() + varIndex) { return _GET_ELEMENT_TYPE_INFO(var); }
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, 0, var1)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, 0, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, 1, var2)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, 2, var3)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, 3, var4)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, 4, var5)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE_CASE(parent, 5, var6)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE6, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE5, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE4, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE3, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE2, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE1)(parent, __VA_ARGS__)

// Cases for getElementHoldMode(key)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { return _GET_ELEMENT_HOLDMODE(var); }
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE1(var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE6(var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE_CASE(5, VARNAMESTR_FROM_TUPLE var6, var6)
#define _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE6, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE5, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE4, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE3, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE2, \
               _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE1)(__VA_ARGS__)

// Cases for getElementHoldMode(index)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, varIndex, var) \
  if (index == parent::getElementCount() + varIndex) { return _GET_ELEMENT_HOLDMODE(var); }
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, 0, var1)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, 0, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, 1, var2)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, 2, var3)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, 3, var4)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, 4, var5)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE_CASE(parent, 5, var6)
#define _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE6, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE5, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE4, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE3, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE2, \
               _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE1)(parent, __VA_ARGS__)

// Cases for getElementKey
#define _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, varIndex, varNameStr) \
  if (varIndex + parent::getElementCount() == index) return SBSTR(varNameStr);
#define _IMPLEMENT_MAP_CONTAINING_GETKEY1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_MAP_CONTAINING_GETKEY2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, 1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_MAP_CONTAINING_GETKEY3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, 2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_MAP_CONTAINING_GETKEY4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, 3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_MAP_CONTAINING_GETKEY5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, 4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_MAP_CONTAINING_GETKEY6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY5(parent, var1, var2, var3, var5, var5) \
  _IMPLEMENT_MAP_CONTAINING_GETKEY_CASE(parent, 5, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_MAP_CONTAINING_GETKEY(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_GETKEY6, \
               _IMPLEMENT_MAP_CONTAINING_GETKEY5, \
               _IMPLEMENT_MAP_CONTAINING_GETKEY4, \
               _IMPLEMENT_MAP_CONTAINING_GETKEY3, \
               _IMPLEMENT_MAP_CONTAINING_GETKEY2, \
               _IMPLEMENT_MAP_CONTAINING_GETKEY1)(parent, __VA_ARGS__)

// Cases for findElementIndex
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, varIndex, varNameStr) \
  if (SBSTR(key) == varNameStr) return varIndex + parent::getElementCount();
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1)
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, 0, VARNAMESTR_FROM_TUPLE var1) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, 1, VARNAMESTR_FROM_TUPLE var2)
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, 2, VARNAMESTR_FROM_TUPLE var3)
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, 3, VARNAMESTR_FROM_TUPLE var4)
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, 4, VARNAMESTR_FROM_TUPLE var5)
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_FINDINDEX_CASE(parent, 5, VARNAMESTR_FROM_TUPLE var6)
#define _IMPLEMENT_MAP_CONTAINING_FINDINDEX(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_FINDINDEX6, \
               _IMPLEMENT_MAP_CONTAINING_FINDINDEX5, \
               _IMPLEMENT_MAP_CONTAINING_FINDINDEX4, \
               _IMPLEMENT_MAP_CONTAINING_FINDINDEX3, \
               _IMPLEMENT_MAP_CONTAINING_FINDINDEX2, \
               _IMPLEMENT_MAP_CONTAINING_FINDINDEX1)(parent, __VA_ARGS__)

// MapContaining Implementation Macro
#define IMPLEMENT_MAP_CONTAINING(parent, ...) \
  public: using MapContaining::setElement; \
  public: virtual Int setElement(Char const *key, TiObject *val) \
  { \
    _IMPLEMENT_MAP_CONTAINING_KEYSET(parent, __VA_ARGS__); \
    return parent::setElement(key, val); \
  } \
  public: virtual void setElement(Int index, TiObject *val) \
  { \
    _IMPLEMENT_MAP_CONTAINING_INDEXSET(parent, __VA_ARGS__); \
    parent::setElement(index, val); \
  } \
  public: virtual Word getElementCount() const \
  { \
    return SELECT_MACRO(__VA_ARGS__, _, _, _, _, 6, 5, 4, 3, 2, 1) + parent::getElementCount(); \
  } \
  public: using MapContaining::getElement; \
  public: virtual TiObject* getElement(Char const *key) const \
  { \
    _IMPLEMENT_MAP_CONTAINING_KEYGET(__VA_ARGS__); \
    return parent::getElement(key); \
  } \
  public: virtual TiObject* getElement(Int index) const \
  { \
    _IMPLEMENT_MAP_CONTAINING_INDEXGET(parent, __VA_ARGS__); \
    return parent::getElement(index); \
  } \
  public: virtual TypeInfo* getElementNeededType(Char const *key) const \
  { \
    _IMPLEMENT_MAP_CONTAINING_KEYGETTYPE(__VA_ARGS__); \
    return parent::getElementNeededType(key); \
  } \
  public: virtual TypeInfo* getElementNeededType(Int index) const \
  { \
    _IMPLEMENT_MAP_CONTAINING_INDEXGETTYPE(parent, __VA_ARGS__); \
    return parent::getElementNeededType(index); \
  } \
  public: virtual HoldMode getElementHoldMode(Char const *key) const \
  { \
    _IMPLEMENT_MAP_CONTAINING_KEYGETHOLDMODE(__VA_ARGS__); \
    return parent::getElementHoldMode(key); \
  } \
  public: virtual HoldMode getElementHoldMode(Int index) const \
  { \
    _IMPLEMENT_MAP_CONTAINING_INDEXGETHOLDMODE(parent, __VA_ARGS__); \
    return parent::getElementHoldMode(index); \
  } \
  public: virtual const SbStr getElementKey(Int index) const \
  { \
    _IMPLEMENT_MAP_CONTAINING_GETKEY(parent, __VA_ARGS__); \
    return parent::getElementKey(index); \
  } \
  public: virtual Int findElementIndex(Char const *key) const \
  { \
    if (key == 0) { \
      throw EXCEPTION(InvalidArgumentException, S("key"), S("key is null")); \
    } \
    _IMPLEMENT_MAP_CONTAINING_FINDINDEX(parent, __VA_ARGS__); \
    return parent::findElementIndex(key); \
  }

} } // namespace

#endif

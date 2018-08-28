/**
 * @file Core/Basic/containing_helpers.h
 * Contains MapContaining related helper definitions.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

// Remove a single element
#define _REMOVE_ELEMENT(var) \
  auto value = (VARTYPE_FROM_TUPLE var*)0; VARSETTER_FROM_TUPLE var

// Get a single element
#define _GET_ELEMENT(var) \
  ti_cast<TiObject>(const_cast<_GET_ELEMENT_TYPE(var)*>(VARGETTER_FROM_TUPLE var))

// Get the type of a element
#define _GET_ELEMENT_TYPE(var) \
  VARTYPE_FROM_TUPLE var

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

// Cases for removeElement(key)
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(varIndex, varNameStr, var) \
  if (SBSTR(key) == varNameStr) { \
    _REMOVE_ELEMENT(var); return varIndex; \
  }
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE1(var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1)
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(0, VARNAMESTR_FROM_TUPLE var1, var1) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(1, VARNAMESTR_FROM_TUPLE var2, var2)
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(2, VARNAMESTR_FROM_TUPLE var3, var3)
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(3, VARNAMESTR_FROM_TUPLE var4, var4)
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(4, VARNAMESTR_FROM_TUPLE var5, var5)
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE6(var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_KEYREMOVE_CASE(5, VARNAMESTR_FROM_TUPLE var6, var6)
#define _IMPLEMENT_MAP_CONTAINING_KEYREMOVE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_KEYREMOVE6, \
               _IMPLEMENT_MAP_CONTAINING_KEYREMOVE5, \
               _IMPLEMENT_MAP_CONTAINING_KEYREMOVE4, \
               _IMPLEMENT_MAP_CONTAINING_KEYREMOVE3, \
               _IMPLEMENT_MAP_CONTAINING_KEYREMOVE2, \
               _IMPLEMENT_MAP_CONTAINING_KEYREMOVE1)(__VA_ARGS__)

// Cases for removeElement(index)
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, varIndex, var) \
  if (index == parent::getElementCount() + varIndex) { _REMOVE_ELEMENT(var); return; }
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE1(parent, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, 0, var1)
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, 0, var1) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, 1, var2)
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE2(parent, var1, var2) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, 2, var3)
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE3(parent, var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, 3, var4)
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE4(parent, var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, 4, var5)
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE6(parent, var1, var2, var3, var4, var5, var6) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE5(parent, var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE_CASE(parent, 5, var6)
#define _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE(parent, ...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE6, \
               _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE5, \
               _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE4, \
               _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE3, \
               _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE2, \
               _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE1)(parent, __VA_ARGS__)

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
  public: virtual Int removeElement(Char const *key) \
  { \
    _IMPLEMENT_MAP_CONTAINING_KEYREMOVE(__VA_ARGS__); \
    return parent::removeElement(key); \
  } \
  public: virtual void removeElement(Int index) \
  { \
    _IMPLEMENT_MAP_CONTAINING_INDEXREMOVE(parent, __VA_ARGS__); \
    parent::removeElement(index); \
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
  public: virtual const SbStr& getElementKey(Int index) const \
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

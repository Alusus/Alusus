/**
 * @file Core/Data/data.h
 * Contains container related helper definitions.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CONTAINERHELPERS_H
#define CORE_DATA_CONTAINERHELPERS_H

#define _VARTYPE(varType, varName) varType
#define _VARNAME(varType, varName) varName
#define _VARNAMESTR(varType, varName) #varName

// Cases for set(index)
#define _IMPLEMENT_MAP_CONTAINER_INDEXSET_CASE(varIndex, varType, varName) \
  if (index == varIndex) { UPDATE_OWNED_SHAREDPTR(this->varName, getSharedPtr(ti_cast<varType>(val), true)); return; }
#define _IMPLEMENT_MAP_CONTAINER_INDEXSET1(var1) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET_CASE(0, _VARTYPE var1, _VARNAME var1)
#define _IMPLEMENT_MAP_CONTAINER_INDEXSET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET_CASE(0, _VARTYPE var1, _VARNAME var1) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET_CASE(1, _VARTYPE var2, _VARNAME var2)
#define _IMPLEMENT_MAP_CONTAINER_INDEXSET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET_CASE(2, _VARTYPE var3, _VARNAME var3)
#define _IMPLEMENT_MAP_CONTAINER_INDEXSET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET_CASE(3, _VARTYPE var4, _VARNAME var4)
#define _IMPLEMENT_MAP_CONTAINER_INDEXSET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_INDEXSET_CASE(4, _VARTYPE var5, _VARNAME var5)
#define _IMPLEMENT_MAP_CONTAINER_INDEXSET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_INDEXSET5, \
               _IMPLEMENT_MAP_CONTAINER_INDEXSET4, \
               _IMPLEMENT_MAP_CONTAINER_INDEXSET3, \
               _IMPLEMENT_MAP_CONTAINER_INDEXSET2, \
               _IMPLEMENT_MAP_CONTAINER_INDEXSET1)(__VA_ARGS__)

// Cases for set(key)
#define _IMPLEMENT_MAP_CONTAINER_KEYSET_CASE(varIndex, varType, varName, varNameStr) \
  if (SBSTR(key) == varNameStr) { \
    UPDATE_OWNED_SHAREDPTR(this->varName, getSharedPtr(ti_cast<varType>(val), true)); return varIndex; \
  }
#define _IMPLEMENT_MAP_CONTAINER_KEYSET1(var1) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET_CASE(0, _VARTYPE var1, _VARNAME var1, _VARNAMESTR var1)
#define _IMPLEMENT_MAP_CONTAINER_KEYSET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET_CASE(0, _VARTYPE var1, _VARNAME var1, _VARNAMESTR var1) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET_CASE(1, _VARTYPE var2, _VARNAME var2, _VARNAMESTR var2)
#define _IMPLEMENT_MAP_CONTAINER_KEYSET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET_CASE(2, _VARTYPE var3, _VARNAME var3, _VARNAMESTR var3)
#define _IMPLEMENT_MAP_CONTAINER_KEYSET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET_CASE(3, _VARTYPE var4, _VARNAME var4, _VARNAMESTR var4)
#define _IMPLEMENT_MAP_CONTAINER_KEYSET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_KEYSET_CASE(4, _VARTYPE var5, _VARNAME var5, _VARNAMESTR var5)
#define _IMPLEMENT_MAP_CONTAINER_KEYSET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_KEYSET5, \
               _IMPLEMENT_MAP_CONTAINER_KEYSET4, \
               _IMPLEMENT_MAP_CONTAINER_KEYSET3, \
               _IMPLEMENT_MAP_CONTAINER_KEYSET2, \
               _IMPLEMENT_MAP_CONTAINER_KEYSET1)(__VA_ARGS__)

// Cases for remove(index)
#define _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE_CASE(varIndex, varType, varName) \
  if (index == varIndex) { this->varName = SharedPtr<varType>(); return; }
#define _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE1(var1) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE_CASE(0, _VARTYPE var1, _VARNAME var1)
#define _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE_CASE(0, _VARTYPE var1, _VARNAME var1) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE_CASE(1, _VARTYPE var2, _VARNAME var2)
#define _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE_CASE(2, _VARTYPE var3, _VARNAME var3)
#define _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE_CASE(3, _VARTYPE var4, _VARNAME var4)
#define _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE_CASE(4, _VARTYPE var5, _VARNAME var5)
#define _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE5, \
               _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE4, \
               _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE3, \
               _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE2, \
               _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE1)(__VA_ARGS__)

// Cases for remove(key)
#define _IMPLEMENT_MAP_CONTAINER_KEYREMOVE_CASE(varIndex, varType, varName, varNameStr) \
  if (SBSTR(key) == varNameStr) { this->varName = SharedPtr<varType>(); return; }
#define _IMPLEMENT_MAP_CONTAINER_KEYREMOVE1(var1) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE_CASE(0, _VARTYPE var1, _VARNAME var1, _VARNAMESTR var1)
#define _IMPLEMENT_MAP_CONTAINER_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE_CASE(0, _VARTYPE var1, _VARNAME var1, _VARNAMESTR var1) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE_CASE(1, _VARTYPE var2, _VARNAME var2, _VARNAMESTR var2)
#define _IMPLEMENT_MAP_CONTAINER_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE_CASE(2, _VARTYPE var3, _VARNAME var3, _VARNAMESTR var3)
#define _IMPLEMENT_MAP_CONTAINER_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE_CASE(3, _VARTYPE var4, _VARNAME var4, _VARNAMESTR var4)
#define _IMPLEMENT_MAP_CONTAINER_KEYREMOVE5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_KEYREMOVE_CASE(4, _VARTYPE var5, _VARNAME var5, _VARNAMESTR var5)
#define _IMPLEMENT_MAP_CONTAINER_KEYREMOVE(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_KEYREMOVE5, \
               _IMPLEMENT_MAP_CONTAINER_KEYREMOVE4, \
               _IMPLEMENT_MAP_CONTAINER_KEYREMOVE3, \
               _IMPLEMENT_MAP_CONTAINER_KEYREMOVE2, \
               _IMPLEMENT_MAP_CONTAINER_KEYREMOVE1)(__VA_ARGS__)

// Cases for get(index)
#define _IMPLEMENT_MAP_CONTAINER_INDEXGET_CASE(varIndex, varType, varName) \
  if (index == varIndex) return this->varName.ti_cast_get<TiObject>();
#define _IMPLEMENT_MAP_CONTAINER_INDEXGET1(var1) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET_CASE(0, _VARTYPE var1, _VARNAME var1)
#define _IMPLEMENT_MAP_CONTAINER_INDEXGET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET_CASE(0, _VARTYPE var1, _VARNAME var1) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET_CASE(1, _VARTYPE var2, _VARNAME var2)
#define _IMPLEMENT_MAP_CONTAINER_INDEXGET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET_CASE(2, _VARTYPE var3, _VARNAME var3)
#define _IMPLEMENT_MAP_CONTAINER_INDEXGET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET_CASE(3, _VARTYPE var4, _VARNAME var4)
#define _IMPLEMENT_MAP_CONTAINER_INDEXGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_INDEXGET_CASE(4, _VARTYPE var5, _VARNAME var5)
#define _IMPLEMENT_MAP_CONTAINER_INDEXGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_INDEXGET5, \
               _IMPLEMENT_MAP_CONTAINER_INDEXGET4, \
               _IMPLEMENT_MAP_CONTAINER_INDEXGET3, \
               _IMPLEMENT_MAP_CONTAINER_INDEXGET2, \
               _IMPLEMENT_MAP_CONTAINER_INDEXGET1)(__VA_ARGS__)

// Cases for get(key)
#define _IMPLEMENT_MAP_CONTAINER_KEYGET_CASE(varIndex, varType, varName, varNameStr) \
  if (SBSTR(key) == varNameStr) return this->varName.ti_cast_get<TiObject>();
#define _IMPLEMENT_MAP_CONTAINER_KEYGET1(var1) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET_CASE(0, _VARTYPE var1, _VARNAME var1, _VARNAMESTR var1)
#define _IMPLEMENT_MAP_CONTAINER_KEYGET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET_CASE(0, _VARTYPE var1, _VARNAME var1, _VARNAMESTR var1) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET_CASE(1, _VARTYPE var2, _VARNAME var2, _VARNAMESTR var2)
#define _IMPLEMENT_MAP_CONTAINER_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET_CASE(2, _VARTYPE var3, _VARNAME var3, _VARNAMESTR var3)
#define _IMPLEMENT_MAP_CONTAINER_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET_CASE(3, _VARTYPE var4, _VARNAME var4, _VARNAMESTR var4)
#define _IMPLEMENT_MAP_CONTAINER_KEYGET5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_KEYGET_CASE(4, _VARTYPE var5, _VARNAME var5, _VARNAMESTR var5)
#define _IMPLEMENT_MAP_CONTAINER_KEYGET(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_KEYGET5, \
               _IMPLEMENT_MAP_CONTAINER_KEYGET4, \
               _IMPLEMENT_MAP_CONTAINER_KEYGET3, \
               _IMPLEMENT_MAP_CONTAINER_KEYGET2, \
               _IMPLEMENT_MAP_CONTAINER_KEYGET1)(__VA_ARGS__)

// Cases for getKey
#define _IMPLEMENT_MAP_CONTAINER_GETKEY_CASE(varIndex, varType, varNameStr) \
  if (varIndex == index) return SBSTR(varNameStr);
#define _IMPLEMENT_MAP_CONTAINER_GETKEY1(var1) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY_CASE(0, _VARTYPE var1, _VARNAMESTR var1)
#define _IMPLEMENT_MAP_CONTAINER_GETKEY2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY_CASE(0, _VARTYPE var1, _VARNAMESTR var1) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY_CASE(1, _VARTYPE var2, _VARNAMESTR var2)
#define _IMPLEMENT_MAP_CONTAINER_GETKEY3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY_CASE(2, _VARTYPE var3, _VARNAMESTR var3)
#define _IMPLEMENT_MAP_CONTAINER_GETKEY4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY_CASE(3, _VARTYPE var4, _VARNAMESTR var4)
#define _IMPLEMENT_MAP_CONTAINER_GETKEY5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_GETKEY_CASE(4, _VARTYPE var5, _VARNAMESTR var5)
#define _IMPLEMENT_MAP_CONTAINER_GETKEY(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_GETKEY5, \
               _IMPLEMENT_MAP_CONTAINER_GETKEY4, \
               _IMPLEMENT_MAP_CONTAINER_GETKEY3, \
               _IMPLEMENT_MAP_CONTAINER_GETKEY2, \
               _IMPLEMENT_MAP_CONTAINER_GETKEY1)(__VA_ARGS__)

// Cases for findIndex
#define _IMPLEMENT_MAP_CONTAINER_FINDINDEX_CASE(varIndex, varType, varNameStr) \
  if (SBSTR(key) == varNameStr) return varIndex;
#define _IMPLEMENT_MAP_CONTAINER_FINDINDEX1(var1) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX_CASE(0, _VARTYPE var1, _VARNAMESTR var1)
#define _IMPLEMENT_MAP_CONTAINER_FINDINDEX2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX_CASE(0, _VARTYPE var1, _VARNAMESTR var1) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX_CASE(1, _VARTYPE var2, _VARNAMESTR var2)
#define _IMPLEMENT_MAP_CONTAINER_FINDINDEX3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX2(var1, var2) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX_CASE(2, _VARTYPE var3, _VARNAMESTR var3)
#define _IMPLEMENT_MAP_CONTAINER_FINDINDEX4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX3(var1, var2, var3) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX_CASE(3, _VARTYPE var4, _VARNAMESTR var4)
#define _IMPLEMENT_MAP_CONTAINER_FINDINDEX5(var1, var2, var3, var4, var5) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX4(var1, var2, var3, var4) \
  _IMPLEMENT_MAP_CONTAINER_FINDINDEX_CASE(4, _VARTYPE var5, _VARNAMESTR var5)
#define _IMPLEMENT_MAP_CONTAINER_FINDINDEX(...) \
  SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, \
               _IMPLEMENT_MAP_CONTAINER_FINDINDEX5, \
               _IMPLEMENT_MAP_CONTAINER_FINDINDEX4, \
               _IMPLEMENT_MAP_CONTAINER_FINDINDEX3, \
               _IMPLEMENT_MAP_CONTAINER_FINDINDEX2, \
               _IMPLEMENT_MAP_CONTAINER_FINDINDEX1)(__VA_ARGS__)

// MapContainer Implementation Macro
#define IMPLEMENT_MAP_CONTAINER(...) \
  public: virtual void set(Int index, TiObject *val) \
  { \
    _IMPLEMENT_MAP_CONTAINER_INDEXSET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual Int set(Char const *key, TiObject *val) \
  { \
    _IMPLEMENT_MAP_CONTAINER_KEYSET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual void remove(Int index) \
  { \
    _IMPLEMENT_MAP_CONTAINER_INDEXREMOVE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual void remove(Char const *key) \
  { \
    _IMPLEMENT_MAP_CONTAINER_KEYREMOVE(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual Word getCount() const \
  { \
    return SELECT_MACRO(__VA_ARGS__, _, _, _, _, _, 5, 4, 3, 2, 1); \
  } \
  public: virtual TiObject* get(Int index) const \
  { \
    _IMPLEMENT_MAP_CONTAINER_INDEXGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual TiObject* get(Char const *key) const \
  { \
    _IMPLEMENT_MAP_CONTAINER_KEYGET(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("key"), STR("Key not found"), key); \
  } \
  public: virtual const SbStr& getKey(Int index) const \
  { \
    _IMPLEMENT_MAP_CONTAINER_GETKEY(__VA_ARGS__); \
    throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Out of range"), index); \
  } \
  public: virtual Int findIndex(Char const *key) const \
  { \
    if (key == 0) { \
      throw EXCEPTION(InvalidArgumentException, STR("key"), STR("key is null")); \
    } \
    _IMPLEMENT_MAP_CONTAINER_FINDINDEX(__VA_ARGS__); \
    return -1; \
  }

#endif

/**
 * @file Core/interop.cpp
 * Contains functions for Alusus-Cpp interoperability.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"

//==============================================================================
// Misc Functions

DL_EXPORTED void* getFromGlobalStorage(Char const *name)
{
  return GLOBAL_STORAGE->getObject(name);
}


//==============================================================================
// TiObject Functions

DL_EXPORTED ObjectTypeInfo const* TiObject_getMyTypeInfo(TiObject *self)
{
  return self->getMyTypeInfo();
}

DL_EXPORTED Bool TiObject_isDerivedFrom(TiObject *self, TypeInfo const *info)
{
  return self->isDerivedFrom(info);
}

DL_EXPORTED TiInterface* TiObject_getInterface(TiObject *self, TypeInfo const *info)
{
  return self->getInterface(info);
}


//==============================================================================
// TiInterface Functions

DL_EXPORTED InterfaceTypeInfo const* TiInterface_getMyInterfaceInfo(TiInterface *self)
{
  return self->getMyInterfaceInfo();
}

DL_EXPORTED TiObject* TiInterface_getTiObject(TiInterface *self)
{
  return self->getTiObject();
}


//==============================================================================
// Binding Functions

DL_EXPORTED void Binding_setMemberByName(Binding *self, Char const *name, TiObject *val)
{
  self->setMember(name, val);
}

DL_EXPORTED void Binding_setMemberByIndex(Binding *self, Int index, TiObject *val)
{
  self->setMember(index, val);
}

DL_EXPORTED Word Binding_getMemberCount(Binding *self)
{
  return self->getMemberCount();
}

DL_EXPORTED TiObject* Binding_getMemberByName(Binding *self, Char const *name)
{
  return self->getMember(name);
}

DL_EXPORTED TiObject* Binding_getMemberByIndex(Binding *self, Int index)
{
  return self->getMember(index);
}

DL_EXPORTED TypeInfo const* Binding_getMemberNeededTypeByName(Binding *self, Char const *name)
{
  return self->getMemberNeededType(name);
}

DL_EXPORTED TypeInfo const* Binding_getMemberNeededTypeByIndex(Binding *self, Int index)
{
  return self->getMemberNeededType(index);
}

DL_EXPORTED Srl::String Binding_getMemberKey(Binding *self, Int index)
{
  return Srl::String(true, self->getMemberKey(index).getBuf());
}

DL_EXPORTED Int Binding_findMemberIndex(Binding *self, Char const *key)
{
  return self->findMemberIndex(key);
}


//==============================================================================
// Containing<TiObject> Functions

DL_EXPORTED void Containing_setElement(Containing<TiObject> *self, Int index, TiObject *val)
{
  self->setElement(index, val);
}

DL_EXPORTED Word Containing_getElementCount(Containing<TiObject> *self)
{
  return self->getElementCount();
}

DL_EXPORTED TiObject* Containing_getElement(Containing<TiObject> *self, Int index)
{
  return self->getElement(index);
}

DL_EXPORTED TypeInfo const* Containing_getElementNeededType(Containing<TiObject> *self, Int index)
{
  return self->getElementNeededType(index);
}


//==============================================================================
// DynamicContaining<TiObject> Functions

DL_EXPORTED Int DynamicContaining_addElement(DynamicContaining<TiObject> *self, TiObject *val)
{
  return self->addElement(val);
}

DL_EXPORTED void DynamicContaining_insertElement(DynamicContaining<TiObject> *self, Int index, TiObject *val)
{
  self->insertElement(index, val);
}

DL_EXPORTED void DynamicContaining_removeElement(DynamicContaining<TiObject> *self, Int index)
{
  self->removeElement(index);
}

DL_EXPORTED TypeInfo const* DynamicContaining_getElementsNeededType(DynamicContaining<TiObject> *self)
{
  return self->getElementsNeededType();
}


//==============================================================================
// MapContaining<TiObject> Functions

DL_EXPORTED Int MapContaining_setElement(MapContaining<TiObject> *self, Char const *key, TiObject *val)
{
  return self->setElement(key, val);
}

DL_EXPORTED TiObject* MapContaining_getElement(MapContaining<TiObject> *self, Char const *key)
{
  return self->getElement(key);
}

DL_EXPORTED TypeInfo const* MapContaining_getElementNeededType(MapContaining<TiObject> *self, Char const *key)
{
  return self->getElementNeededType(key);
}

DL_EXPORTED Srl::String MapContaining_getElementKey(MapContaining<TiObject> *self, Int index)
{
  return Srl::String(true, self->getElementKey(index).getBuf());
}

DL_EXPORTED Int MapContaining_findElementIndex(MapContaining<TiObject> *self, Char const *key)
{
  return self->findElementIndex(key);
}


//==============================================================================
// DynamicMapContaining<TiObject> Functions

DL_EXPORTED Int DynamicMapContaining_addElement(DynamicMapContaining<TiObject> *self, Char const *key, TiObject *val)
{
  return self->addElement(key, val);
}

DL_EXPORTED void DynamicMapContaining_insertElement(
  DynamicMapContaining<TiObject> *self, Int index, Char const *key, TiObject *val
) {
  self->insertElement(index, key, val);
}

DL_EXPORTED void DynamicMapContaining_removeElementByIndex(DynamicMapContaining<TiObject> *self, Int index)
{
  self->removeElement(index);
}

DL_EXPORTED void DynamicMapContaining_removeElementByKey(DynamicMapContaining<TiObject> *self, Char const *key)
{
  self->removeElement(key);
}

DL_EXPORTED TypeInfo const* DynamicMapContaining_getElementsNeededType(DynamicMapContaining<TiObject> *self)
{
  return self->getElementsNeededType();
}

/**
 * @file Core/interop.cpp
 * Contains functions for Alusus-Cpp interoperability.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

DL_EXPORTED void addLocalization(Char const *locale, Char const *key, Char const *value)
{
  Core::Notices::L18nDictionary::getSingleton()->addEntry(locale, key, value);
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
// Core::Ast::Node Functions

DL_EXPORTED SharedPtr<Core::Ast::Node> const& CoreAstNode_getMetadata(Core::Ast::Node *self, Char const *name)
{
  return self->getMetadata(name);
}

DL_EXPORTED void CoreAstNode_setMetadata(Core::Ast::Node *self, Char const *name, SharedPtr<Core::Ast::Node> const &value)
{
  self->setMetadata(name, value);
}

DL_EXPORTED void CoreAstNode_removeMetadata(Core::Ast::Node *self, Char const *name)
{
  self->removeMetadata(name);
}


//==============================================================================
// Containing Functions
//
// The containing interfaces are templates, so we need a set of functions for each content type. The name of each
// function is prefixed with the name of the content type, which is the same prefix used by the Alusus side for the
// exported names of these template classes.

#define DEFINE_CONTAINING_FUNCTIONS(prefix, CTYPE) \
  DL_EXPORTED void prefix##Containing_setElement(Containing<CTYPE> *self, Int index, CTYPE *val) \
  { \
    self->setElement(index, val); \
  } \
  DL_EXPORTED Word prefix##Containing_getElementCount(Containing<CTYPE> *self) \
  { \
    return self->getElementCount(); \
  } \
  DL_EXPORTED CTYPE* prefix##Containing_getElement(Containing<CTYPE> *self, Int index) \
  { \
    return self->getElement(index); \
  } \
  DL_EXPORTED TypeInfo const* prefix##Containing_getElementNeededType(Containing<CTYPE> *self, Int index) \
  { \
    return self->getElementNeededType(index); \
  } \
  DL_EXPORTED Int prefix##Containing_findElementIndex(Containing<CTYPE> *self, CTYPE *val) \
  { \
    return self->findElementIndex(val); \
  } \
  \
  DL_EXPORTED Int prefix##DynamicContaining_addElement(DynamicContaining<CTYPE> *self, CTYPE *val) \
  { \
    return self->addElement(val); \
  } \
  DL_EXPORTED void prefix##DynamicContaining_insertElement( \
    DynamicContaining<CTYPE> *self, Int index, CTYPE *val \
  ) { \
    self->insertElement(index, val); \
  } \
  DL_EXPORTED void prefix##DynamicContaining_removeElement(DynamicContaining<CTYPE> *self, Int index) \
  { \
    self->removeElement(index); \
  } \
  DL_EXPORTED TypeInfo const* prefix##DynamicContaining_getElementsNeededType(DynamicContaining<CTYPE> *self) \
  { \
    return self->getElementsNeededType(); \
  } \
  \
  DL_EXPORTED Int prefix##MapContaining_setElement(MapContaining<CTYPE> *self, Char const *key, CTYPE *val) \
  { \
    return self->setElement(key, val); \
  } \
  DL_EXPORTED CTYPE* prefix##MapContaining_getElement(MapContaining<CTYPE> *self, Char const *key) \
  { \
    return self->getElement(key); \
  } \
  DL_EXPORTED TypeInfo const* prefix##MapContaining_getElementNeededType( \
    MapContaining<CTYPE> *self, Char const *key \
  ) { \
    return self->getElementNeededType(key); \
  } \
  DL_EXPORTED Srl::String prefix##MapContaining_getElementKey(MapContaining<CTYPE> *self, Int index) \
  { \
    return Srl::String(true, self->getElementKey(index).getBuf()); \
  } \
  DL_EXPORTED Int prefix##MapContaining_findElementIndex(MapContaining<CTYPE> *self, Char const *key) \
  { \
    return self->findElementIndex(key); \
  } \
  \
  DL_EXPORTED Int prefix##DynamicMapContaining_addElement( \
    DynamicMapContaining<CTYPE> *self, Char const *key, CTYPE *val \
  ) { \
    return self->addElement(key, val); \
  } \
  DL_EXPORTED void prefix##DynamicMapContaining_insertElement( \
    DynamicMapContaining<CTYPE> *self, Int index, Char const *key, CTYPE *val \
  ) { \
    self->insertElement(index, key, val); \
  } \
  DL_EXPORTED void prefix##DynamicMapContaining_removeElementByIndex( \
    DynamicMapContaining<CTYPE> *self, Int index \
  ) { \
    self->removeElement(index); \
  } \
  DL_EXPORTED void prefix##DynamicMapContaining_removeElementByKey( \
    DynamicMapContaining<CTYPE> *self, Char const *key \
  ) { \
    self->removeElement(key); \
  } \
  DL_EXPORTED TypeInfo const* prefix##DynamicMapContaining_getElementsNeededType( \
    DynamicMapContaining<CTYPE> *self \
  ) { \
    return self->getElementsNeededType(); \
  }

DEFINE_CONTAINING_FUNCTIONS(CoreBasicTiObject_, TiObject)
DEFINE_CONTAINING_FUNCTIONS(CoreAstNode_, Core::Ast::Node)

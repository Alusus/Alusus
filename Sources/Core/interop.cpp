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

DL_EXPORTED void* getFromGlobalStorage(Char const *name)
{
  return GLOBAL_STORAGE->getObject(name);
}


DL_EXPORTED ObjectTypeInfo const* getObjectTypeInfo(TiObject *obj)
{
  return obj->getMyTypeInfo();
}


DL_EXPORTED Bool isObjectDerivedFrom(TiObject *obj, TypeInfo const *info)
{
  return obj->isDerivedFrom(info);
}


DL_EXPORTED TiInterface* getObjectInterface(TiObject *obj, TypeInfo const *info)
{
  return obj->getInterface(info);
}

/**
 * @file Core/Basic/TiInterface.cpp
 * Contains the implementation of class Core::Basic::TiInterface.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Functions

/**
 * Returns a pointer to the type information object associated with this
 * interface. This function (along with getMyInterfaceInfo) is automatically
 * defined by the TYPE_INFO macro.
 */
TypeInfo * TiInterface::getTypeInfo()
{
  static Core::Basic::TypeInfo *typeInfo = 0;
  if (typeInfo == 0) {
    Char const *myType = STR("TiInterface");
    Char const *typeNamespace = STR("Core.Basic");
    Char const *moduleName = STR("Core");
    Char const *url = STR("alusus.net");
    Char const *uniqueName = STR("alusus.net#Core#Core.Basic.TiInterface");
    typeInfo = reinterpret_cast<Core::Basic::TypeInfo*>(GLOBAL_STORAGE->getObject(uniqueName));
    if (typeInfo == 0) {
      typeInfo = new Core::Basic::TypeInfo(myType, typeNamespace, moduleName, url, 0);
      GLOBAL_STORAGE->setObject(uniqueName, reinterpret_cast<void*>(typeInfo));
    }
  }
  return typeInfo;
}


/**
 * Check if this interface is of the given type, or a derived type.
 *
 * @return Returns true if the given type info is for the interface from which
 *         this interface is instantiated, or for the interface from which this
 *         interface's class is derived, false otherwise.
 */
Bool TiInterface::isInterfaceDerivedFrom(TypeInfo * info) const
{
  TypeInfo * i = this->getMyInterfaceInfo();
  while (i != 0) {
    if (i == info) return true;
    i = i->getBaseTypeInfo();
  }
  return false;
}

} } // namespace

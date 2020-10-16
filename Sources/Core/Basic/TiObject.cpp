/**
 * @file Core/Basic/TiObject.cpp
 * Contains the implementation of class Core::Basic::TiObject.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "core.h"

namespace Core::Basic
{

//==============================================================================
// Member Functions

/**
 * Returns a pointer to the type information object associated with this
 * class. This function (along with getMyTypeInfo) is automatically
 * defined by the TYPE_INFO macro.
 */
ObjectTypeInfo const* TiObject::getTypeInfo()
{
  static ObjectTypeInfo const *type_info = 0;
  if (type_info == 0) {
    Char const *my_type = S("TiObject");
    Char const *typeNamespace = S("Core.Basic");
    Char const *moduleName = S("Core");
    Char const *url = S("alusus.org");
    Char const *uniqueName = S("alusus.org#Core#Core.Basic.TiObject");
    type_info = reinterpret_cast<ObjectTypeInfo const*>(GLOBAL_STORAGE->getObject(uniqueName));
    if (type_info == 0) {
      type_info = new ObjectTypeInfo(my_type, typeNamespace, moduleName, url, 0, 0);
      GLOBAL_STORAGE->setObject(uniqueName, const_cast<ObjectTypeInfo*>(type_info));
    }
  }
  return type_info;
}


/**
 * Check if this object is of the given type, or a derived type.
 *
 * @return Returns true if the given type info is for the class from which
 *         this object is instantiated, or for the class from which this
 *         object's class is derived, false otherwise.
 */
Bool TiObject::isDerivedFrom(TypeInfo const *info) const
{
  TypeInfo const *i = this->getMyTypeInfo();
  while (i != 0) {
    if (i == info) return true;
    i = i->getBaseTypeInfo();
  }
  return false;
}

} // namespace

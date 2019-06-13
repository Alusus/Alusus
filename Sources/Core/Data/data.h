/**
 * @file Core/Data/data.h
 * Contains the definitions and include statements of all types in the Data
 * namespace.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_DATA_H
#define CORE_DATA_DATA_H

/**
 * @defgroup core_data Data
 * @ingroup core
 * @brief Classes that contains definitions data.
 *
 * This namespace contains definitions for classes and types that are used to
 * hold the data used in definitions.
 */


//==============================================================================
// Forward Class Definitions

namespace Core::Data
{
  class Node;
  class Seeker;
  class SourceLocation;
}

namespace Core::Notices
{
  class Notice;
  class Store;
}


//==============================================================================
// Functions

namespace Core::Data
{

/**
 * Reset the indexes of all references in a specific range within a tree.
 * @ingroup core_data
 * Call the unsetIndexes method of DataHaving interface, if implemented by
 * the object.
 */
void unsetIndexes(TiObject *obj, Int from, Int to);

/**
 * @brief Find an object in the chain of owners with the given type.
 * @ingroup core_data
 * If the given object is of the given type, it will be returned.
 */
Node* findOwner(Node *obj, TypeInfo *typeInfo);

template <class T> T* findOwner(Node *obj)
{
  return static_cast<T*>(findOwner(obj, T::getTypeInfo()));
}

/**
 * @brief Print the given object to the given stream.
 * @ingroup core_data
 *
 * If the object implements the Printable interface, it will pass the call to
 * that interface, otherwise it will print the object type and the production
 * id if available.
 */
void dumpData(OutStream &stream, TiObject *ptr, int indents);

Word getSourceLocationRecordCount(SourceLocation const *sl);

// TODO: Find module for other dimensions.

} // namespace


//==============================================================================
// Macros

/**
 * @brief Set the owner of ptr to this if ptr is of type Node.
 * @ingroup core_data
 */
#define OWN_SHAREDPTR(ptr) \
  { \
    auto __ptr = (ptr).ti_cast_get<Core::Data::Node>(); \
    if (__ptr != 0) { __ptr->setOwner(this); } \
  }

/**
 * @brief A plain pointer version of OWN_SHAREDPTR.
 * @ingroup core_data
 * @sa OWN_SHAREDPTR()
 */
#define OWN_PLAINPTR(ptr) \
  { \
    auto __ptr = (ptr).ti_cast_get<Core::Data::Node>(); \
    if (__ptr != 0) { __ptr->setOwner(this); } \
  }

/**
 * @brief Reset the owner of ptr if ptr is of type Node.
 * @ingroup core_data
 */
#define DISOWN_SHAREDPTR(ptr) \
  { \
    auto __ptr = (ptr).ti_cast_get<Core::Data::Node>(); \
    if (__ptr != 0 && __ptr->getOwner() == this) { __ptr->setOwner(0); } \
  }

/**
 * @brief A plain pointer version of DIWOWN_SHAREDPTR.
 * @ingroup core_data
 * @sa DISOWN_SHAREDPTR()
 */
#define DISOWN_PLAINPTR(ptr) \
  { \
    auto __ptr = ti_cast<Core::Data::Node>(ptr); \
    if (__ptr != 0 && __ptr->getOwner() == this) { __ptr->setOwner(0); } \
  }

/**
 * @brief A macro to make it easy to set and update owned object pointers.
 * @ingroup core_data
 * This macro will first reset the owner of the current object before setting
 * the new object and setting the owner of the new object.
 */
#define UPDATE_OWNED_SHAREDPTR(ptr, val) \
  { \
    DISOWN_SHAREDPTR(ptr); \
    (ptr) = val; \
    OWN_SHAREDPTR(ptr); \
  }

/**
 * @brief A plain pointer version of UPDATED_OWNED_SHAREDPTR.
 * @ingroup core_data
 * @sa UPDATED_OWNED_SHARED_PTR()
 */
#define UPDATE_OWNED_PLAINPTR(ptr, val) \
  { \
    DISOWN_PLAINPTR(ptr); \
    (ptr) = val; \
    OWN_PLAINPTR(ptr); \
  }

/**
 * @brief A macro to simplify resetting a pointer to an owned object.
 * @ingroup core_data
 * This macro will first reset the owner of the object before resetting the
 * pointer.
 */
#define RESET_OWNED_SHAREDPTR(ptr) \
  {\
    DISOWN_SHAREDPTR(ptr); \
    (ptr).reset(); \
  }

/**
 * @brief A plain pointer version of RESET_OWNED_SHAREDPTR.
 * @ingroup core_data
 * @sa RESET_OWNED_SHAREDPTR()
 */
#define RESET_OWNED_PLAINPTR(ptr) \
  {\
    DISOWN_PLAINPTR(ptr); \
    (ptr) = 0; \
  }


//==============================================================================
// Classes

#include "Node.h"

// Helpers
#include "paired_pointers.h"
#include "IdGenerator.h"
#include "source_location.h"

// Generic Data Interfaces
#include "DataHaving.h"
#include "IdHaving.h"
#include "Clonable.h"
#include "Printable.h"

// Containers
#include "NbList.h"
#include "NbMap.h"
#include "VariableStack.h"
#include "DataStack.h"

// Other Classes
#include "Token.h"

// Grammar
#include "Grammar/grammar.h"

// Abstract Syntax Tree
#include "Ast/ast.h"

// Operations
#include "Seeker.h"

#endif

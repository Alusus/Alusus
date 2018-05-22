/**
 * @file Core/Data/data.h
 * Contains the definitions and include statements of all types in the Data
 * namespace.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
  class Module;
  class Provider;
  class Seeker;
  class SourceLocation;
}

namespace Core::Notices
{
  class Notice;
}


//==============================================================================
// Data Types

namespace Core::Data
{

/**
 * @brief An enumeration that specifies the usage criteria of references.
 * The usage criteria is used by references to determine caching criteria.
 * References can cache the indexes of referenced data to avoid repeating
 * the search. By knowing the usage criteria references will be able to
 * determine whether the indexes can be cached.
 *
 * SINGLE_DATA_SINGLE_MATCH: The reference will always be used against the same
 *                           data repository, and will only be used to match a
 *                           single target.
 * SINGLE_DATA_MULTI_MATCH: The reference will always be used against the same
 *                          data repository, but can match multiple targets.
 * MULTI_DATA: The reference can be used against different data repositories.
 */
// OBSOLETE:
s_enum(ReferenceUsageCriteria, SINGLE_DATA_SINGLE_MATCH, SINGLE_DATA_MULTI_MATCH, MULTI_DATA);

/**
 * @brief An enumeration for the list of operations for references.
 * References use lambdas to determine the operation to perform.and those
 * lambdas can return one of the values in this enumeration to tell the
 * Reference object what operation to perform.
 *
 * MOVE: Move to the next element matched by this reference.
 * STOP: Stop the matching and ignore any other possible matches.
 * PERFORM_AND_MOVE: Perform the requested operation on this match, then move
 *                   to the next match.
 * PERFORM_AND_STOP: Perform the requested operation on this match, then stop
 *                   and ignore other possible matches.
 */
// OBSOLETE:
s_enum(RefOp, MOVE, STOP, PERFORM_AND_MOVE, PERFORM_AND_STOP);

/**
 * @brief An enumeration for change operation of Containers' contents.
 * @ingroup core_data
 *
 * This enumeration is used to specifiy the type of the operation happneing on
 * Containers. This is used by containers' contentChangeNotifier.
 *
 * ADD: The operation is an addition of a new element.
 * UPDATE: The operation is an update of an existing element.
 * REMOVE: The operation is a removal of an existing element.
 */
// OBSOLETE:
s_enum(ContentChangeOp, ADD, UPDATE, REMOVE);

} // namespace


//==============================================================================
// Functions

namespace Core::Data
{

/**
 * A helper function to check whether 'op' is any of the perform values.
 * @ingroup core_data
 */
Bool isPerform(RefOp op);

/**
 * A helper function to check whether 'op' is any of the move values.
 * @ingroup core_data
 */
Bool isMove(RefOp op);

/**
 * Reset the indexes of all references in a specific range within a tree.
 * @ingroup core_data
 * Call the unsetIndexes method of DataOwner interface, if implemented by
 * the object.
 */
void unsetIndexes(TiObject *obj, Int from, Int to);

/**
 * @brief Set the IDs of all elements in a given tree.
 * @ingroup core_data
 * Sets the ID of the given object, and the IDs of any objects contained within
 * the given object if the given object is a container. The ids will be the
 * concatenated container keys that lead to the given object from the root.
 * @sa generateId()
 */
void setTreeIds(TiObject *obj, Node *root);

/**
 * @brief Set the IDs of all elements in a given tree.
 * @ingroup core_data
 * Sets the ID of the given object, and the IDs of any objects contained within
 * the given object if the given object is a container. The IDs of inner
 * objects will have the format: <id>.<childName>
 */
void setTreeIds(TiObject *obj, Node *root, const Char *id);

/**
 * @brief Generate an ID for the given object.
 * @ingroup core_data
 * This function will generate the ID by tracing the owners of the given object
 * all the way to the root and generate the ID from the concatenated keys that
 * leads to the given object.
 * @param obj The object for which to generate the ID.
 * @param id A string stream to hold the generated ID.
 */
void generateId(Node *obj, Node *root, StrStream &id);

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
// Function Signature Definitions

namespace Core::Data
{

typedef std::function<RefOp(Int index, TiObject *&obj)> ReferenceSetLambda;
typedef std::function<RefOp(Int index, TiObject *obj)> ReferenceRemoveLambda;
typedef std::function<RefOp(Int index, TiObject *obj)> ReferenceForeachLambda;

typedef std::function<RefOp(Int index, TiObject *&obj)> SeekerSetLambda;
typedef std::function<RefOp(Int index, TiObject *obj)> SeekerRemoveLambda;
typedef std::function<RefOp(Int index, TiObject *obj, TiObject *parent)> SeekerForeachLambda;

} // namespace


//==============================================================================
// Macros

/**
 * @brief Set the owner of ptr to this if ptr is of type Node.
 * @ingroup core_data
 */
#define OWN_SHAREDPTR(ptr) \
  { \
    auto __ptr = (ptr).ti_cast_get<Node>(); \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>()) { \
        __ptr->setOwner(this); \
    } \
  }

/**
 * @brief A plain pointer version of OWN_SHAREDPTR.
 * @ingroup core_data
 * @sa OWN_SHAREDPTR()
 */
#define OWN_PLAINPTR(ptr) \
  { \
    auto __ptr = ptr; \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>()) { \
        ti_cast<Node>(__ptr)->setOwner(this); \
    } \
  }

/**
 * @brief Reset the owner of ptr if ptr is of type Node.
 * @ingroup core_data
 */
#define DISOWN_SHAREDPTR(ptr) \
  { \
    auto __ptr = (ptr).ti_cast_get<Node>(); \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>() && __ptr->getOwner() == this) { \
        __ptr->setOwner(0); \
    } \
  }

/**
 * @brief A plain pointer version of DIWOWN_SHAREDPTR.
 * @ingroup core_data
 * @sa DISOWN_SHAREDPTR()
 */
#define DISOWN_PLAINPTR(ptr) \
  { \
    auto __ptr = ti_cast<Node>(ptr); \
    if (__ptr != 0 && __ptr->isDerivedFrom<Node>() && __ptr->getOwner() == this) { \
        __ptr->setOwner(0); \
    } \
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
#include "DataOwner.h"
#include "Initializable.h"
#include "IdHolder.h"
#include "Clonable.h"
#include "Printable.h"

// Containers
#include "NbList.h"
#include "NbMap.h"
#include "VariableStack.h"
#include "DataStack.h"

// Container Interfaces
#include "Container.h"
#include "ListContainer.h"
#include "NamedListContainer.h"
#include "MapContainer.h"

// Other Classes
#include "Token.h"

// Grammar
#include "Grammar/grammar.h"

// Abstract Syntax Tree
#include "Ast/ast.h"

// Operations
#include "Seeker.h"

#endif

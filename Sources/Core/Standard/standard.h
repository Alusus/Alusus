/**
 * @file Core/Standard/standard.h
 * Contains the definitions and include statements of all types in the Standard
 * namespace.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_STANDARD_STANDARD_H
#define CORE_STANDARD_STANDARD_H

namespace Core { namespace Standard
{

/**
 * @defgroup standard Standard
 * @ingroup core
 * @brief Classes related to the Core's standard grammar.
 *
 * This namespace contains definitions for standard grammar and commands and
 * other related classes.
 */

class LibraryGateway;
class RootManager;

//==============================================================================
// Types

/**
 * @brief A getter function for the library gateway.
 * @ingroup standard
 *
 * This type defines a pointer to a function that gets the singleton library
 * gateway that every dynamic library should implement.
 */
typedef LibraryGateway* (*LibraryGatewayGetter)();


//==============================================================================
// Constants

/**
 * @brief The name of the getter function for the library gateway.
 * @ingroup standard
 *
 * This function gets the singleton library gateway that every dynamic library
 * should implement.
 */
#define LIBRARY_GATEWAY_GETTER_NAME "get_library_gateway"

/**
 * @brief The definition of the getter function for the library gateway.
 * @ingroup standard
 *
 * This function gets the singleton library gateway that every dynamic library
 * should implement.
 */
#define LIBRARY_GATEWAY_GETTER_DEF Core::Standard::LibraryGateway* get_library_gateway()


//==============================================================================
// Functions

/**
 * @brief Gets the currently set working directory.
 * @ingroup standard
 *
 * This method is used to wrap the platform-specific implementation inside a
 * platform independent function.
 */
std::string getWorkingDirectory();

/**
 * @brief Gets the directory of the executable.
 * @ingroup standard
 *
 * This can be different from the working directory if the user launches the
 * executable from a path other than the exe's path.
 * This method is used to wrap the platform-specific implementation inside a
 * platform independent function.
 */
std::string getModuleDirectory();

/**
 * @brief Merge the values of two container trees.
 * @ingroup standard
 *
 * Merge a given container tree into another container tree. Merging is done by
 * setting all values in a source countainer into the destination container. If
 * the value being set is a container itself, the function will recursively call
 * itself. For map containers, the value will override any value with the same
 * key in the destination container. For list containers, the source values will
 * be appended at the end of the dest list. The function will handle plain
 * containers as well.
 */
Bool mergeContainers(TiObject *dest, TiObject *src, Processing::ParserState *state);

/**
 * @brief Merge the given object into the provided definition manager.
 * @ingroup standard
 *
 * If no object is already defined at the given qualifier, the new object will
 * be set. If an object of different type is already defined, it will be
 * overwritten after a build message is raised. If an object of the same type
 * is already defined and it supports a Container interface, the objects will
 * be merged. If the objects are the same type but does not support a
 * Container interface, the destination will be overwritten after a build msg
 * is created.
 */
void mergeDefinition(Char const *qualifier, TiObject *obj, Processing::ParserState *state);

} } // namespace


//==============================================================================
// Classes

#include "ImportLoadFailedMsg.h"
#include "StringLiteralTokenizingHandler.h"
// TODO:
// #include "NumberLiteralTokenizingHandler.h"
#include "ImportParsingHandler.h"

#include "RedefinitionMsg.h"

#include "GrammarPlant.h"
#include "LibraryGateway.h"
#include "LibraryManager.h"
#include "RootManager.h"

#endif

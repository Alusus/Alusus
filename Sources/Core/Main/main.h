/**
 * @file Core/Main/main.h
 * Contains the definitions and include statements of all types in the Main
 * namespace.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_MAIN_MAIN_H
#define CORE_MAIN_MAIN_H

namespace Core::Main
{

/**
 * @defgroup core_standard Main
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
 * @ingroup core_standard
 *
 * This type defines a pointer to a function that gets the singleton library
 * gateway that every dynamic library should implement.
 */
typedef LibraryGateway* (*LibraryGatewayGetter)();


//==============================================================================
// Constants

/**
 * @brief The name of the getter function for the library gateway.
 * @ingroup core_standard
 *
 * This function gets the singleton library gateway that every dynamic library
 * should implement.
 */
#define LIBRARY_GATEWAY_GETTER_NAME "get_library_gateway"

/**
 * @brief The definition of the getter function for the library gateway.
 * @ingroup core_standard
 *
 * This function gets the singleton library gateway that every dynamic library
 * should implement.
 */
#define LIBRARY_GATEWAY_GETTER_DEF Core::Main::LibraryGateway* get_library_gateway()


//==============================================================================
// Functions

/**
 * @brief Gets the currently set working directory.
 * @ingroup core_standard
 *
 * This method is used to wrap the platform-specific implementation inside a
 * platform independent function.
 */
Brl::String getWorkingDirectory();

/**
 * @brief Gets the directory of the executable.
 * @ingroup core_standard
 *
 * This can be different from the working directory if the user launches the
 * executable from a path other than the exe's path.
 * This method is used to wrap the platform-specific implementation inside a
 * platform independent function.
 */
Brl::String getModuleDirectory();

} // namespace


//==============================================================================
// Classes

#include "LibraryGateway.h"
#include "LibraryManager.h"
#include "RootScopeHandler.h"
#include "RootManager.h"

#endif

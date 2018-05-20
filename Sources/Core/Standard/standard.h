/**
 * @file Core/Standard/standard.h
 * Contains the definitions and include statements of all types in the Standard
 * namespace.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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
 * @defgroup core_standard Standard
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
#define LIBRARY_GATEWAY_GETTER_DEF Core::Standard::LibraryGateway* get_library_gateway()


//==============================================================================
// Functions

/**
 * @brief Gets the currently set working directory.
 * @ingroup core_standard
 *
 * This method is used to wrap the platform-specific implementation inside a
 * platform independent function.
 */
std::string getWorkingDirectory();

/**
 * @brief Gets the directory of the executable.
 * @ingroup core_standard
 *
 * This can be different from the working directory if the user launches the
 * executable from a path other than the exe's path.
 * This method is used to wrap the platform-specific implementation inside a
 * platform independent function.
 */
std::string getModuleDirectory();

} } // namespace


//==============================================================================
// Classes

#include "LibraryGateway.h"
#include "LibraryManager.h"
#include "RootManager.h"

#endif

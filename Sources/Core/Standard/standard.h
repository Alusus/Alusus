/**
 * @file Core/Standard/standard.h
 * Contains the definitions and include statements of all types in the Standard
 * namespace.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef STANDARD_STANDARD_H
#define STANDARD_STANDARD_H

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

/// Merge the values of two container trees.
Bool mergeContainers(IdentifiableObject *dest, IdentifiableObject *src, Processing::ParserState *state);

/// Merge the given object into the provided definition manager.
void mergeDefinition(Char const *qualifier, SharedPtr<IdentifiableObject> const &obj,
                     Processing::ParserState *state);

} } // namespace


//==============================================================================
// Classes

#include "ImportLoadFailedMsg.h"
#include "ImportParsingHandler.h"

#include "RedefinitionMsg.h"

#include "GrammarPlant.h"
#include "LibraryGateway.h"
#include "LibraryManager.h"
#include "RootManager.h"

#endif

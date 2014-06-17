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
 * @brief The classes of standard implementation of the Core.
 *
 * This namespace contains definitions for standard implementation of the core,
 * including standard grammar and commands and other related classes.
 */

//==============================================================================
// Data Types

/**
 * @brief A set of parsing flags to use with grammar terms.
 * @ingroup standard
 *
 * OMISSIBLE: Specifies that the parsed data for this term can be omitted if the
 *            data has no info to provide. For token terms, this is true if the
 *            token is constants. For other terms, it's true if the child term
 *            returned null contents. For route terms this will only be
 *            effective if the PASS_UP flag is also provided. For list terms
 *            (duplicate and concat) omission happens to individual items of the
 *            list if no data were received from the parent. Productions can be
 *            omitted if it has only one item which is a data of a referenced
 *            production.<br>
 * PASS_UP: This flag specifies that this term is to pass its data up to the
 *          parent. Typically, the parent's element will be a list type element
 *          so the data will be added to that list. This flag applies to route
 *          as well as list term types.<br>
 * FORCE_LIST: For list items (concat and duplicate). This flag will force the
 *             creation of a list item even if there is only one item in the
 *             list. This flag conflicts with PASS_UP.
 */
enumeration(ParsingFlags,
            OMISSIBLE = 1,
            FORCE_LIST = 2,
            PASS_UP = 4);

class LibraryGateway;
class RootManager;

/**
 * @brief A getter function for the library gateway.
 * @ingroup main
 *
 * This type defines a pointer to a function that gets the singleton library
 * gateway that every dynamic library should implement.
 */
typedef LibraryGateway* (*LibraryGatewayGetter)();


//==============================================================================
// Constants

/**
 * @brief The name of the getter function for the library gateway.
 * @ingroup main
 *
 * This function gets the singleton library gateway that every dynamic library
 * should implement.
 */
#define LIBRARY_GATEWAY_GETTER_NAME "get_library_gateway"

/**
 * @brief The definition of the getter function for the library gateway.
 * @ingroup main
 *
 * This function gets the singleton library gateway that every dynamic library
 * should implement.
 */
#define LIBRARY_GATEWAY_GETTER_DEF Core::Standard::LibraryGateway* get_library_gateway()


//==============================================================================
// Functions

/// Merge the values of two container trees.
Bool mergeContainers(IdentifiableObject *dest, IdentifiableObject *src, Parser::State *state);

/// Merge the given object into the provided definition manager.
void mergeDefinition(Data::SharedRepository *repository, Char const *qualifier,
                     SharedPtr<IdentifiableObject> const &obj, Parser::State *state);

} } // namespace


//==============================================================================
// Classes

#include "ParsedItem.h"
#include "ParsedToken.h"
#include "ParsedRoute.h"
#include "ParsedList.h"
#include "ParsedDataBrowser.h"
#include "GenericParsingHandler.h"

#include "ImportLoadFailedMsg.h"
#include "ImportParsingHandler.h"

#include "RedefinitionMsg.h"

#include "GrammarPlant.h"
#include "LibraryGateway.h"
#include "LibraryManager.h"
#include "RootManager.h"

#endif

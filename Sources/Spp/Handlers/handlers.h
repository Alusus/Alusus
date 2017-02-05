/**
 * @file Spp/Handlers/ast.h
 * Contains the definitions and include statements of all types in the
 * Spp::Handlers namespace.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_H
#define SPP_HANDLERS_H

namespace Spp { namespace Handlers
{

/**
 * @defgroup spp_parsing_handlers ParsingHandlers
 * @ingroup spp
 * @brief Classes related to the SPP's paring handlers.
 */

//==============================================================================
// Build Messages

DEFINE_BUILD_MSG(InvalidDumpArg, "Spp.Handlers", "Spp", "alusus.net", "SPP1001", 1,
                 STR("Invalid argument for 'dump' command."));
DEFINE_BUILD_MSG(InvalidBuildArg, "Spp.Handlers", "Spp", "alusus.net", "SPP1001", 1,
                 STR("Invalid argument for 'build' command."));
DEFINE_BUILD_MSG(MissingFunctionSigOrBody, "Spp.Handlers", "Spp", "alusus.net", "SPP1002", 1,
                 STR("Missing signature or body in 'function' command."));
DEFINE_BUILD_MSG(InvalidFunctionSignature, "Spp.Handleres", "Spp", "alusus.net", "SPP1003", 1,
                 STR("Invalid function signature."));
DEFINE_BUILD_MSG(InvalidFunctionBody, "Spp.Handlers", "Spp", "alusus.net", "SPP1004", 1,
                 STR("Invalid function body."));
DEFINE_BUILD_MSG(InvalidFunctionArg, "Spp.Handlers", "Spp", "alusus.net", "SPP1005", 1,
                 STR("Invalid function argumnet."));
DEFINE_BUILD_MSG(InvalidFunctionArgName, "Spp.Handlers", "Spp", "alusus.net", "SPP1006", 1,
                 STR("Invalid function arg name."));
DEFINE_BUILD_MSG(InvalidFunctionArgType, "Spp.Handlers", "Spp", "alusus.net", "SPP1007", 1,
                 STR("Invalid function arg type."));
DEFINE_BUILD_MSG(InvalidWhileStatement, "Spp.Handlers", "Spp", "alusus.net", "SPP1008", 1,
                 STR("Invalid while statement."));

} } // namespace


//==============================================================================
// Classes

#include "FunctionParsingHandler.h"
#include "WhileParsingHandler.h"
#include "DumpParsingHandler.h"
#include "BuildParsingHandler.h"

#endif

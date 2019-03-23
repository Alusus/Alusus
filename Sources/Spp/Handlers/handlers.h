/**
 * @file Spp/Handlers/ast.h
 * Contains the definitions and include statements of all types in the
 * Spp::Handlers namespace.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
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
 * @defgroup spp_handlers Handlers
 * @ingroup spp
 * @brief Classes related to the SPP's parsing handlers.
 */

} } // namespace


//==============================================================================
// Classes

#include "FunctionParsingHandler.h"
#include "IfParsingHandler.h"
#include "WhileParsingHandler.h"
#include "ForParsingHandler.h"
#include "CodeGenParsingHandler.h"
#include "TildeOpParsingHandler.h"
#include "MacroParsingHandler.h"
#include "ModuleParsingHandler.h"
#include "TypeParsingHandler.h"

#endif

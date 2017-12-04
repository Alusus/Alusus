/**
 * @file Spp/Handlers/ast.h
 * Contains the definitions and include statements of all types in the
 * Spp::Handlers namespace.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
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

//==============================================================================
// Notices

DEFINE_NOTICE(InvalidBuildArgNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1001", 1,
  STR("Invalid argument for 'build' command.")
);
DEFINE_NOTICE(MissingFunctionSigNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1002", 1,
  STR("Missing signature in 'function' command.")
);
DEFINE_NOTICE(InvalidFunctionSignatureNotice, "Spp.Handleres", "Spp", "alusus.net", "SPP1003", 1,
  STR("Invalid function signature.")
);
DEFINE_NOTICE(InvalidFunctionBodyNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1004", 1,
  STR("Invalid function body.")
);
DEFINE_NOTICE(InvalidFunctionArgNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1005", 1,
  STR("Invalid function argumnet.")
);
DEFINE_NOTICE(InvalidFunctionArgNameNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1006", 1,
  STR("Invalid function arg name.")
);
DEFINE_NOTICE(InvalidFunctionArgTypeNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1007", 1,
  STR("Invalid function arg type.")
);
DEFINE_NOTICE(InvalidWhileStatementNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1008", 1,
  STR("Invalid while statement.")
);
DEFINE_NOTICE(InvalidIfStatementNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1009", 1,
  STR("Invalid if statement.")
);

} } // namespace


//==============================================================================
// Classes

#include "FunctionParsingHandler.h"
#include "IfParsingHandler.h"
#include "WhileParsingHandler.h"
#include "CodeGenParsingHandler.h"

#endif

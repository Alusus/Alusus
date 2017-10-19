/**
 * @file Core/Processing/Handlers/ast.h
 * Contains the definitions and include statements of all types in the
 * Core::Processing::Handlers namespace.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_H
#define CORE_PROCESSING_HANDLERS_H

namespace Core { namespace Processing { namespace Handlers
{

/**
 * @defgroup core_parsing_handlers ParsingHandlers
 * @ingroup processing
 * @brief Classes related to the Core's paring handlers.
 */

//==============================================================================
// Notices

DEFINE_NOTICE(MissingDefLinkNotice, "Core.Processing.Handlers", "Core", "alusus.net", "DEF1001", 1,
  STR("Missing link expression in 'def' command.")
);
DEFINE_NOTICE(MissingDefNameNotice, "Core.Processing.Handlers", "Core", "alusus.net", "DEF1002", 1,
  STR("Missing definition name in 'def' command.")
);
DEFINE_NOTICE(InvalidDefCommandNotice, "Core.Processing.Handlers", "Core", "alusus.net", "DEF1003", 1,
  STR("Invalid 'def' command.")
);

} } } // namespace


//==============================================================================
// Classes

#include "GenericParsingHandler.h"
#include "ScopeParsingHandler.h"
#include "DefParsingHandler.h"
#include "ListParsingHandler.h"
#include "InfixParsingHandler.h"
#include "OutfixParsingHandler.h"
#include "PrefixParsingHandler.h"
#include "PostfixParsingHandler.h"
#include "SubjectParsingHandler.h"
#include "ChainOpParsingHandler.h"
#include "TextParsingHandler.h"
#include "ModifierParsingHandler.h"
#include "GenericCommandParsingHandler.h"
#include "CustomParsingHandler.h"

#endif

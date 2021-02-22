/**
 * @file Core/Processing/Handlers/ast.h
 * Contains the definitions and include statements of all types in the
 * Core::Processing::Handlers namespace.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_HANDLERS_H
#define CORE_PROCESSING_HANDLERS_H

/**
 * @defgroup core_processing_handlers ParsingHandlers
 * @ingroup core_processing
 * @brief Classes related to the Core's paring handlers.
 */

//==============================================================================
// Forward Class Declaration

namespace Core::Main
{
  class RootScopeHandler;
}


//==============================================================================
// Classes

#include "ConstTokenizingHandler.h"
#include "StringLiteralTokenizingHandler.h"
#include "IdentifierTokenizingHandler.h"
// TODO:
// #include "NumberLiteralTokenizingHandler.h"

#include "GenericParsingHandler.h"
#include "ScopeParsingHandler.h"
#include "RootScopeParsingHandler.h"
#include "DefParsingHandler.h"
#include "SequenceParsingHandler.h"
#include "ListExpParsingHandler.h"
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

#include "ImportParsingHandler.h"
#include "DumpAstParsingHandler.h"

#endif

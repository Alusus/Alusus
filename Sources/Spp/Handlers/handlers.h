/**
 * @file Spp/Handlers/handlers.h
 * Contains the definitions and include statements of all types in the
 * Spp::Handlers namespace.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_H
#define SPP_HANDLERS_H

namespace Spp::Handlers
{

/**
 * @defgroup spp_handlers Handlers
 * @ingroup spp
 * @brief Classes related to the SPP's parsing handlers.
 */

/// @ingroup spp_handlers
Bool processFunctionArgPacks(Spp::Ast::FunctionType *funcType, Core::Notices::Store *noticeStore);

/// @ingroup spp_handlers
Bool parseNumber(
  TiObject *ast, TiWord &result, Core::Data::Ast::MetaHaving *parentMetadata, Core::Notices::Store *noticeStore
);

} // namespace


//==============================================================================
// Classes

#include "FunctionParsingHandler.h"
#include "IfParsingHandler.h"
#include "WhileParsingHandler.h"
#include "ForParsingHandler.h"
#include "TildeOpParsingHandler.h"
#include "MacroParsingHandler.h"
#include "ModuleParsingHandler.h"
#include "TypeParsingHandler.h"
#include "TypeHandlersParsingHandler.h"
#include "UseInParsingHandler.h"

#endif

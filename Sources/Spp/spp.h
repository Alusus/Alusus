/**
 * @file Spp/spp.h
 * The main header file of the SPP. Contains include statements for the entire
 * Standard Programming Paradigm library.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// System header files
#include "core.h"

namespace Spp
{

/**
 * @defgroup spp Spp
 */

}

// Alusus's SPP base header files

#include "ExecutionContext.h"

#include "Ast/ast.h"

#include "SeekerExtension.h"

#include "LlvmCodeGen/llvm_code_gen.h"
#include "Handlers/handlers.h"

#include "LibraryGateway.h"

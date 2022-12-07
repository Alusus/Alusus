/**
 * @file Spp/spp.h
 * The main header file of the SPP. Contains include statements for the entire
 * Standard Programming Paradigm library.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_H
#define SPP_H

// System header files
#include "core.h"

namespace Spp
{

/**
 * @defgroup spp Spp
 */

/**
 * @brief Constant definitions for different levels of log messages.
 * @ingroup spp
 */
s_enum(LogLevel,
  PREPROCESS = 64,
  CODEGEN = 128,
  LLVMCODEGEN_IR = 268,
  LLVMCODEGEN_DIAGNOSTIC = 512
);

/// Function pointer type for parsing handlers that takes state input data and translates it to something else.
typedef TioSharedPtr (*ParsingHandlerFunc)(TioSharedPtr);

}

// Type names
DEFINE_TYPE_NAME(Spp::ParsingHandlerFunc, "alusus.org/Spp/Spp.ParsingHandlerFunc");

// Forward declarations

namespace Spp
{
  class BuildSession;

  namespace Ast
  {
    class Function;
  }
}

// Alusus's SPP base header files

#include "ExecutionContext.h"
#include "GlobalCtorDtorInfo.h"
#include "DependencyList.h"
#include "DependencyInfo.h"
#include "Executing.h"
#include "ExpressionComputation.h"

#include "Notices/notices.h"

#include "Ast/ast.h"

#include "CodeGen/code_gen.h"
#include "LlvmCodeGen/llvm_code_gen.h"

#include "Handlers/handlers.h"

#include "Grammar/grammar.h"

#include "BuildSession.h"
#include "BuildManager.h"

#include "Rt/rt.h"

#include "SeekerExtension.h"
#include "RootScopeHandlerExtension.h"
#include "RootManagerExtension.h"

#include "LibraryGateway.h"

#endif

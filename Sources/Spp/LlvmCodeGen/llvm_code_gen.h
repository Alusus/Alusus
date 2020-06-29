/**
 * @file Spp/LlvmCodeGen/llvm_code_gen.h
 * Contains the definitions and include statements of all types in the
 * LlvmCodeGen namespace.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_LLVMCODEGEN_H
#define SPP_LLVMCODEGEN_LLVMCODEGEN_H

#include "llvm_dependencies.h"

namespace Spp::LlvmCodeGen
{

/**
 * @defgroup spp_llvmcodegen Build Operations for LLVM Target
 * @ingroup spp
 * @brief Classes for providing an LLVM build target.
 */


// Forward Declarations
class BuildTarget;

// Global Functions
void llvmDiagnosticCallback(const llvm::DiagnosticInfo &di, void *context);

} // namespace


//==============================================================================
// Classes

// Build Data
#include "types.h"
#include "Function.h"
#include "Block.h"
#include "Variable.h"
#include "Value.h"
#include "IfContext.h"
#include "LoopContext.h"

// The Generator
#include "jit_engines.h"
#include "TargetGenerator.h"
#include "BuildTarget.h"
#include "LazyJitBuildTarget.h"
#include "OfflineBuildTarget.h"

#endif

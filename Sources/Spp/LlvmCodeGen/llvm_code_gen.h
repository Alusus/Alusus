/**
 * @file Spp/LlvmCodeGen/llvm_code_gen.h
 * Contains the definitions and include statements of all types in the
 * LlvmCodeGen namespace.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_LLVMCODEGEN_H
#define SPP_LLVMCODEGEN_LLVMCODEGEN_H

namespace Spp { namespace LlvmCodeGen
{

/**
 * @defgroup spp_llvmcodegen Build Operations for LLVM Target
 * @ingroup spp
 * @brief Classes for providing an LLVM build target.
 */

} } // namespace


//==============================================================================
// Classes

#include "llvm_dependencies.h"

// Build Data
#include "types.h"
#include "Block.h"
#include "Variable.h"
#include "Value.h"
#include "IfContext.h"
#include "LoopContext.h"

// The Generator
#include "TargetGenerator.h"

#endif

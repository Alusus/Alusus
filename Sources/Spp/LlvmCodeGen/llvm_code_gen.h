/**
 * @file Spp/LlvmCodeGen/llvm_code_gen.h
 * Contains the definitions and include statements of all types in the
 * LlvmCodeGen namespace.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_LLVMCODEGEN_H
#define SPP_LLVMCODEGEN_LLVMCODEGEN_H

namespace Spp { namespace LlvmCodeGen
{

/**
 * @defgroup llvm_code_gen LLVM Code Generation
 * @ingroup spp
 * @brief Classes related to the SPP's llvm-based code generation.
 */

} } // namespace


//==============================================================================
// Classes

#include "llvm_dependencies.h"

// Types
#include "Type.h"
#include "IntegerType.h"
#include "FloatType.h"
#include "PointerType.h"

// Functions
#include "Block.h"
#include "Function.h"
#include "UserFunction.h"

// The Generator
#include "Generator.h"

#endif

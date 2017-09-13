/**
 * @file Spp/LlvmCodeGen/llvm_code_gen.h
 * Contains the definitions and include statements of all types in the
 * LlvmCodeGen namespace.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
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

constexpr Char const* META_EXTRA_NAME = STR("llvmCodeGen");


//==============================================================================
// Build Messages

DEFINE_NOTICE(NoCalleeMatchNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1301", 1,
  STR("No matches were found for the given callee.")
);
DEFINE_NOTICE(MultipleCalleeMatchNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1302", 1,
  STR("Multiple matches were found for the given callee.")
);
DEFINE_NOTICE(InvalidTypeNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1303", 1,
  STR("No valid match was found for the given type.")
);
DEFINE_NOTICE(InvalidFloatBitCountNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1304", 1,
  STR("Invalid float bit count. Bit count should be either 32, 64, or 128.")
);
DEFINE_NOTICE(InvalidCastNotice, "Spp.Handlers", "Spp", "alusus.net", "SPP1305", 1,
  STR("Invalid cast.")
);

} } // namespace


//==============================================================================
// Classes

#include "llvm_dependencies.h"

// Functions
#include "Block.h"
#include "Function.h"
#include "UserFunction.h"

// The Generator
#include "TypeGenerator.h"
#include "Generator.h"

#endif

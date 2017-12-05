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

DEFINE_NOTICE(UnsupportedOperationNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1301", 1,
  STR("Unsupported operation.")
);
DEFINE_NOTICE(InvalidOperationNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1302", 1,
  STR("Invalid operation.")
);
DEFINE_NOTICE(InvalidTypeNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1305", 1,
  STR("No valid match was found for the given type.")
);
DEFINE_NOTICE(InvalidFloatBitCountNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1306", 1,
  STR("Invalid float bit count. Bit count should be either 32, 64, or 128.")
);
DEFINE_NOTICE(InvalidIntegerBitCountNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1307", 1,
  STR("Invalid integer bit count.")
);
DEFINE_NOTICE(InvalidCastNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1308", 1,
  STR("Invalid cast.")
);
DEFINE_NOTICE(InvalidReturnValueNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1309", 1,
  STR("Invalid return value.")
);
DEFINE_NOTICE(InvalidReferenceNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1310", 1,
  STR("Invalid symbol.")
);
DEFINE_NOTICE(NotImplicitlyCastableNotice, "Spp.LlvmCodeGen", "Spp", "alusus.net", "SPP1311", 1,
  STR("Value is not implicitly castable to target type.")
);

} } // namespace


//==============================================================================
// Classes

namespace Spp { namespace LlvmCodeGen
{

class Generator;

} }

#include "llvm_dependencies.h"

// Functions
#include "Block.h"
#include "Variable.h"

// The Generator
#include "TypeGenerator.h"
#include "LiteralGenerator.h"
#include "ExpressionGenerator.h"
#include "VariableGenerator.h"
#include "CommandGenerator.h"
#include "Generator.h"

#endif

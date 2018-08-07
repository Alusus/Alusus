/**
 * @file Spp/Notices/notices.h
 * Contains the definitions and include statements of all user notices.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_NOTICES_NOTICES_H
#define SPP_NOTICES_NOTICES_H

namespace Spp::Notices
{

/**
 * @defgroup spp_notices Notices
 * @ingroup spp
 * @brief Classes that contains notices.
 */

} // namespace


//==============================================================================
// Ast Related Notices

namespace Spp::Notices
{

DEFINE_NOTICE(TemplateArgMismatchNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1001", 1,
  STR("Template arguments mismatch.")
);
DEFINE_NOTICE(InvalidTemplateArgNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1002", 1,
  STR("Invalid template argument.")
);
DEFINE_NOTICE(InvalidTypeNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1003", 1,
  STR("No valid match was found for the given type.")
);
DEFINE_NOTICE(InvalidTypeMemberNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1004", 1,
  STR("No valid match was found for the given type member.")
);
DEFINE_NOTICE(MultipleCalleeMatchNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1005", 1,
  STR("Multiple matches were found for the given callee.")
);
DEFINE_NOTICE(NoCalleeMatchNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1006", 1,
  STR("No matches were found for the given callee.")
);
DEFINE_NOTICE(UnknownSymbolNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1007", 1,
  STR("Unknown symbol.")
);
DEFINE_NOTICE(ArgsMismatchNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1008", 1,
  STR("Provided arguments do not match signature.")
);
DEFINE_NOTICE(ConflictingDefinitionsNotice, "Spp.Notices", "Spp", "alusus.net", "SPPA1009", 1,
  STR("Multiple conflicting definitions were found for the given callee.")
);

} /// namespace


//==============================================================================
// Handlers Related Notices

namespace Spp::Notices
{

DEFINE_NOTICE(InvalidBuildArgNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1001", 1,
  STR("Invalid argument for build command.")
);
DEFINE_NOTICE(MissingFunctionSigNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1002", 1,
  STR("Missing signature in function definition.")
);
DEFINE_NOTICE(InvalidFunctionSignatureNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1003", 1,
  STR("Invalid function signature.")
);
DEFINE_NOTICE(InvalidFunctionBodyNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1004", 1,
  STR("Invalid function body.")
);
DEFINE_NOTICE(InvalidFunctionArgNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1005", 1,
  STR("Invalid function argumnet.")
);
DEFINE_NOTICE(InvalidFunctionArgNameNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1006", 1,
  STR("Invalid function arg name.")
);
DEFINE_NOTICE(InvalidFunctionArgTypeNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1007", 1,
  STR("Invalid function arg type.")
);
DEFINE_NOTICE(InvalidWhileStatementNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1008", 1,
  STR("Invalid while statement.")
);
DEFINE_NOTICE(InvalidIfStatementNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1009", 1,
  STR("Invalid if statement.")
);
DEFINE_NOTICE(InvalidForStatementNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1010", 1,
  STR("Invalid for statement.")
);
DEFINE_NOTICE(InvalidContinueStatementNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1011", 1,
  STR("Invalid continue statement.")
);
DEFINE_NOTICE(InvalidBreakStatementNotice, "Spp.Notices", "Spp", "alusus.net", "SPPH1012", 1,
  STR("Invalid break statement.")
);

} // namespace


//==============================================================================
// CodeGen Related Notices

namespace Spp::Notices
{

DEFINE_NOTICE(UnsupportedOperationNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1001", 1,
  STR("Unsupported operation.")
);
DEFINE_NOTICE(InvalidOperationNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1002", 1,
  STR("Invalid operation.")
);
DEFINE_NOTICE(InvalidFloatBitCountNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1003", 1,
  STR("Invalid float bit count. Bit count should be either 32 or 64.")
);
DEFINE_NOTICE(InvalidIntegerBitCountNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1004", 1,
  STR("Invalid integer bit count.")
);
DEFINE_NOTICE(InvalidCastNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1005", 1,
  STR("Invalid cast.")
);
DEFINE_NOTICE(InvalidReturnValueNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1006", 1,
  STR("Invalid return value.")
);
DEFINE_NOTICE(InvalidConditionValueNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1007", 1,
  STR("Invalid condition value.")
);
DEFINE_NOTICE(InvalidReferenceNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1008", 1,
  STR("Invalid symbol.")
);
DEFINE_NOTICE(InvalidSizeOperandNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1009", 1,
  STR("Invalid operand for ~size operator.")
);
DEFINE_NOTICE(InvalidExpressionListNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1010", 1,
  STR("Invalid expression list.")
);
DEFINE_NOTICE(InvalidContinueStepsNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1011", 1,
  STR("Invalid continue steps.")
);
DEFINE_NOTICE(InvalidBreakStepsNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1012", 1,
  STR("Invalid break steps.")
);
DEFINE_NOTICE(InvalidLogicalOperandNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1013", 1,
  STR("Invalid operand of logical operator.")
);
DEFINE_NOTICE(CircularUserTypeDefinitionsNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1014", 1,
  STR("Circular user type definitions.")
);
DEFINE_NOTICE(IncompatibleOperatorTypesNotice, "Spp.Notices", "Spp", "alusus.net", "SPPG1015", 1,
  STR("Incompatible types for the given operator.")
);

} // namespace

#endif

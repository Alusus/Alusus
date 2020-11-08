/**
 * @file Spp/Notices/notices.h
 * Contains the definitions and include statements of all user notices.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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

DEFINE_NOTICE(TemplateArgMismatchNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1001", 1,
  "Template arguments mismatch."
);
DEFINE_NOTICE(InvalidTemplateArgNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1002", 1,
  "Invalid template argument."
);
DEFINE_NOTICE(InvalidTypeNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1003", 1,
  "No valid match was found for the given type."
);
DEFINE_NOTICE(InvalidTypeMemberNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1004", 1,
  "No valid match was found for the given type member."
);
DEFINE_NOTICE(MultipleCalleeMatchNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1005", 1,
  "Multiple matches were found for the given callee."
);
DEFINE_NOTICE(NoCalleeMatchNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1006", 1,
  "No matches were found for the given callee."
);
DEFINE_NOTICE(UnknownSymbolNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1007", 1,
  "Unknown symbol."
);
DEFINE_NOTICE(ArgsMismatchNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1008", 1,
  "Provided arguments do not match signature."
);
DEFINE_NOTICE(ConflictingDefinitionsNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1009", 1,
  "Multiple conflicting definitions were found for the given callee."
);
DEFINE_NOTICE(InvalidUseStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1010", 1,
  "Use statement not pointing to a valid target."
);
DEFINE_NOTICE(InvalidSquareBracketOperandNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1011", 1,
  "Invalid operand for [] operator."
);
DEFINE_NOTICE(IdentifierIsNotTypeNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1012", 1,
  "Given identifier does not refer to a type."
);
DEFINE_NOTICE(MissingTypeNotice, "Spp.Notices", "Spp", "alusus.org", "SPPA1013", 1,
  "A needed type is missing. You may be missing an import."
);

} /// namespace


//==============================================================================
// Handlers Related Notices

namespace Spp::Notices
{

DEFINE_NOTICE(InvalidBuildArgNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1001", 1,
  "Invalid argument for build command."
);
DEFINE_NOTICE(MissingFunctionSigNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1002", 1,
  "Missing signature in function definition."
);
DEFINE_NOTICE(InvalidFunctionSignatureNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1003", 1,
  "Invalid function signature."
);
DEFINE_NOTICE(InvalidFunctionElementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1004", 1,
  "Invalid function element."
);
DEFINE_NOTICE(InvalidFunctionArgNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1005", 1,
  "Invalid function argumnet."
);
DEFINE_NOTICE(InvalidFunctionArgNameNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1006", 1,
  "Invalid function arg name."
);
DEFINE_NOTICE(InvalidFunctionArgTypeNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1007", 1,
  "Invalid function arg type."
);
DEFINE_NOTICE(InvalidWhileStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1008", 1,
  "Invalid while statement."
);
DEFINE_NOTICE(InvalidIfStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1009", 1,
  "Invalid if statement."
);
DEFINE_NOTICE(InvalidForStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1010", 1,
  "Invalid for statement."
);
DEFINE_NOTICE(InvalidContinueStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1011", 1,
  "Invalid continue statement."
);
DEFINE_NOTICE(InvalidBreakStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1012", 1,
  "Invalid break statement."
);
DEFINE_NOTICE(InvalidMacroArgDefNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1013", 1,
  "Invalid macro argumnet definition."
);
DEFINE_NOTICE(InvalidHandlerStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1014", 1,
  "Invalid handler statement."
);
DEFINE_NOTICE(InvalidTypeElementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1015", 1,
  "Invalid type element."
);
DEFINE_NOTICE(InvalidTemplateArgNameNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1016", 1,
  "Invalid template arg name."
);
DEFINE_NOTICE(InvalidTemplateArgTypeNotice, "Spp.Notices", "Spp", "alusus.org", "SPPH1017", 1,
  "Invalid templatae arg type."
);

} // namespace


//==============================================================================
// CodeGen Related Notices

namespace Spp::Notices
{

DEFINE_NOTICE(UnsupportedOperationNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1001", 1,
  "Unsupported operation."
);
DEFINE_NOTICE(InvalidOperationNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1002", 1,
  "Invalid operation."
);
DEFINE_NOTICE(InvalidFloatBitCountNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1003", 1,
  "Invalid float bit count. Bit count should be either 32 or 64."
);
DEFINE_NOTICE(InvalidIntegerBitCountNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1004", 1,
  "Invalid integer bit count."
);
DEFINE_NOTICE(InvalidCastNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1005", 1,
  "Invalid cast."
);
DEFINE_NOTICE(InvalidReturnValueNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1006", 1,
  "Invalid return value."
);
DEFINE_NOTICE(InvalidConditionValueNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1007", 1,
  "Invalid condition value."
);
DEFINE_NOTICE(InvalidReferenceNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1008", 1,
  "Invalid symbol."
);
DEFINE_NOTICE(InvalidSizeOperandNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1009", 1,
  "Invalid operand for ~size operator."
);
DEFINE_NOTICE(InvalidExpressionListNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1010", 1,
  "Invalid expression list."
);
DEFINE_NOTICE(InvalidContinueStepsNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1011", 1,
  "Invalid continue steps."
);
DEFINE_NOTICE(InvalidBreakStepsNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1012", 1,
  "Invalid break steps."
);
DEFINE_NOTICE(InvalidLogicalOperandNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1013", 1,
  "Invalid operand of logical operator."
);
DEFINE_NOTICE(CircularUserTypeDefinitionsNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1014", 1,
  "Circular user type definitions."
);
DEFINE_NOTICE(IncompatibleOperatorTypesNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1015", 1,
  "Incompatible types for the given operator."
);
DEFINE_NOTICE(ExecutionAbortedNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1016", 1,
  "Execution aborted due to previous errors."
);
DEFINE_NOTICE(InvalidMacroArgNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1017", 1,
  "Invalid macro argument."
);
DEFINE_NOTICE(InvalidMacroNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1018", 1,
  "Invalid macro."
);
DEFINE_NOTICE(InvalidObjectMemberAccessNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1019", 1,
  "Invalid access to object member."
);
DEFINE_NOTICE(InvalidGlobalDefAccessNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1020", 1,
  "Invalid access to global definition."
);
DEFINE_NOTICE(UnreachableCodeNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1021", 1,
  "Unreachable code."
);
DEFINE_NOTICE(MissingReturnStatementNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1022", 1,
  "Missing a return statement for a non-void function."
);
DEFINE_NOTICE(InvalidModifierDataNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1023", 1,
  "Invalid modifier data."
);
DEFINE_NOTICE(UninitializedLocalVariableNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1024", 1,
  "Trying to access a local variable before it's initialized."
);
DEFINE_NOTICE(InvalidInitOperandNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1025", 1,
  "Invalid operand for ~init operator."
);
DEFINE_NOTICE(InvalidTerminateOperandNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1026", 1,
  "Invalid operand for ~terminate operator."
);
DEFINE_NOTICE(ThisRedefinedNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1027", 1,
  "Invalid redefinition of `this` in function args."
);
DEFINE_NOTICE(SharedInjectionNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1028", 1,
  "Trying to inject a shared definition."
);
DEFINE_NOTICE(InvalidInjectionTypeNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1029", 1,
  "Invalid type for an injected definition."
);
DEFINE_NOTICE(TypeMissingAssignOpNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1030", 1,
  "Assignment failed. This type needs an assignment operator."
);
DEFINE_NOTICE(TypeMissingMatchingInitOpNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1031", 1,
  "Variable initialization failed. Custom init op matching the given data is needed for this type."
);
DEFINE_NOTICE(InvalidNextArgNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1032", 1,
  "Invalid next_arg operation."
);
DEFINE_NOTICE(InvalidDerefOperandNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1033", 1,
  "Invalid operand for ~deref operator."
);
DEFINE_NOTICE(InvalidNoDerefOperandNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1034", 1,
  "Invalid operand for ~no_deref operator."
);
DEFINE_NOTICE(TypeMemberIsNotVarNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1035", 1,
  "The requested member is not a variable definition."
);
DEFINE_NOTICE(InvalidDotOpTargetNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1036", 1,
  "Cannot use dot operator on this type of elements."
);
DEFINE_NOTICE(IncompleteInfixOpNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1037", 1,
  "Incomplete infix operator. This is likely caused by a macro or an eval statement that evaluated to null."
);
DEFINE_NOTICE(IncompleteOutfixOpNotice, "Spp.Notices", "Spp", "alusus.org", "SPPG1038", 1,
  "Incomplete prefix or postfix operator. This is likely caused by a macro or an eval statement that evaluated to null."
);

} // namespace

#endif

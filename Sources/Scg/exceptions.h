/**
 * @file Scg/exceptions.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __exceptions_h__
#define __exceptions_h__

// Core header files.
#include <Basic/exceptions.h>

// Scg include files
#include <typedefs.h>


namespace Scg
{

  // TODO: Divide exceptions into compilation exceptions and logic exceptions.
  // For example, SyntaxErrorException is a compilation exception which is
  // raised when the input source is invalid. On the other hand,
  // ArgumentOutOfRangeException is a logic exception that is raised when
  // an SCG's function's is passed an argument having value out of the
  // expected range.
  DEFINE_EXCEPTION(ArgumentException, GenericException, STR("Argument Exception"));
  DEFINE_EXCEPTION(ArgumentMismatchException, ArgumentException, STR("Argument Mismatc Exception"));
  DEFINE_EXCEPTION(ArgumentOutOfRangeException, ArgumentException, STR("Argument Out of Range Exception"));
  DEFINE_EXCEPTION(CompilationErrorException, GenericException, STR("Compilation Error Exception"));
  DEFINE_EXCEPTION(EvaluationException, GenericException, STR("Evaluation Exception"));
  DEFINE_EXCEPTION(InfiniteLoopException, GenericException, STR("Infinite Loop Exception"));
  DEFINE_EXCEPTION(InvalidArgumentException, ArgumentException, STR("Invalid Argument Exception"));
  DEFINE_EXCEPTION(InvalidCastException, GenericException, STR("Invalid Cast Exception"));
  DEFINE_EXCEPTION(InvalidLhsException, GenericException, STR("Invalid Lhs Exception"));
  DEFINE_EXCEPTION(InvalidObjectException, GenericException, STR("Invalid Object Exception"));
  DEFINE_EXCEPTION(InvalidOperationException, GenericException, STR("Invalid Operation Exception"));
  DEFINE_EXCEPTION(InvalidTypeException, GenericException, STR("Invalid Type Exception"));
  DEFINE_EXCEPTION(InvalidValueException, GenericException, STR("Invalid Value Exception"));
  DEFINE_EXCEPTION(MultipleMatchesException,CompilationErrorException, STR("Multiple Matches Exception"));
  DEFINE_EXCEPTION(NotImplementedException, GenericException, STR("Not Implemented Exception"));
  DEFINE_EXCEPTION(RedefinitionException, GenericException, STR("Redefinition Exception"));
  DEFINE_EXCEPTION(SyntaxErrorException, GenericException, STR("Syntax Error Exception"));
  DEFINE_EXCEPTION(SystemException, GenericException, STR("System Exception"));
  DEFINE_EXCEPTION(UnattachedExpressionException, GenericException, STR("Unattached Expression Exception"));
  DEFINE_EXCEPTION(UndefinedFunctionException, CompilationErrorException, STR("Undefined Function Exception"));
  DEFINE_EXCEPTION(UndefinedTypeException, GenericException, STR("Undefined Type Exception"));
  DEFINE_EXCEPTION(UndefinedVariableException, GenericException, STR("Undefined Variable Exception"));
  DEFINE_EXCEPTION(UnreachableCodeException, GenericException, STR("Unreachable Code Exception"));

#define THROW_NOT_IMPLEMENTED() \
  throw EXCEPTION(NotImplementedException, "The function is not implemented yet.")
}

#endif // __exceptions_h__

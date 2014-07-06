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

// Scg include files
#include <typedefs.h>

namespace Scg
{
  class Exception
  {
  public:
      Exception(const std::string &message, const std::string &filename,
        const std::string &functionName, int lineNumber)
        : message(message)
        , filename(filename)
        , functionName(functionName)
        , lineNumber(lineNumber)
      { }

  public:
      const std::string &GetMessage() const { return message; }
      const std::string &GetFileName() const { return filename; }
      const std::string &GetFunctionName() const { return functionName; }
      const int GetLineNumber() const { return lineNumber; }

  public:
      std::string message;
      std::string filename;
      std::string functionName;
      int         lineNumber;
  };

#define DEFINE_EXCEPTION(exception_name, base)                                \
  class exception_name : public base                                          \
  {                                                                           \
  public:                                                                     \
      exception_name(const std::string &message, const std::string &filename, \
          const std::string &function, int lineNumber) :                      \
          base(message, filename, function, lineNumber)                       \
      {                                                                       \
      }                                                                       \
  }

  // TODO: Divide exceptions into compilation exceptions and logic exceptions.
  // For example, SyntaxErrorException is a compilation exception which is
  // raised when the input source is invalid. On the other hand,
  // ArgumentOutOfRangeException is a logic exception that is raised when
  // an SCG's function's is passed an argument having value out of the
  // expected range.
  DEFINE_EXCEPTION(ArgumentException,             Exception);
  DEFINE_EXCEPTION(ArgumentMismatchException,     ArgumentException);
  DEFINE_EXCEPTION(ArgumentOutOfRangeException,   ArgumentException);
  DEFINE_EXCEPTION(CompilationErrorException,     Exception);
  DEFINE_EXCEPTION(EvaluationException,           Exception);
  DEFINE_EXCEPTION(InfiniteLoopException,         Exception);
  DEFINE_EXCEPTION(InvalidArgumentException,      ArgumentException);
  DEFINE_EXCEPTION(InvalidCastException,          Exception);
  DEFINE_EXCEPTION(InvalidLhsException,           Exception);
  DEFINE_EXCEPTION(InvalidObjectException,        Exception);
  DEFINE_EXCEPTION(InvalidOperationException,     Exception);
  DEFINE_EXCEPTION(InvalidTypeException,          Exception);
  DEFINE_EXCEPTION(InvalidValueException,         Exception);
  DEFINE_EXCEPTION(MemoryException,               Exception);
  DEFINE_EXCEPTION(MultipleMatchesException,      CompilationErrorException);
  DEFINE_EXCEPTION(NotImplementedException,       Exception);
  DEFINE_EXCEPTION(RedefinitionException,         Exception);
  DEFINE_EXCEPTION(SyntaxErrorException,          Exception);
  DEFINE_EXCEPTION(SystemException,               Exception);
  DEFINE_EXCEPTION(UnattachedExpressionException, Exception);
  DEFINE_EXCEPTION(UndefinedFunctionException,    CompilationErrorException);
  DEFINE_EXCEPTION(UndefinedTypeException,        Exception);
  DEFINE_EXCEPTION(UndefinedVariableException,    Exception);
  DEFINE_EXCEPTION(UnreachableCodeException,      Exception);

#define THROW_EXCEPTION(exception_name, message) \
  throw exception_name(message, __FILE__, __FUNCTION__, __LINE__)

#define THROW_NOT_IMPLEMENTED() \
  THROW_EXCEPTION(NotImplementedException, "Not implemented yet.")
}

#endif // __exceptions_h__

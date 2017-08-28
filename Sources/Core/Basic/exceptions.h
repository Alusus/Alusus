/**
 * @file Core/Basic/exceptions.h
 * Contains the declarations of all exception classes used by the core.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_EXCEPTIONS_H
#define CORE_BASIC_EXCEPTIONS_H

namespace Core { namespace Basic
{

// TODO: Add member functions to retrieve exception detailed information.

//==============================================================================
// Exception Classes

/**
 * @brief The root of all exception classes used by the core.
 * @ingroup basic_exceptions
 * @details This class derives from std::exception.
 */
class Exception : public std::exception
{
  //============================================================================
  // Static Variables

  protected: Str functionName;
  protected: Str sourceFile;
  protected: Int lineNumber;


  //============================================================================
  // Constructors

  protected: Exception(Char const *function, Char const *source, Int line) :
    functionName(function), sourceFile(source), lineNumber(line)
  {
  }


  //============================================================================
  // Member Functions

  /// Get name of function where exception originated.
  public: Str const& getFunctionName() const
  {
    return this->functionName;
  }

  /// Get name of source file where exception originated.
  public: Str const& getSourceFile() const
  {
    return this->sourceFile;
  }

  /// Get line number at which exception was thrown.
  public: Int getLineNumber() const
  {
    return this->lineNumber;
  }

  /**
   * @brief Get the error message.
   *
   * This function is provided for compatibility with std::exception. User
   * should avoid using this function and use getErrorMessage instead. The
   * reason behind using a new function is to use the generic Char datatype
   * instead of the built-in 8-bit char datatype. The Char datatype is
   * introduced for future compatibility purposes when the code is migrated
   * to Unicode.
   */
  public: virtual const char* what() const throw()
  {
    static thread_local Str msg;
    msg = this->getVerboseErrorMessage();
    return msg.c_str();
  }

  /// Get the error message.
  public: virtual Str getErrorMessage() const throw()
  {
    return STR("Unknown Exception");
  }

  /// Get an error message that includes error location details.
  public: virtual Str getVerboseErrorMessage() const throw();

}; // class


/**
 * @brief Generic exception.
 * @ingroup basic_exceptions
 * @details This exception can hold any message.
 */
class GenericException : public Exception
{
  //============================================================================
  // Member Variables

  /// The error message associated with this exception.
  private: Str comment;


  //============================================================================
  // Constructor

  public: GenericException(Char const *cmt,
                           Char const *function, Char const *source, Int line) :
    comment(cmt), Exception(function, source, line)
  {
  }

  public: virtual ~GenericException() throw()
  {
  }


  //============================================================================
  // Member Functions

  public: Str const& getComment() const
  {
    return this->comment;
  }

  public: virtual Str getErrorMessage() const throw()
  {
    return STR("Generic Exception: ") + this->comment;
  }

}; // class


/**
 * @brief File operation exception.
 * @ingroup basic_exceptions
 * @details This exception type is thrown with file related exceptions.
 */
class FileException : public Exception
{
  //============================================================================
  // Member Variables

  /// The name of the file with which the exception happened.
  Str fileName;

  /**
   * @brief The kind of operation attempted on the file.
   * This value can be one of the following:<br>
   * o: Opening the file.<br>
   * c: Closing the file.<br>
   * r: Reading from the file.<br>
   * w: Writing to the file.
   */
  Char operation;

  /// An additional comment to include in the error message.
  Str comment;


  //============================================================================
  // Constructor

  public: FileException(Char const *fname, Char op, Char const *cmt,
                        Char const *function, Char const *source, Int line) :
    fileName(fname), operation(op), comment(cmt), Exception(function, source, line)
  {
  }

  public: FileException(Char const *fname, Char op,
                        Char const *function, Char const *source, Int line) :
    fileName(fname), operation(op), Exception(function, source, line)
  {
  }

  public: virtual ~FileException() throw()
  {
  }


  //============================================================================
  // Member Functions

  public: Str const& getFileName() const
  {
    return this->fileName;
  }

  public: Char getOperation() const
  {
    return this->operation;
  }

  public: Str const& getComment() const
  {
    return this->comment;
  }

  public: virtual Str getErrorMessage() const throw();

}; // class


/**
 * @brief Memory operation exception.
 * @ingroup basic_exceptions
 * @details This exception type is thrown with memory related exceptions.
 */
class MemoryException : public Exception
{
  //============================================================================
  // Member Variables

  /**
   * @brief The kind of operation attempted.
   * This value can be one of the following:<br>
   * a: Allocate a new memory block.<br>
   * f: Free a memory block.<br>
   */
  Char operation;

  /// An additional comment to include in the error message.
  Str comment;


  //============================================================================
  // Constructor

  public: MemoryException(Char op, Char const *cmt,
                          Char const *function, Char const *source, Int line) :
    operation(op), comment(cmt), Exception(function, source, line)
  {
  }

  public: MemoryException(Char op,
                          Char const *function, Char const *source, Int line) :
    operation(op), Exception(function, source, line)
  {
  }

  public: virtual ~MemoryException() throw()
  {
  }


  //============================================================================
  // Member Functions

  public: Char getOperation() const
  {
    return this->operation;
  }

  public: Str const& getComment() const
  {
    return this->comment;
  }

  public: virtual Str getErrorMessage() const throw();

}; // class


/**
 * @brief Invalid arguments exception.
 * @ingroup basic_exceptions
 *
 * This exception type is thrown with exceptions related to invalid supplied
 * arguments.
 */
class InvalidArgumentException : public Exception
{
  //============================================================================
  // Member Variables

  /// The name of the argument that carried invalid data.
  Str argumentName;

  /// The value of the argument that carried invalid data.
  Str argumentValue;

  /// An additional comment to include in the error message.
  Str comment;


  //============================================================================
  // Constructor

  public: InvalidArgumentException(Char const *argname, Char const *cmt,
                                   Char const *function, Char const *source, Int line) :
    argumentName(argname), comment(cmt), Exception(function, source, line)
  {
  }

  public: template <class T> InvalidArgumentException(Char const *argname, Char const *cmt, const T &argvalue,
                                                      Char const *function, Char const *source, Int line) :
    argumentName(argname), comment(cmt), Exception(function, source, line)
  {
    StrStream stream;
    stream << argvalue;
    this->argumentValue = stream.str();
  }

  public: virtual ~InvalidArgumentException() throw()
  {
  }


  //============================================================================
  // Member Functions

  public: Str const& getArgumentName() const
  {
    return this->argumentName;
  }

  public: Str const& getArgumentValue() const
  {
    return this->argumentValue;
  }

  public: Str const& getComment() const
  {
    return this->comment;
  }

  public: virtual Str getErrorMessage() const throw();

}; // class

} } // namespace


//==============================================================================
// Exception Macros

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600))
  #define __THIS_FUNCTION__ __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
  #define __THIS_FUNCTION__ __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
  #define __THIS_FUNCTION__ __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
  #define __THIS_FUNCTION__ __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
  #define __THIS_FUNCTION__ __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
  #define __THIS_FUNCTION__ __func__
#else
  #define __THIS_FUNCTION__ "(unknown)"
#endif

#define DEFINE_EXCEPTION(exceptionName, base, title) \
  class exceptionName : public base \
  { \
  public: \
    exceptionName(Char const *cmt, \
                   Char const *function, Char const *source, Int line) : \
      base(cmt, function, source, line) \
    { \
    } \
    virtual Core::Basic::Str getErrorMessage() const throw() \
    { \
      return Core::Basic::Str(title) + STR(": ") + this->getComment(); \
    } \
  }

#define EXCEPTION(exceptionName, ...) \
  exceptionName(__VA_ARGS__, __THIS_FUNCTION__, __FILE__, __LINE__)

#endif

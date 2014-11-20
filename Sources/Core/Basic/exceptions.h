/**
 * @file Core/Basic/exceptions.h
 * Contains the declarations of all exception classes used by the core.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef BASIC_EXCEPTIONS_H
#define BASIC_EXCEPTIONS_H

namespace Core { namespace Basic
{

// TODO: Add member functions to retrieve exception detailed information.

/**
 * @brief The root of all exception classes used by the core.
 * @ingroup basic_exceptions
 * @details This class derives from std::exception.
 */
class Exception : public std::exception
{
  //============================================================================
  // Static Variables

  /**
   * @brief A buffer to hold error message text temporarily.
   *
   * This variable is used by getErrorMessage to host the message text
   * returned by the function.
   */
  protected: static Str _message;


  //============================================================================
  // Member Functions

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
    return this->getErrorMessage();
  }

  /// Get the error message.
  public: virtual Char const* getErrorMessage() const throw()
  {
    return STR("Unknown Exception");
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

  public: FileException(Char const *fname, Char op, Char const *cmt=0) :
    fileName(fname), operation(op)
  {
    if (cmt != 0) this->comment = cmt;
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

  public: virtual Char const* getErrorMessage() const throw();

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

  public: MemoryException(Char op, Char const *cmt=0) : operation(op)
  {
    if (cmt != 0) this->comment = cmt;
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

  public: virtual Char const* getErrorMessage() const throw();

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

  /**
   * @brief The location where the exception happened.
   * This can be a file name, a function name, etc.
   */
  Str location;

  /// An additional comment to include in the error message.
  Str comment;


  //============================================================================
  // Constructor

  public: InvalidArgumentException(Char const *argname, Char const *loc=0,
                                   Char const *cmt=0) :
    argumentName(argname)
  {
    if (loc != 0) this->location = loc;
    if (cmt != 0) this->comment = cmt;
  }

  public: template <class T> InvalidArgumentException(Char const *argname, Char const *loc, Char const *cmt,
                                                      const T &argvalue) :
    argumentName(argname)
  {
    if (loc != 0) this->location = loc;
    if (cmt != 0) this->comment = cmt;
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

  public: Str const& getLocation() const
  {
    return this->location;
  }

  public: Str const& getComment() const
  {
    return this->comment;
  }

  public: virtual Char const* getErrorMessage() const throw();

}; // class


/**
 * @brief General exception.
 * @ingroup basic_exceptions
 * @details This exception can hold any message.
 */
class GeneralException : public Exception
{
  //============================================================================
  // Member Variables

  /**
   * @brief The location where the exception happened.
   * @details This can be a file name, a function name, etc.
   */
  Str location;

  /// The error message associated with this exception.
  Str message;


  //============================================================================
  // Constructor

  public: GeneralException(Char const *msg, Char const *loc=0) :
    message(msg)
  {
    if (loc != 0) this->location = loc;
  }

  public: virtual ~GeneralException() throw()
  {
  }


  //============================================================================
  // Member Functions

  public: Str const& getLocation() const
  {
    return this->location;
  }

  public: Str const& get_message() const
  {
    return this->message;
  }

  public: virtual Char const* getErrorMessage() const throw();

}; // class

} } // namespace

#endif

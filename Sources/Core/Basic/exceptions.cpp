/**
 * @file Core/Basic/exceptions.cpp
 * Contains the implementation of exception classes.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Functions

Str Exception::getVerboseErrorMessage() const throw()
{
  return this->getErrorMessage() + S("\nLocation:\n") + this->functionName + S("\n") +
      this->sourceFile + S(" : ") + std::to_string(this->lineNumber);
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Str FileException::getErrorMessage() const throw()
{
  StrStream msg;
  msg << S("File Exception: ");
  switch (this->operation) {
    case C('o'):
      msg << S("Couldn't open file: ");
      break;
    case C('c'):
      msg << S("Couldn't close file: ");
      break;
    case C('r'):
      msg << S("Couldn't read from file: ");
      break;
    case C('w'):
      msg << S("Couldn't write to file: ");
      break;
    default:
      // this should never be reached
      ASSERT(false);
      break;
  }
  msg << this->fileName;
  msg << S(".");
  if (this->comment.size() > 0) {
    msg << S(".\n");
    msg << this->comment;
  }
  return msg.str();
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Str MemoryException::getErrorMessage() const throw()
{
  Str msg = S("Memory Exception: ");
  switch (this->operation) {
    case C('a'):
      msg += S("Couldn't allocate memory block.");
      break;
    case C('f'):
      msg += S("Couldn't free memory block.");
      break;
    default:
      // this should never be reached
      ASSERT(false);
      break;
  }
  if (this->comment.size() > 0) {
    msg += S("\n");
    msg += this->comment;
  }
  return msg;
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Str InvalidArgumentException::getErrorMessage() const throw()
{
  Str msg = S("Invalid Argument Exception (") + this->argumentName;
  if (this->argumentValue.size() > 0) {
    msg += S(" = ");
    msg += this->argumentValue;
  }
  msg += S(")");
  if (this->comment.size() > 0) {
    msg += S("\n");
    msg += this->comment;
  }
  return msg;
}

} } // namespace

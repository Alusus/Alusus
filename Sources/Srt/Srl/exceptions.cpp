/**
 * @file Srl/exceptions.cpp
 * Contains the implementation of exception classes.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "srl.h"

namespace Srl
{

//==============================================================================
// Member Functions

String Exception::getVerboseErrorMessage() const throw()
{
  #pragma GCC diagnostic ignored "-Wstringop-overread"
  return this->getErrorMessage() + S("\nLocation:\n") + this->functionName + S("\n") +
      this->sourceFile + S(" : ") + (LongInt)this->lineNumber;
  #pragma GCC diagnostic warning "-Wstringop-overread"
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
String FileException::getErrorMessage() const throw()
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
  if (this->comment.getLength() > 0) {
    msg << S(".\n");
    msg << this->comment;
  }
  return msg.str().c_str();
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
String MemoryException::getErrorMessage() const throw()
{
  String msg = S("Memory Exception: ");
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
  if (this->comment.getLength() > 0) {
    msg += S("\n");
    msg += this->comment;
  }
  return msg;
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
String InvalidArgumentException::getErrorMessage() const throw()
{
  String msg = String(S("Invalid Argument Exception (")) + this->argumentName;
  if (this->argumentValue.getLength() > 0) {
    msg += S(" = ");
    msg += this->argumentValue;
  }
  msg += S(")");
  if (this->comment.getLength() > 0) {
    msg += S("\n");
    msg += this->comment;
  }
  return msg;
}

} // namespace

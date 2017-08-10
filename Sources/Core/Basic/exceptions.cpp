/**
 * @file Core/Basic/exceptions.cpp
 * Contains the implementation of exception classes.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Basic
{

//==============================================================================
// Member Functions

Str Exception::getVerboseErrorMessage() const throw()
{
  return this->getErrorMessage() + STR("\nLocation:\n") + this->functionName + STR("\n") +
      this->sourceFile + STR(" : ") + std::to_string(this->lineNumber);
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Str FileException::getErrorMessage() const throw()
{
  StrStream msg;
  msg << STR("File Exception: ");
  switch (this->operation) {
    case CHR('o'):
      msg << STR("Couldn't open file: ");
      break;
    case CHR('c'):
      msg << STR("Couldn't close file: ");
      break;
    case CHR('r'):
      msg << STR("Couldn't read from file: ");
      break;
    case CHR('w'):
      msg << STR("Couldn't write to file: ");
      break;
    default:
      // this should never be reached
      ASSERT(false);
      break;
  }
  msg << this->fileName;
  msg << STR(".");
  if (this->comment.size() > 0) {
    msg << STR(".\n");
    msg << this->comment;
  }
  return msg.str();
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Str MemoryException::getErrorMessage() const throw()
{
  Str msg = STR("Memory Exception: ");
  switch (this->operation) {
    case CHR('a'):
      msg += STR("Couldn't allocate memory block.");
      break;
    case CHR('f'):
      msg += STR("Couldn't free memory block.");
      break;
    default:
      // this should never be reached
      ASSERT(false);
      break;
  }
  if (this->comment.size() > 0) {
    msg += STR("\n");
    msg += this->comment;
  }
  return msg;
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Str InvalidArgumentException::getErrorMessage() const throw()
{
  Str msg = STR("Invalid Argument Exception (") + this->argumentName;
  if (this->argumentValue.size() > 0) {
    msg += STR(" = ");
    msg += this->argumentValue;
  }
  msg += STR(")");
  if (this->comment.size() > 0) {
    msg += STR("\n");
    msg += this->comment;
  }
  return msg;
}

} } // namespace

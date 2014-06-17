/**
 * @file Core/Basic/exceptions.cpp
 * Contains the implementation of exception classes.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
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
// Static Variables

Str Exception::_message;


//==============================================================================
// Member Functions

/**
 * @return Returns a pointer to a string containing the error message.
 */
Char const* FileException::getErrorMessage() const throw()
{
  switch (this->operation) {
    case CHR('o'):
      Exception::_message = STR("Couldn't open file: ");
      break;
    case CHR('c'):
      Exception::_message = STR("Couldn't close file: ");
      break;
    case CHR('r'):
      Exception::_message = STR("Couldn't read from file: ");
      break;
    case CHR('w'):
      Exception::_message = STR("Couldn't write to file: ");
      break;
    default:
      // this should never be reached
      ASSERT(false);
      break;
  }
  Exception::_message += this->fileName;
  Exception::_message += STR(".");
  if (this->comment.size() > 0) {
    Exception::_message += STR(".\n");
    Exception::_message += this->comment;
  }
  return Exception::_message.c_str();
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Char const* MemoryException::getErrorMessage() const throw()
{
  switch (this->operation) {
    case CHR('a'):
      Exception::_message = STR("Couldn't allocate memory block.");
      break;
    case CHR('f'):
      Exception::_message = STR("Couldn't free memory block.");
      break;
    default:
      // this should never be reached
      ASSERT(false);
      break;
  }
  if (this->comment.size() > 0) {
    Exception::_message += STR("\n");
    Exception::_message += this->comment;
  }
  return Exception::_message.c_str();
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Char const* InvalidArgumentException::getErrorMessage() const throw()
{
  Exception::_message = STR("Invalid Argument (");
  Exception::_message += this->argumentName;
  if (this->argumentValue.size() > 0) {
    Exception::_message += STR(" = ");
    Exception::_message += this->argumentValue;
  }
  Exception::_message += STR(")");
  if (this->location.size() > 0) {
    Exception::_message += STR("\nLocation: ");
    Exception::_message += this->location;
  }
  if (this->comment.size() > 0) {
    Exception::_message += STR("\n");
    Exception::_message += this->comment;
  }
  return Exception::_message.c_str();
}


/**
 * @return Returns a pointer to a string containing the error message.
 */
Char const* GeneralException::getErrorMessage() const throw()
{
  Exception::_message = this->message;
  if (this->location.size() > 0) {
    Exception::_message += STR("\nLocation: ");
    Exception::_message += this->location;
  }
  return Exception::_message.c_str();
}

} } // namespace

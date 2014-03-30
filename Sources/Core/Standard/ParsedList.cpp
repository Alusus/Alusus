/**
 * @file Core/Standard/ParsedList.cpp
 * Contains the implementation of class Standard::ParsedList.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"
#include <stdarg.h>

namespace Core { namespace Standard
{

//==============================================================================
// Constructors

ParsedList::ParsedList(Word pid, Int cnt, IdentifiableObject *first, ...) : ParsedItem(pid)
{
  va_list objects;
  va_start(objects, first);
  if (cnt <= 0) {
    throw InvalidArgumentException(STR("cnt"),
                                   STR("Core::Standard::ParsedList::ParsedList"),
                                   STR("Should be > 0."));
  }
  this->pushElement(SharedPtr<IdentifiableObject>(first));
  for (Int i = 0; i < cnt-1; ++i) {
    this->pushElement(SharedPtr<IdentifiableObject>(va_arg(objects, IdentifiableObject*)));
  }
  va_end(objects);
}


ParsedList::ParsedList(Word pid, Int cnt, const SharedPtr<IdentifiableObject> *first, ...) : ParsedItem(pid)
{
  va_list objects;
  va_start(objects, first);
  if (cnt <= 0) {
    throw InvalidArgumentException(STR("cnt"),
                                   STR("Core::Standard::ParsedList::ParsedList"),
                                   STR("Should be > 0."));
  }
  this->pushElement(*first);
  for (Int i = 0; i < cnt-1; ++i) {
    this->pushElement(*va_arg(objects, SharedPtr<IdentifiableObject>*));
  }
  va_end(objects);
}


ParsedList::ParsedList(Word pid, Int l, Int c, Int cnt, IdentifiableObject *first, ...) : ParsedItem(pid, l, c)
{
  va_list objects;
  va_start(objects, first);
  if (cnt <= 0) {
    throw InvalidArgumentException(STR("cnt"),
                                   STR("Core::Standard::ParsedList::ParsedList"),
                                   STR("Should be > 0."));
  }
  this->pushElement(SharedPtr<IdentifiableObject>(first));
  for (Int i = 0; i < cnt-1; ++i) {
    this->pushElement(SharedPtr<IdentifiableObject>(va_arg(objects, IdentifiableObject*)));
  }
  va_end(objects);
}


ParsedList::ParsedList(Word pid, Int l, Int c, Int cnt, const SharedPtr<IdentifiableObject> *first, ...) :
  ParsedItem(pid, l, c)
{
  va_list objects;
  va_start(objects, first);
  if (cnt <= 0) {
    throw InvalidArgumentException(STR("cnt"),
                                   STR("Core::Standard::ParsedList::ParsedList"),
                                   STR("Should be > 0."));
  }
  this->pushElement(*first);
  for (Int i = 0; i < cnt-1; ++i) {
    this->pushElement(*va_arg(objects, const SharedPtr<IdentifiableObject>*));
  }
  va_end(objects);
}

} } // namespace

/**
 * @file Core/Basic/arguments.h
 * Contains definitions for arguments macros and classes.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_ARGUMENTS_H
#define CORE_BASIC_ARGUMENTS_H

namespace Core::Basic
{

/**
 * @brief The way the value of an Argument is held.
 * @ingroup basic_utils
 */
s_enum(ArgumentType, PTR, REF, SHARED);

/**
 * @brief A structure to hold different arguments used in initializer lists.
 * @ingroup basic_utils
 * CTYPE is the type of the argument's value.
 */
template <class CTYPE> struct Argument
{
  ArgumentType type;
  Char const *id;

  CTYPE *ptr;
  CTYPE const *ref;
  SharedPtr<CTYPE> shared;

  Argument(Char const *i, CTYPE *v) : id(i), ptr(v), ref(0), type(ArgumentType::PTR) {}
  Argument(Char const *i, CTYPE const &v) : id(i), ptr(0), ref(&v), type(ArgumentType::REF) {}
  Argument(Char const *i, SharedPtr<CTYPE> const &v)
    : id(i), ptr(0), ref(0), shared(v), type(ArgumentType::SHARED) {}
};

} // namespace

#endif

/**
 * @file Core/Basic/arguments.h
 * Contains definitions for arguments macros and classes.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_ARGUMENTS_H
#define CORE_BASIC_ARGUMENTS_H

namespace Core::Basic
{

/**
 * @brief A structure to hold different arguments used in initializer lists.
 * @ingroup basic_utils
 */
struct Argument
{
  s_enum(Type, PTR, REF, SHARED);

  Argument::Type type;
  Char const *id;

  TiObject *ptr;
  TiObject const *ref;
  SharedPtr<TiObject> shared;

  Argument(Char const *i, TiObject *v) : id(i), ptr(v), ref(0), type(Argument::Type::PTR) {}
  Argument(Char const *i, TiObject const &v) : id(i), ptr(0), ref(&v), type(Argument::Type::REF) {}
  Argument(Char const *i, SharedPtr<TiObject> const &v)
    : id(i), ptr(0), ref(0), shared(v), type(Argument::Type::SHARED) {}
};

} // namespace

#endif

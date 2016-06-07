/**
 * @file Core/Basic/arguments.h
 * Contains definitions for arguments macros and classes.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_ARGUMENTS_H
#define CORE_BASIC_ARGUMENTS_H

namespace Core { namespace Basic
{

/**
 * @brief A structure to hold different arguments used in initializer lists.
 * @ingroup basic_utils
 */
template <class TID> struct Argument
{
  TID id;

  Int intVal;
  Bool boolVal;
  Char const *strVal;
  SharedPtr<TiObject> ioVal;

  Argument(TID i, Int v) : id(i), intVal(v) {}
  Argument(TID i, Word v) : id(i), intVal(reinterpret_cast<Int>(v)) {}
  Argument(TID i, Bool v) : id(i), boolVal(v) {}
  Argument(TID i, Char const *v) : id(i), strVal(v) {}
  Argument(TID i, SharedPtr<TiObject> const &v) : id(i), ioVal(v) {}
};


/**
 * @brief Search for an argument with the given id.
 * @return A pointer to the argument, or null if it wasn't found.
 * @ingroup basic_utils
 */
template <class TID> const Argument<TID>* findArgument(const std::initializer_list<Argument<TID>> &args, TID id)
{
  for (const Argument<TID> &arg : args) if (arg.id == id) return &arg;
  return 0;
}

} } // namespace

#endif

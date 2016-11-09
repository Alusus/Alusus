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

s_enum(ArgumentType, INT, WORD, BOOL, STR, TI_PTR, TI_REF, TI_SHARED);

/**
 * @brief A structure to hold different arguments used in initializer lists.
 * @ingroup basic_utils
 */
template <class TID> struct Argument
{
  ArgumentType type;
  TID id;

  Int intVal;
  Bool boolVal;
  Char const *strVal;
  TiObject *tiPtr;
  TiObject const *tiRef;
  SharedPtr<TiObject> tiShared;

  Argument(TID i, Int v) : id(i), intVal(v), type(ArgumentType::INT) {}
  Argument(TID i, Word v) : id(i), intVal(static_cast<Int>(v)), type(ArgumentType::WORD) {}
  Argument(TID i, Bool v) : id(i), boolVal(v), type(ArgumentType::BOOL) {}
  Argument(TID i, Char const *v) : id(i), strVal(v), type(ArgumentType::STR) {}
  Argument(TID i, TiObject *v) : id(i), tiPtr(v), type(ArgumentType::TI_PTR) {}
  Argument(TID i, TiObject const &v) : id(i), tiRef(&v), type(ArgumentType::TI_REF) {}
  Argument(TID i, SharedPtr<TiObject> const &v) : id(i), tiShared(v), type(ArgumentType::TI_SHARED) {}
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

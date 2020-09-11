/**
 * @file Core/Basic/type_names.h
 * Contains definitions for type name string representation.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TYPENAMES_H
#define CORE_BASIC_TYPENAMES_H

template <typename... TYPES> struct TypeName;

// Type list.
template <typename TYPE, typename... REST> struct TypeName<TYPE, REST...>
{
  static std::string get()
  {
    return TypeName<TYPE>::get() + S(",") + TypeName<REST...>::get();
  }
};

// Pointer type.
template <class T> struct TypeName<T*>
{
    static std::string get() { return TypeName<T>::get() + S("*"); }
};

// Reference type.
template <class T> struct TypeName<T&>
{
    static std::string get() { return TypeName<T>::get() + S("&"); }
};

// Const type.
template <class T> struct TypeName<T const>
{
    static std::string get() { return TypeName<T>::get() + S(" const"); }
};

// Function type.
template <class T, class ...R> struct TypeName<T(R...)>
{
  static std::string get() { return TypeName<T>::get() + S("(") + TypeName<R...>::get() + S(")"); }
};

// Default, assume TiObject
template <class T> struct TypeName<T>
{
  static std::string get() { return T::getTypeInfo()->getUniqueName(); }
};

template <> struct TypeName<>
{
  static std::string get()
  {
    return "";
  }
};


#define DEFINE_TYPE_NAME(type, name) \
  template <> struct TypeName<type> \
  { \
    static std::string get() { return S(name); } \
  }
#define DEFINE_TEMPLATE_TYPE_NAME(type, name) \
  template <class T> struct TypeName<type<T>> \
  { \
      static std::string get() { return S(name) S("<") + TypeName<T>::get() + S(">"); } \
  }

DEFINE_TYPE_NAME(void, "void");

DEFINE_TYPE_NAME(Brl::Bool, "alusus.org/Brl/Brl.Bool");
DEFINE_TYPE_NAME(Brl::Int, "alusus.org/Brl/Brl.Int");
DEFINE_TYPE_NAME(Brl::ShortInt, "alusus.org/Brl/Brl.ShortInt");
DEFINE_TYPE_NAME(Brl::LongInt, "alusus.org/Brl/Brl.LongInt");
DEFINE_TYPE_NAME(Brl::Word, "alusus.org/Brl/Brl.Word");
DEFINE_TYPE_NAME(Brl::ShortWord, "alusus.org/Brl/Brl.ShortWord");
DEFINE_TYPE_NAME(Brl::LongWord, "alusus.org/Brl/Brl.LongWord");
DEFINE_TYPE_NAME(Brl::Float, "alusus.org/Brl/Brl.Float");
DEFINE_TYPE_NAME(Brl::Double, "alusus.org/Brl/Brl.Double");
DEFINE_TYPE_NAME(Brl::Char, "alusus.org/Brl/Brl.Char");
DEFINE_TYPE_NAME(Core::Basic::Str, "alusus.org/Core/Core.Basic.Str");
DEFINE_TYPE_NAME(Core::Basic::StrStream, "alusus.org/Core/Core.Basic.StrStream");

DEFINE_TEMPLATE_TYPE_NAME(std::shared_ptr, "std::shared_ptr");
DEFINE_TEMPLATE_TYPE_NAME(std::vector, "std::vector");
DEFINE_TEMPLATE_TYPE_NAME(std::function, "std::function");

namespace Core { namespace Basic {
  template<class T> class SharedPtr;
  template<class T> class WeakPtr;
} }
DEFINE_TEMPLATE_TYPE_NAME(Core::Basic::SharedPtr, "alusus.org/Core/Core.Basic.SharedPtr");
DEFINE_TEMPLATE_TYPE_NAME(Core::Basic::WeakPtr, "alusus.org/Core/Core.Basic.WeakPtr");

#endif

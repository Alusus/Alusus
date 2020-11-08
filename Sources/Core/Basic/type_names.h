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
  static Srl::String get()
  {
    return TypeName<TYPE>::get() + S(",") + TypeName<REST...>::get();
  }
};

// Pointer type.
template <class T> struct TypeName<T*>
{
    static Srl::String get() { return TypeName<T>::get() + S("*"); }
};

// Reference type.
template <class T> struct TypeName<T&>
{
    static Srl::String get() { return TypeName<T>::get() + S("&"); }
};

// Const type.
template <class T> struct TypeName<T const>
{
    static Srl::String get() { return TypeName<T>::get() + S(" const"); }
};

// Function type.
template <class T, class ...R> struct TypeName<T(R...)>
{
  static Srl::String get() { return TypeName<T>::get() + S("(") + TypeName<R...>::get() + S(")"); }
};

// Default, assume TiObject
template <class T> struct TypeName<T>
{
  static Srl::String get() { return T::getTypeInfo()->getUniqueName(); }
};

template <> struct TypeName<>
{
  static Srl::String get()
  {
    return "";
  }
};


#define DEFINE_TYPE_NAME(type, name) \
  template <> struct TypeName<type> \
  { \
    static Srl::String get() { return S(name); } \
  }
#define DEFINE_TEMPLATE_TYPE_NAME(type, name) \
  template <class T> struct TypeName<type<T>> \
  { \
      static Srl::String get() { return Srl::String(S(name) S("<")) + TypeName<T>::get() + S(">"); } \
  }
#define DEFINE_TEMPLATE2_TYPE_NAME(type, name) \
  template <class T1, class T2> struct TypeName<type<T1, T2>> \
  { \
      static Srl::String get() { \
        return Srl::String(S(name) S("<")) + TypeName<T1>::get() + S(",") + TypeName<T2>::get() + S(">"); \
      } \
  }

DEFINE_TYPE_NAME(void, "void");

DEFINE_TYPE_NAME(Srl::Bool, "alusus.org/Srl/Srl.Bool");
DEFINE_TYPE_NAME(Srl::Int, "alusus.org/Srl/Srl.Int");
DEFINE_TYPE_NAME(Srl::ShortInt, "alusus.org/Srl/Srl.ShortInt");
DEFINE_TYPE_NAME(Srl::LongInt, "alusus.org/Srl/Srl.LongInt");
DEFINE_TYPE_NAME(Srl::Word, "alusus.org/Srl/Srl.Word");
DEFINE_TYPE_NAME(Srl::ShortWord, "alusus.org/Srl/Srl.ShortWord");
DEFINE_TYPE_NAME(Srl::LongWord, "alusus.org/Srl/Srl.LongWord");
DEFINE_TYPE_NAME(Srl::Float, "alusus.org/Srl/Srl.Float");
DEFINE_TYPE_NAME(Srl::Double, "alusus.org/Srl/Srl.Double");
DEFINE_TYPE_NAME(Srl::Char, "alusus.org/Srl/Srl.Char");
DEFINE_TYPE_NAME(Srl::StrStream, "alusus.org/Srl/Srl.StrStream");
DEFINE_TYPE_NAME(Srl::String, "alusus.org/Srl/Srl.String");
DEFINE_TYPE_NAME(Core::Basic::Str, "alusus.org/Core/Core.Basic.Str");

DEFINE_TEMPLATE_TYPE_NAME(Srl::Array, "alusus.org/Srl/Srl.Array");
DEFINE_TEMPLATE2_TYPE_NAME(Srl::Map, "alusus.org/Srl/Srl.Array");

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

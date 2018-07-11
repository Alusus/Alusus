/**
 * @file Core/Basic/type_names.h
 * Contains definitions for type name string representation.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
    return TypeName<TYPE>::get() + "," + TypeName<REST...>::get();
  }
};

// Pointer type.
template <class T> struct TypeName<T*>
{
    static std::string get() { return TypeName<T>::get() + "*"; }
};

// Reference type.
template <class T> struct TypeName<T&>
{
    static std::string get() { return TypeName<T>::get() + "&"; }
};

// Const type.
template <class T> struct TypeName<T const>
{
    static std::string get() { return TypeName<T>::get() + " const"; }
};

// Function type.
template <class T, class ...R> struct TypeName<T(R...)>
{
  static std::string get() { return TypeName<T>::get() + "(" + TypeName<R...>::get() + ")"; }
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
    static std::string get() { return name; } \
  }
#define DEFINE_TEMPLATE_TYPE_NAME(type, name) \
  template <class T> struct TypeName<type<T>> \
  { \
      static std::string get() { return name "<" + TypeName<T>::get() + ">"; } \
  }

DEFINE_TYPE_NAME(void, "void");

DEFINE_TYPE_NAME(Core::Basic::Bool, "alusus.net/Core/Core.Basic.Bool");
DEFINE_TYPE_NAME(Core::Basic::Int, "alusus.net/Core/Core.Basic.Int");
DEFINE_TYPE_NAME(Core::Basic::ShortInt, "alusus.net/Core/Core.Basic.ShortInt");
DEFINE_TYPE_NAME(Core::Basic::LongInt, "alusus.net/Core/Core.Basic.LongInt");
DEFINE_TYPE_NAME(Core::Basic::Word, "alusus.net/Core/Core.Basic.Word");
DEFINE_TYPE_NAME(Core::Basic::ShortWord, "alusus.net/Core/Core.Basic.ShortWord");
DEFINE_TYPE_NAME(Core::Basic::LongWord, "alusus.net/Core/Core.Basic.LongWord");
DEFINE_TYPE_NAME(Core::Basic::Float, "alusus.net/Core/Core.Basic.Float");
DEFINE_TYPE_NAME(Core::Basic::Double, "alusus.net/Core/Core.Basic.Double");
DEFINE_TYPE_NAME(Core::Basic::Char, "alusus.net/Core/Core.Basic.Char");
DEFINE_TYPE_NAME(Core::Basic::Str, "alusus.net/Core/Core.Basic.Str");
DEFINE_TYPE_NAME(Core::Basic::StrStream, "alusus.net/Core/Core.Basic.StrStream");

DEFINE_TEMPLATE_TYPE_NAME(std::shared_ptr, "std::shared_ptr");
DEFINE_TEMPLATE_TYPE_NAME(std::vector, "std::vector");
DEFINE_TEMPLATE_TYPE_NAME(std::function, "std::function");

namespace Core { namespace Basic {
  template<class T> class SharedPtr;
  template<class T> class WeakPtr;
} }
DEFINE_TEMPLATE_TYPE_NAME(Core::Basic::SharedPtr, "alusus.net/Core/Core.Basic.SharedPtr");
DEFINE_TEMPLATE_TYPE_NAME(Core::Basic::WeakPtr, "alusus.net/Core/Core.Basic.WeakPtr");

#endif

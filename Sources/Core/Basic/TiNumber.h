/**
 * @file Core/Basic/TiNumber.h
 * Contains the header of class Core::Basic::TiNumber.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TINUMBER_H
#define CORE_BASIC_TINUMBER_H

namespace Core { namespace Basic
{

/**
 * @brief An identifiable object that holds a number.
 * @ingroup basic_datatypes
 */
template<class T, class P> class TiNumber : public P
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiNumber, P, "Core.Basic", "Core", "alusus.org", (T, P));


  //============================================================================
  // Member Variables

  private: T value;


  //============================================================================
  // Constructor

  public: TiNumber(T v=0) : value(v)
  {
  }

  public: static SharedPtr<TiNumber<T, P>> create(T v=0)
  {
    return newSrdObj<TiNumber<T, P>>(v);
  }


  //============================================================================
  // Operators

  public: TiNumber<T, P>& operator=(T v)
  {
    this->value = v;
    return *this;
  }

  public: TiNumber<T, P> operator+(T v)
  {
    return TiNumber<T, P>(this->value + v);
  }

  public: TiNumber<T, P>& operator+=(T v)
  {
    this->value += v;
    return *this;
  }

  public: TiNumber<T, P> operator-(T v)
  {
    return TiNumber<T, P>(this->value - v);
  }

  public: TiNumber<T, P>& operator-=(T v)
  {
    this->value -= v;
    return *this;
  }

  public: TiNumber<T, P> operator*(T v)
  {
    return TiNumber<T, P>(this->value * v);
  }

  public: TiNumber<T, P>& operator*=(T v)
  {
    this->value *= v;
    return *this;
  }

  public: TiNumber<T, P> operator/(T v)
  {
    return TiNumber<T, P>(this->value / v);
  }

  public: TiNumber<T, P>& operator/=(T v)
  {
    this->value /= v;
    return *this;
  }

  public: TiNumber<T, P> operator%(T v)
  {
    return TiNumber<T, P>(this->value % v);
  }

  public: TiNumber<T, P>& operator%=(T v)
  {
    this->value %= v;
    return *this;
  }

  public: TiNumber<T, P> operator<<(Int v)
  {
    return TiNumber<T, P>(this->value << v);
  }

  public: TiNumber<T, P>& operator<<=(Int v)
  {
    this->value <<= v;
    return *this;
  }

  public: TiNumber<T, P> operator>>(Int v)
  {
    return TiNumber<T, P>(this->value >> v);
  }

  public: TiNumber<T, P>& operator>>=(Int v)
  {
    this->value >>= v;
    return *this;
  }

  public: TiNumber<T, P>& operator++()
  {
    this->value++;
    return *this;
  }

  public: TiNumber<T, P>& operator--()
  {
    this->value--;
    return *this;
  }

  public: operator T() const
  {
    return this->value;
  }


  //============================================================================
  // Member Functions

  public: void set(T v)
  {
    this->value = v;
  }

  public: T get() const
  {
    return this->value;
  }

}; // class


//==============================================================================
// Typedefs

typedef TiNumber<Int, TiObject> TiInt;

typedef TiNumber<Word, TiObject> TiWord;

typedef TiNumber<Float, TiObject> TiFloat;

} } // namespace

#endif

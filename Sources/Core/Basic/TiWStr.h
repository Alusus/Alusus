/**
 * @file Core/Basic/TiWStr.h
 * Contains the header of class Core::Basic::TiWStr.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIWSTR_H
#define CORE_BASIC_TIWSTR_H

namespace Core::Basic
{

/**
 * @brief An identifiable object that holds a string value.
 * @ingroup basic_datatypes
 */
template <class P> class TiWStrBase : public P
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiWStrBase, P, "Core.Data", "Core", "alusus.org", (P));


  //============================================================================
  // Member Variables

  private: WStr value;


  //============================================================================
  // Constructors

  public: TiWStrBase()
  {
  }

  public: TiWStrBase(WChar const *v) : value(v)
  {
  }

  public: TiWStrBase(WChar const *v, Word c) : value(v, c)
  {
  }

  public: TiWStrBase(Char const *v, Word c = 0) : value(v, c)
  {
  }

  public: static SharedPtr<TiWStrBase<P>> create(WChar const *v)
  {
    return newSrdObj<TiWStrBase<P>>(v);
  }

  public: static SharedPtr<TiWStrBase<P>> create(WChar const *v, Word c)
  {
    return newSrdObj<TiWStrBase<P>>(v, c);
  }

  public: static SharedPtr<TiWStrBase<P>> create(Char const *v, Word c)
  {
    return newSrdObj<TiWStrBase<P>>(v, c);
  }


  //============================================================================
  // Operators

  public: TiWStrBase<P>& operator=(TiWStrBase<P> const &v)
  {
    this->value = v.value;
    return *this;
  }

  public: TiWStrBase<P>& operator=(WChar const *v)
  {
    this->value = v;
    return *this;
  }

  public: TiWStrBase<P>& operator=(Char const *v)
  {
    this->value = v;
    return *this;
  }

  public: operator WChar const*() const
  {
    return this->value;
  }

  public: Bool operator==(TiWStrBase<P> const &s) const
  {
    return this->value.compare(s.value) == 0;
  }

  public: Bool operator==(WChar const *s) const
  {
    return this->value.compare(s) == 0;
  }

  public: Bool operator!=(TiWStrBase<P> const &s) const
  {
    return this->value.compare(s.value) != 0;
  }

  public: Bool operator!=(WChar const *s) const
  {
    return this->value.compare(s) != 0;
  }

  public: Bool operator>(TiWStrBase<P> const &s) const
  {
    return this->value.compare(s.value) > 0;
  }

  public: Bool operator>(WChar const *s) const
  {
    return this->value.compare(s) > 0;
  }

  public: Bool operator<(TiWStrBase<P> const &s) const
  {
    return this->value.compare(s.value) < 0;
  }

  public: Bool operator<(WChar const *s) const
  {
    return this->value.compare(s) < 0;
  }


  //============================================================================
  // Member Functions

  public: void set(WChar const *v)
  {
    this->value = v;
  }

  public: void set(WChar const *v, Word c)
  {
    this->value.assign(v, c);
  }

  public: WChar const* get() const
  {
    return this->value;
  }

  public: WStr const& getWStr() const
  {
    return this->value;
  }

}; // class


//==============================================================================
// Typedefs

typedef TiWStrBase<TiObject> TiWStr;

} // namespace

#endif

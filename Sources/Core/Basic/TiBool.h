/**
 * @file Core/Basic/TiBool.h
 * Contains the header of class Core::Basic::TiBool.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIBOOL_H
#define CORE_BASIC_TIBOOL_H

namespace Core { namespace Basic
{

/**
 * @brief An identifiable object that holds a boolean.
 * @ingroup basic_datatypes
 */
template<class P> class TiBoolBase : public P
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(TiBoolBase, P, "Core.Basic", "Core", "alusus.org", (P));


  //============================================================================
  // Member Variables

  private: Bool value;


  //============================================================================
  // Constructor

  public: TiBoolBase(Bool v=false) : value(v)
  {
  }

  public: static SharedPtr<TiBoolBase<P>> create(Bool v=0)
  {
    return newSrdObj<TiBoolBase<P>>(v);
  }


  //============================================================================
  // Operators

  public: TiBoolBase<P>& operator=(Bool v)
  {
    this->value = v;
    return *this;
  }

  public: operator Bool() const
  {
    return this->value;
  }


  //============================================================================
  // Member Functions

  public: void set(Bool v)
  {
    this->value = v;
  }

  public: Bool get() const
  {
    return this->value;
  }

}; // class


//==============================================================================
// Typedefs

typedef TiBoolBase<TiObject> TiBool;

} } // namespace

#endif

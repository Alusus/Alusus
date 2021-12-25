/**
 * @file Core/Basic/TiPtr.h
 * Contains the header of class Core::Basic::TiPtr.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIPTR_H
#define CORE_BASIC_TIPTR_H

namespace Core::Basic
{

/**
 * @brief An identifiable object that holds a ptr.
 * @ingroup basic_datatypes
 */
template<class P> class TiPtrBase : public P
{
  //============================================================================
  // Type Info

  typedef TiPtrBase<P> _MyType;
  TEMPLATE_TYPE_INFO(TiPtrBase, P, "Core.Basic", "Core", "alusus.org", (P));
  OBJECT_FACTORY(_MyType);


  //============================================================================
  // Member Variables

  private: void *value;


  //============================================================================
  // Constructor

  public: TiPtrBase(void *v=0) : value(v)
  {
  }

  public: static SharedPtr<TiPtrBase<P>> create(void *v=0)
  {
    return newSrdObj<TiPtrBase<P>>(v);
  }


  //============================================================================
  // Operators

  public: TiPtrBase<P>& operator=(void *v)
  {
    this->value = v;
    return *this;
  }

  public: operator void*() const
  {
    return this->value;
  }


  //============================================================================
  // Member Functions

  public: void set(void *v)
  {
    this->value = v;
  }

  public: void* get() const
  {
    return this->value;
  }

}; // class


//==============================================================================
// Typedefs

typedef TiPtrBase<TiObject> TiPtr;

} // namespace

#endif

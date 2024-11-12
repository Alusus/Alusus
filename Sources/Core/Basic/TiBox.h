/**
 * @file Core/Basic/TiBox.h
 * Contains the header of class Core::Basic::TiBox.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_TIBOX_H
#define CORE_BASIC_TIBOX_H

namespace Core::Basic
{

template<class CTYPE, class PTYPE=TiObject> class TiBox : public PTYPE
{
  //============================================================================
  // Type Info

  typedef TiBox<CTYPE, PTYPE> _MyType;
  TEMPLATE_TYPE_INFO(TiBox, PTYPE, "Core.Basic", "Core", "alusus.org", (CTYPE, PTYPE));
  OBJECT_FACTORY(_MyType);


  //============================================================================
  // Member Variables

  private: CTYPE obj;


  //============================================================================
  // Constructors & Destructor

  public: TiBox() : obj(0) {}

  public: TiBox(CTYPE const &o) : obj(o)
  {
  }

  public: virtual ~TiBox()
  {
  }

  public: static SharedPtr<TiBox<CTYPE, PTYPE>> create(CTYPE const &o)
  {
    return newSrdObj<TiBox<CTYPE, PTYPE>>(o);
  }


  //============================================================================
  // Member Functions

  public: void set(CTYPE const &o)
  {
    this->obj = o;
  }

  public: CTYPE const& get() const
  {
    return this->obj;
  }

}; // class

} // namespace

#endif

/**
 * @file Core/Basic/SharedBox.h
 * Contains the header of class Core::Basic::SharedBox.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_SHAREDBOX_H
#define CORE_BASIC_SHAREDBOX_H

namespace Core { namespace Basic
{

template<class CTYPE, class PTYPE=TiObject> class SharedBox : public PTYPE
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(SharedBox, PTYPE, "Core.Basic", "Core", "alusus.net", CTYPE, PTYPE);


  //============================================================================
  // Member Variables

  private: SharedPtr<CTYPE> obj;


  //============================================================================
  // Constructors & Destructor

  public: SharedBox() {}

  public: SharedBox(SharedPtr<CTYPE> const &o) : obj(o)
  {
  }


  //============================================================================
  // Member Functions

  public: void set(SharedPtr<CTYPE> const &o)
  {
    this->obj = o;
  }

  public: SharedPtr<CTYPE> const& get() const
  {
    return this->obj;
  }

}; // class


//==============================================================================
// Default Types

typedef SharedBox<TiObject> TioSharedBox;

} } // namespace

#endif

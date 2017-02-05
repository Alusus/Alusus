/**
 * @file Core/Basic/Box.h
 * Contains the header of class Core::Basic::Box.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_BOX_H
#define CORE_BASIC_BOX_H

namespace Core { namespace Basic
{

template<class CTYPE, class PTYPE=TiObject> class Box : public PTYPE
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(Box, PTYPE, "Core.Basic", "Core", "alusus.net", CTYPE, PTYPE);


  //============================================================================
  // Member Variables

  private: CTYPE *obj;


  //============================================================================
  // Constructors & Destructor

  Box() : obj(0) {}

  Box(CTYPE *o) : obj(o)
  {
  }


  //============================================================================
  // Member Functions

  public: void set(CTYPE *o)
  {
    this->obj = o;
  }

  public: CTYPE* get() const
  {
    return this->obj;
  }

}; // class

} } // namespace

#endif

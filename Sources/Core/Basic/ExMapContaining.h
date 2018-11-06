/**
 * @file Core/Basic/ExMapContaining.h
 * Contains the header of interface Basic::ExMapContaining.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_EXMAPCONTAINING_H
#define CORE_BASIC_EXMAPCONTAINING_H

namespace Core::Basic
{

template<class CTYPE> class ExMapContaining : public MapContaining<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(ExMapContaining, MapContaining<CTYPE>, "Core.Data", "Core", "alusus.net", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual TypeInfo* getElementNeededType(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Element not found"), name);
  }
  public: virtual TypeInfo* getElementNeededType(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual HoldMode getElementHoldMode(Char const *name) const
  {
    throw EXCEPTION(InvalidArgumentException, S("name"), S("Element not found"), name);
  }
  public: virtual HoldMode getElementHoldMode(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

}; // class

} // namespace

#endif

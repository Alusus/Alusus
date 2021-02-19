/**
 * @file Core/Basic/DynamicContaining.h
 * Contains the header of interface Basic::DynamicContaining.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_DYNAMICCONTAINING_H
#define CORE_BASIC_DYNAMICCONTAINING_H

namespace Core::Basic
{

template<class CTYPE> class DynamicContaining : public Containing<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(DynamicContaining, Containing<CTYPE>, "Core.Basic", "Core", "alusus.org", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual Int addElement(CTYPE *val)
  {
    throw EXCEPTION(GenericException, S("Not supported."));
  }

  public: virtual void insertElement(Int index, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual void removeElement(Int index)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual TypeInfo const* getElementsNeededType() const
  {
    throw EXCEPTION(GenericException, S("Not supported."));
  }

  public: virtual HoldMode getElementsHoldMode() const
  {
    throw EXCEPTION(GenericException, S("Not supported."));
  }

  public: virtual TypeInfo const* getElementNeededType(Int index) const
  {
    return this->getElementsNeededType();
  }

  public: virtual HoldMode getElementHoldMode(Int index) const
  {
    return this->getElementsHoldMode();
  }

}; // class

} // namespace

#endif

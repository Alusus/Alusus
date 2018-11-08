/**
 * @file Core/Basic/DynamicContaining.h
 * Contains the header of interface Basic::DynamicContaining.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_DYNAMICCONTAINING_H
#define CORE_BASIC_DYNAMICCONTAINING_H

namespace Core::Basic
{

template<class CTYPE> class DynamicContaining : public virtual Containing<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(DynamicContaining, Containing<CTYPE>, "Core.Data", "Core", "alusus.net", (CTYPE));


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

  public: virtual TypeInfo* getElementsNeededType() const
  {
    throw EXCEPTION(GenericException, S("Not supported."));
  }

  public: virtual HoldMode getElementsHoldMode() const
  {
    throw EXCEPTION(GenericException, S("Not supported."));
  }

  public: virtual TypeInfo* getElementNeededType(Int index) const
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

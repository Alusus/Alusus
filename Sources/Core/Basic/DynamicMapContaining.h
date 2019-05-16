/**
 * @file Core/Basic/DynamicMapContaining.h
 * Contains the header of interface Basic::DynamicMapContaining.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_DYNAMICMAPCONTAINING_H
#define CORE_BASIC_DYNAMICMAPCONTAINING_H

namespace Core::Basic
{

template<class CTYPE> class DynamicMapContaining : public MapContaining<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(DynamicMapContaining, MapContaining<CTYPE>, "Core.Data", "Core", "alusus.org", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual Int addElement(Char const *key, CTYPE *val)
  {
    throw EXCEPTION(GenericException, S("Not supported."));
  }

  public: virtual void insertElement(Int index, Char const *key, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual void removeElement(Int index)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }
  public: virtual Int removeElement(Char const *key)
  {
    throw EXCEPTION(InvalidArgumentException, S("key"), S("Key not found"), key);
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
  public: virtual TypeInfo* getElementNeededType(Char const *key) const
  {
    return this->getElementsNeededType();
  }

  public: virtual HoldMode getElementHoldMode(Int index) const
  {
    return this->getElementsHoldMode();
  }
  public: virtual HoldMode getElementHoldMode(Char const *key) const
  {
    return this->getElementsHoldMode();
  }

}; // class

} // namespace

#endif

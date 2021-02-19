/**
 * @file Core/Basic/Containing.h
 * Contains the header of interface Basic::Containing.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_CONTAINING_H
#define CORE_BASIC_CONTAINING_H

namespace Core::Basic
{

template<class CTYPE> class Containing : public TiInterface
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(Containing, TiInterface, "Core.Basic", "Core", "alusus.org", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual void setElement(Int index, CTYPE *val)
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual Word getElementCount() const
  {
    return 0;
  }

  public: virtual CTYPE* getElement(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual TypeInfo const* getElementNeededType(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

  public: virtual HoldMode getElementHoldMode(Int index) const
  {
    throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range"), index);
  }

}; // class

} // namespace

#endif

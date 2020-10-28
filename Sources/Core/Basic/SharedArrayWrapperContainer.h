/**
 * @file Core/Basic/SharedArrayWrapperContainer.h
 * Contains the header of class Core::Basic::SharedArrayWrapperContainer.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_SHAREDARRAYWRAPPERCONTAINER_H
#define CORE_BASIC_SHAREDARRAYWRAPPERCONTAINER_H

namespace Core::Basic
{

template<class CTYPE> class SharedArrayWrapperContainer : public TiObject, public Containing<CTYPE>
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(SharedArrayWrapperContainer, TiObject, "Core.Basic", "Core", "alusus.org", (CTYPE), (
    INHERITANCE_INTERFACES(Containing<CTYPE>)
  ));


  //============================================================================
  // Member Variables

  private: Array<SharedPtr<CTYPE>> *array;


  //============================================================================
  // Constructors & Destructor

  public: SharedArrayWrapperContainer(Array<SharedPtr<CTYPE>> *a) : array(a)
  {
    if (this->array == 0) {
      throw EXCEPTION(InvalidArgumentException, S("a"), S("Argument is null."));
    }
  }


  //============================================================================
  // Member Functions

  /// @name Setters and Getters
  /// @{

  public: void setArray(Array<SharedPtr<CTYPE>> *a)
  {
    this->array = a;
  }

  public: Array<SharedPtr<CTYPE>>* getArray() const
  {
    return this->array;
  }

  /// @}

  /// @name Containing Implementation
  /// @{

  public: virtual void setElement(Int index, CTYPE *val)
  {
    if (index < 0 || index > this->array->getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    } else if (index == this->array->getLength()) {
      this->array->add(getSharedPtr(val));
    } else {
      this->array->at(index) = getSharedPtr(val);
    }
  }

  public: virtual Word getElementCount() const
  {
    return this->array->getLength();
  }

  public: virtual CTYPE* getElement(Int index) const
  {
    if (index < 0 || index > this->array->getLength()) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    return this->array->at(index).get();
  }

  public: virtual TypeInfo const* getElementsNeededType() const
  {
    return CTYPE::getTypeInfo();
  }

  public: virtual HoldMode getElementsHoldMode() const
  {
    return HoldMode::PLAIN_REF;
  }

  /// @}

}; // class

} // namespace

#endif

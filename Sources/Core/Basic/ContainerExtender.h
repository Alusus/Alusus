/**
 * @file Core/Basic/ContainerExtender.h
 * Contains the header of class Core::Basic::ContainerExtender.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_BASIC_CONTAINEREXTENDER_H
#define CORE_BASIC_CONTAINEREXTENDER_H

namespace Core::Basic
{

template<class CTYPE, int PRECOUNT=0, int POSTCOUNT=0>
  class ContainerExtender : public TiObject, public Containing<TiObject>
{
  //============================================================================
  // Type Info

  TEMPLATE_TYPE_INFO(ContainerExtender, TiObject, "Core.Basic", "Core", "alusus.org", (CTYPE), (
    INHERITANCE_INTERFACES(Containing<CTYPE>)
  ));


  //============================================================================
  // Member Variables

  private: CTYPE *preItems[PRECOUNT];
  private: CTYPE *postItems[POSTCOUNT];
  private: Containing<CTYPE> *container;


  //============================================================================
  // Constructors & Destructor

  public: ContainerExtender(Containing<TiObject> *c) : container(c)
  {
    if (this->container == 0) {
      throw EXCEPTION(InvalidArgumentException, S("c"), S("Argument is null."));
    }
  }


  //============================================================================
  // Member Functions

  /// @name Setters and Getters
  /// @{

  public: void setContainer(Containing<CTYPE> *c)
  {
    this->container = c;
  }

  public: Containing<CTYPE>* getContainer() const
  {
    return this->container;
  }

  public: void setPreItem(Int index, CTYPE *item)
  {
    if (index < 0 || index >= PRECOUNT) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    this->preItems[index] = item;
  }

  public: CTYPE* getPreItem(Int index) const
  {
    if (index < 0 || index >= PRECOUNT) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    return this->preItems[index];
  }

  public: void setPostItem(Int index, CTYPE *item)
  {
    if (index < 0 || index >= POSTCOUNT) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    this->postItems[index] = item;
  }

  public: CTYPE* getPostItem(Int index) const
  {
    if (index < 0 || index >= POSTCOUNT) {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
    return this->postItems[index];
  }

  /// @}

  /// @name Containing Implementation
  /// @{

  public: virtual void setElement(Int index, CTYPE *val)
  {
    if (index >= 0 && index < PRECOUNT) {
      this->setPreItem(index, val);
    } else if (index >= PRECOUNT && index < PRECOUNT + this->container->getElementCount()) {
      this->container->setElement(index - PRECOUNT, val);
    } else if (index >= PRECOUNT + this->container->getElementCount() && index < this->getElementCount()) {
      this->setPostItem(index - PRECOUNT - this->container->getElementCount(), val);
    } else {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
  }

  public: virtual Word getElementCount() const
  {
    return PRECOUNT + this->container->getElementCount() + POSTCOUNT;
  }

  public: virtual CTYPE* getElement(Int index) const
  {
    if (index >= 0 && index < PRECOUNT) {
      return this->preItems[index];
    } else if (index >= PRECOUNT && index < PRECOUNT + this->container->getElementCount()) {
      return this->container->getElement(index - PRECOUNT);
    } else if (index >= PRECOUNT + this->container->getElementCount() && index < this->getElementCount()) {
      return this->postItems[index - PRECOUNT - this->container->getElementCount()];
    } else {
      throw EXCEPTION(InvalidArgumentException, S("index"), S("Out of range."), index);
    }
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

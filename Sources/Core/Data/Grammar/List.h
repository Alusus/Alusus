/**
 * @file Core/Data/Grammar/List.h
 * Contains the header of class Core::Data::Grammar::List.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_LIST_H
#define CORE_DATA_GRAMMAR_LIST_H

namespace Core::Data::Grammar
{

class List : public NbList, public Binding, public Inheriting
{
  //============================================================================
  // Type Info

  TYPE_INFO(List, NbList, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(Binding, Inheriting)
  ));
  OBJECT_FACTORY(List);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> baseRef;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (baseRef, Reference, SHARED_REF, setBaseRef(value), baseRef.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(List);

  IMPLEMENT_ATTR_CONSTRUCTOR(List);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(List);

  public: virtual ~List()
  {
    RESET_OWNED_SHAREDPTR(this->baseRef);
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  protected: virtual SharedPtr<TiObject> prepareForSet(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    if (inherited) {
      return cloneInherited(obj);
    } else {
      return obj;
    }
  }

  /// @}

  /// @name Property Accessors
  /// @{

  public: void setBaseRef(SharedPtr<Reference> const &p)
  {
    UPDATE_OWNED_SHAREDPTR(this->baseRef, p);
  }
  private: void setBaseRef(Reference *ref)
  {
    this->setBaseRef(getSharedPtr(ref));
  }

  public: SharedPtr<Reference> const& getBaseRef() const
  {
    return this->baseRef;
  }

  public: using NbList::setBase;

  /// @}

  /// @name Inheriting Implementation
  /// @{

  public: virtual Reference* getBaseReference() const
  {
    return this->baseRef.get();
  }

  public: virtual void setBase(TiObject *base)
  {
    List *baseList = ti_cast<List>(base);
    if (baseList == 0) {
      throw EXCEPTION(GenericException, S("Base reference points to an object of an invalid type."));
    }
    this->setBase(baseList);
  }

  public: virtual TiObject* getBase() const
  {
    return NbList::getBase();
  }

  /// @}

}; // class

} // namespace

#endif

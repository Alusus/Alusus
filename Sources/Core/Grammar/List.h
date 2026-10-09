/**
 * @file Core/Grammar/List.h
 * Contains the header of class Core::Grammar::List.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_GRAMMAR_LIST_H
#define CORE_GRAMMAR_LIST_H

namespace Core::Grammar
{

class List : public SharedListBase<TiObject, Node>, public Binding, public Inheriting
{
  //============================================================================
  // Type Info

  typedef SharedListBase<TiObject, Node> _MyBase;
  TYPE_INFO(List, _MyBase, "Core.Grammar", "Core", "alusus.org", (
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
    this->destruct();
  }


  //============================================================================
  // Member Functions

  /// @name Abstract Function Implementations
  /// @{

  protected: virtual SharedPtr<TiObject> prepareForSet(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    if (inherited) {
      return cloneInherited(obj.get());
    } else {
      return obj;
    }
  }

  protected: virtual void finalizeSet(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited, Bool newEntry
  ) {
    if (obj != 0 && obj->isDerivedFrom<Node>()) {
      obj.s_cast_get<Node>()->setOwner(this);
    }
  }

  protected: virtual void prepareForUnset(
    Int index, SharedPtr<TiObject> const &obj, Bool inherited
  ) {
    if (obj != 0 && obj->isDerivedFrom<Node>() && obj.s_cast_get<Node>()->getOwner() == this) {
      obj.s_cast_get<Node>()->setOwner(0);
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

  public: void setBase(List *b)
  {
    _MyBase::setBase(b);
  }

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
    return this->base;
  }

  /// @}

}; // class

} // namespace

#endif

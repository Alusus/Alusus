/**
 * @file Core/Data/Grammar/List.h
 * Contains the header of class Core::Data::Grammar::List.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_LIST_H
#define CORE_DATA_GRAMMAR_LIST_H

namespace Core::Data::Grammar
{

class List : public NbList, public virtual Binding, public virtual Initializable
{
  //============================================================================
  // Type Info

  TYPE_INFO(List, NbList, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Binding, Initializable)
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Grammar::Reference> baseRef;


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


  //============================================================================
  // Member Functions

  /// @name Property Accessors
  /// @{

  public: void setBaseRef(SharedPtr<Reference> const &p)
  {
    this->baseRef = p;
  }
  private: void setBaseRef(Reference *ref)
  {
    this->setBaseRef(getSharedPtr(ref));
  }

  public: SharedPtr<Reference> const& getBaseRef() const
  {
    return this->baseRef;
  }

  /// @}

  /// @name Initializable Implementation
  /// @{

  public: virtual void initialize(TiObject *context);

  /// @}

}; // class

} // namespace

#endif

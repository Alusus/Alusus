/**
 * @file Core/Data/Grammar/ReferenceTerm.h
 * Contains the header of class Core::Data::Grammar::ReferenceTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_REFERENCETERM_H
#define CORE_DATA_GRAMMAR_REFERENCETERM_H

namespace Core::Data::Grammar
{

// TODO: DOC

class ReferenceTerm : public Term, public virtual DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReferenceTerm, Term, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DataHaving)
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> reference;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Term,
    (reference, Reference, SHARED_REF, setReference(value), reference.get())
  );


  //============================================================================
  // Constructor & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ReferenceTerm);

  IMPLEMENT_ATTR_CONSTRUCTOR(ReferenceTerm);

  public: virtual ~ReferenceTerm()
  {
  }


  //============================================================================
  // Member Functions

  public: void setReference(SharedPtr<Reference> const &ref)
  {
    this->reference = ref;
  }

  private: void setReference(Reference *ref)
  {
    this->setReference(getSharedPtr(ref));
  }

  public: SharedPtr<Reference> const& getReference() const
  {
    return this->reference;
  }


  //============================================================================
  // DataHaving Implementation

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->reference != 0) Data::unsetIndexes(this->reference.get(), from, to);
  }

}; // class

} // namespace

#endif

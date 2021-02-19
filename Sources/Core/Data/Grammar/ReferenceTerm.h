/**
 * @file Core/Data/Grammar/ReferenceTerm.h
 * Contains the header of class Core::Data::Grammar::ReferenceTerm.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_REFERENCETERM_H
#define CORE_DATA_GRAMMAR_REFERENCETERM_H

namespace Core::Data::Grammar
{

// TODO: DOC

class ReferenceTerm : public Term
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReferenceTerm, Term, "Core.Data.Grammar", "Core", "alusus.org");
  OBJECT_FACTORY(ReferenceTerm);


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
    RESET_OWNED_SHAREDPTR(this->reference);
  }


  //============================================================================
  // Member Functions

  public: void setReference(SharedPtr<Reference> const &ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->reference, ref);
  }

  private: void setReference(Reference *ref)
  {
    this->setReference(getSharedPtr(ref));
  }

  public: SharedPtr<Reference> const& getReference() const
  {
    return this->reference;
  }

}; // class

} // namespace

#endif

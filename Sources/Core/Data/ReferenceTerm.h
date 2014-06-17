/**
 * @file Core/Data/ReferenceTerm.h
 * Contains the header of class Core::Data::ReferenceTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_REFERENCE_TERM_H
#define DATA_REFERENCE_TERM_H

namespace Core { namespace Data
{

// TODO: DOC

class ReferenceTerm : public Term, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReferenceTerm, Term, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Term, DataOwner);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> reference;


  //============================================================================
  // Constructor & Destructor

  public: ReferenceTerm(Char const *ref=0, Word f=0) : Term(f)
  {
    if (ref != 0) this->reference = ReferenceParser::parseQualifier(ref);
  }

  public: ReferenceTerm(const SharedPtr<Reference> &ref, Word f=0) : Term(f), reference(ref)
  {
  }

  public: virtual ~ReferenceTerm()
  {
  }

  public: static SharedPtr<ReferenceTerm> create(Char const *ref=0, Word f=0)
  {
    return std::make_shared<ReferenceTerm>(ref, f);
  }

  public: static SharedPtr<ReferenceTerm> create(const SharedPtr<Reference> &ref, Word f=0)
  {
    return std::make_shared<ReferenceTerm>(ref, f);
  }


  //============================================================================
  // Member Functions

  public: void setReference(const SharedPtr<Reference> &ref)
  {
    this->reference = ref;
  }

  public: void setReference(Char const *ref)
  {
    if (ref != 0) this->reference = ReferenceParser::parseQualifier(ref);
    else this->reference.reset();
  }

  public: const SharedPtr<Reference>& getReference() const
  {
    return this->reference;
  }


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->reference != 0) Data::unsetIndexes(this->reference.get(), from, to);
  }

}; // class

} } // namespace

#endif

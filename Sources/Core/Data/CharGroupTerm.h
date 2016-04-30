/**
 * @file Core/Data/CharGroupTerm.h
 * Contains the header of class Core::Data::CharGroupTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CHARGROUPTERM_H
#define CORE_DATA_CHARGROUPTERM_H

namespace Core { namespace Data
{

// TODO: DOC

/**
 * @brief Handles character group grammar terms.
 * @ingroup data_terms
 *
 * This class handles instances of character groups in grammar formulas.
 */
class CharGroupTerm : public Term, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(CharGroupTerm, Term, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Term, DataOwner);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> charGroupReference;


  //============================================================================
  // Constructor / Destructor

  public: CharGroupTerm(Char const *ref=0, Word f=0) : Term(f)
  {
    if (ref != 0) {
      this->charGroupReference = REF_PARSER->parseQualifier(ref);
      this->charGroupReference->setOwner(this);
    }
  }

  public: CharGroupTerm(const SharedPtr<Reference> &ref, Word f=0) : Term(f), charGroupReference(ref)
  {
    if (this->charGroupReference != 0) this->charGroupReference->setOwner(this);
  }

  public: virtual ~CharGroupTerm()
  {
    RESET_OWNED_SHAREDPTR(this->charGroupReference);
  }

  public: static SharedPtr<CharGroupTerm> create(Char const *ref=0, Word f=0)
  {
    return std::make_shared<CharGroupTerm>(ref, f);
  }

  public: static SharedPtr<CharGroupTerm> create(const SharedPtr<Reference> &ref, Word f=0)
  {
    return std::make_shared<CharGroupTerm>(ref, f);
  }


  //============================================================================
  // Member Functions

  public: void setReference(Char const *ref)
  {
    if (ref != 0) {
      UPDATE_OWNED_SHAREDPTR(this->charGroupReference, REF_PARSER->parseQualifier(ref));
    } else {
      RESET_OWNED_SHAREDPTR(this->charGroupReference);
    }
  }

  public: SharedPtr<Reference> const& getReference() const
  {
    return this->charGroupReference;
  }


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->charGroupReference != 0) Data::unsetIndexes(this->charGroupReference.get(), from, to);
  }

}; // class

} } // namespace

#endif

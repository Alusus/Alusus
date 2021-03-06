/**
 * @file Core/Data/Grammar/CharGroupTerm.h
 * Contains the header of class Core::Data::Grammar::CharGroupTerm.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_CHARGROUPTERM_H
#define CORE_DATA_GRAMMAR_CHARGROUPTERM_H

namespace Core::Data::Grammar
{

// TODO: DOC

/**
 * @brief Handles character group grammar terms.
 * @ingroup core_data_grammar
 *
 * This class handles instances of character groups in grammar formulas.
 */
class CharGroupTerm : public Term
{
  //============================================================================
  // Type Info

  TYPE_INFO(CharGroupTerm, Term, "Core.Data.Grammar", "Core", "alusus.org");
  OBJECT_FACTORY(CharGroupTerm);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> charGroupReference;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Term,
    (charGroupReference, Reference, SHARED_REF, setCharGroupReference(value), charGroupReference.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(CharGroupTerm);

  IMPLEMENT_ATTR_CONSTRUCTOR(CharGroupTerm);

  public: virtual ~CharGroupTerm()
  {
    RESET_OWNED_SHAREDPTR(this->charGroupReference);
  }


  //============================================================================
  // Member Functions

  public: void setCharGroupReference(SharedPtr<Reference> const &ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->charGroupReference, ref);
  }

  private: void setCharGroupReference(Reference *ref)
  {
    this->setCharGroupReference(getSharedPtr(ref));
  }

  public: SharedPtr<Reference> const& getCharGroupReference() const
  {
    return this->charGroupReference;
  }

}; // class

} // namespace

#endif

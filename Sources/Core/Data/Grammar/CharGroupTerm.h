/**
 * @file Core/Data/Grammar/CharGroupTerm.h
 * Contains the header of class Core::Data::Grammar::CharGroupTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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
class CharGroupTerm : public Term, public virtual DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(CharGroupTerm, Term, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DataHaving)
  ));


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
  }


  //============================================================================
  // Member Functions

  public: void setCharGroupReference(SharedPtr<Reference> const &ref)
  {
    this->charGroupReference = ref;
  }

  private: void setCharGroupReference(Reference *ref)
  {
    this->setCharGroupReference(getSharedPtr(ref));
  }

  public: SharedPtr<Reference> const& getCharGroupReference() const
  {
    return this->charGroupReference;
  }


  //============================================================================
  // DataHaving Implementation

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->charGroupReference != 0) Data::unsetIndexes(this->charGroupReference.get(), from, to);
  }

}; // class

} // namespace

#endif

/**
 * @file Core/Data/MultiplyTerm.h
 * Contains the header of class Core::Data::MultiplyTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_MULTIPLY_TERM_H
#define DATA_MULTIPLY_TERM_H

namespace Core { namespace Data
{

// TODO: Check the ownership of objects during the constructor. Will the objects be properly deleted in the destructor
//       in the case of an exception?

/**
 * @brief Handles multiply grammar terms.
 * @ingroup data_terms
 *
 * This class handles multiply terms in grammar formulas, which are the terms
 * used to implement optional and duplicate branches. The term has a minimum
 * and a maximum values for the number of occurnces it's child term (path) is
 * allowed to have. Passing a min of 0 and a max of 1 makes the term an optional
 * term.
 * Priority can be specified manually, but by default it's for taking the
 * branch rather than quiting the branch.
 */
class MultiplyTerm : public Term, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(MultiplyTerm, Term, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Term, DataOwner);


  //============================================================================
  // Member Variables

  /// @sa setTerm()
  private: SharedPtr<Term> term;

  /// @sa setMinOccurances()
  private: SharedPtr<IdentifiableObject> minOccurances;

  /// @sa setMaxOccurances()
  private: SharedPtr<IdentifiableObject> maxOccurances;

  /// @sa setPriority()
  private: SharedPtr<IdentifiableObject> priority;


  //============================================================================
  // Constructors

  /// Initialize the object.
  public: MultiplyTerm(SharedPtr<IdentifiableObject> const &p=SharedPtr<IdentifiableObject>(), Word f=0,
                       SharedPtr<IdentifiableObject> const &min=SharedPtr<IdentifiableObject>(),
                       SharedPtr<IdentifiableObject> const &max=SharedPtr<IdentifiableObject>(),
                       const SharedPtr<Term> &t=SharedPtr<Term>());

  public: MultiplyTerm(Int p, Word f, Int min, Int max, const SharedPtr<Term> &t) :
    priority(new Integer(p)),
    minOccurances(new Integer(min)),
    maxOccurances(new Integer(max)),
    term(t),
    Term(f)
  {
  }

  public: MultiplyTerm(const std::initializer_list<Argument<TermElement>> &args);

  public: virtual ~MultiplyTerm()
  {
  }

  public: static SharedPtr<MultiplyTerm> create(const std::initializer_list<Argument<TermElement>> &args)
  {
    return std::make_shared<MultiplyTerm>(args);
  }


  //============================================================================
  // Member Functions

  /// Set the head of the child branch.
  public: void setTerm(const SharedPtr<Term> &t)
  {
    this->term = t;
  }

  /// Get the head of the child branch.
  public: const SharedPtr<Term>& getTerm() const
  {
    return this->term;
  }

  /// Set the minimum number of occurances.
  public: void setMinOccurances(SharedPtr<IdentifiableObject> const &min);

  /// @sa setMinOccurances()
  public: SharedPtr<IdentifiableObject> const& getMinOccurances() const
  {
    return this->minOccurances;
  }

  /// Set the maximum number of occurances.
  public: void setMaxOccurances(SharedPtr<IdentifiableObject> const &max);

  /// @sa setMaxOccurances()
  public: SharedPtr<IdentifiableObject> const& getMaxOccurances() const
  {
    return this->maxOccurances;
  }

  /// Set the priority of the child branches.
  public: void setPriority(SharedPtr<IdentifiableObject> const &p);

  /// @sa setPriority()
  public: SharedPtr<IdentifiableObject> const& getPriority() const
  {
    return this->priority;
  }


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} } // namespace

#endif

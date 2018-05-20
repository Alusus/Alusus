/**
 * @file Core/Data/Grammar/MultiplyTerm.h
 * Contains the header of class Core::Data::Grammar::MultiplyTerm.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_MULTIPLYTERM_H
#define CORE_DATA_GRAMMAR_MULTIPLYTERM_H

namespace Core::Data::Grammar
{

// TODO: Check the ownership of objects during the constructor. Will the objects be properly deleted in the destructor
//       in the case of an exception?

/**
 * @brief Handles multiply grammar terms.
 * @ingroup core_data_grammar
 *
 * This class handles multiply terms in grammar formulas, which are the terms
 * used to implement optional and duplicate branches. The term has a minimum
 * and a maximum values for the number of occurnces it's child term (path) is
 * allowed to have. Passing a min of 0 and a max of 1 makes the term an optional
 * term.
 * Priority can be specified manually, but by default it's for taking the
 * branch rather than quiting the branch.
 */
class MultiplyTerm : public Term, public virtual DataOwner, public virtual Core::Basic::MapContainer<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(MultiplyTerm, Term, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(DataOwner, Core::Basic::MapContainer<TiObject>)
  ));


  //============================================================================
  // Member Variables

  private: SharedPtr<Term> term;

  private: TioSharedPtr min;

  private: TioSharedPtr max;

  private: TioSharedPtr priority;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDINGS(Term,
    (min, TiObject, SHARED_REF, setMin(value), min.get()),
    (max, TiObject, SHARED_REF, setMax(value), max.get()),
    (priority, TiObject, SHARED_REF, setPriority(value), priority.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContainer<TiObject>,
    (term, Term, setTerm(value), term.get())
  );


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(MultiplyTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(MultiplyTerm);

  public: virtual ~MultiplyTerm()
  {
    RESET_OWNED_SHAREDPTR(this->term);
  }


  //============================================================================
  // Member Functions

  /// Set the head of the child branch.
  public: void setTerm(const SharedPtr<Term> &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->term, t);
  }

  private: void setTerm(Term *t)
  {
    this->setTerm(getSharedPtr(t));
  }

  /// Get the head of the child branch.
  public: const SharedPtr<Term>& getTerm() const
  {
    return this->term;
  }

  /// Set the minimum number of occurances.
  public: void setMin(SharedPtr<TiObject> const &min);

  private: void setMin(TiObject *m)
  {
    this->setMin(getSharedPtr(m));
  }

  /// @sa setMin()
  public: SharedPtr<TiObject> const& getMin() const
  {
    return this->min;
  }

  /// Set the maximum number of occurances.
  public: void setMax(SharedPtr<TiObject> const &max);

  private: void setMax(TiObject *m)
  {
    this->setMax(getSharedPtr(m));
  }

  /// @sa setMax()
  public: SharedPtr<TiObject> const& getMax() const
  {
    return this->max;
  }

  /// Set the priority of the child branches.
  public: void setPriority(SharedPtr<TiObject> const &p);

  private: void setPriority(TiObject *p)
  {
    this->setPriority(getSharedPtr(p));
  }

  /// @sa setPriority()
  public: SharedPtr<TiObject> const& getPriority() const
  {
    return this->priority;
  }


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} // namespace

#endif

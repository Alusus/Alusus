/**
 * @file Core/Data/Grammar/MultiplyTerm.h
 * Contains the header of class Core::Data::Grammar::MultiplyTerm.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
class MultiplyTerm : public Term, public CacheHaving, public MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(MultiplyTerm, Term, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(CacheHaving, MapContaining<TiObject>)
  ));
  OBJECT_FACTORY(MultiplyTerm);


  //============================================================================
  // Types

  public: typedef std::unordered_map<Str, Int, std::hash<std::string>> TextBasedDecisionCache;
  public: typedef std::unordered_map<Word, Int> IdBasedDecisionCache;


  //============================================================================
  // Member Variables

  private: SharedPtr<Term> term;

  private: TioSharedPtr min;

  private: TioSharedPtr max;

  private: TextBasedDecisionCache innerTextBasedDecisionCache;

  private: IdBasedDecisionCache innerIdBasedDecisionCache;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Term,
    (min, TiObject, SHARED_REF, setMin(value), min.get()),
    (max, TiObject, SHARED_REF, setMax(value), max.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (term, Term, SHARED_REF, setTerm(value), term.get())
  );


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(MultiplyTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(MultiplyTerm);

  public: virtual ~MultiplyTerm()
  {
    RESET_OWNED_SHAREDPTR(this->term);
    RESET_OWNED_SHAREDPTR(this->min);
    RESET_OWNED_SHAREDPTR(this->max);
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

  public: TextBasedDecisionCache* getInnerTextBasedDecisionCache()
  {
    return &this->innerTextBasedDecisionCache;
  }

  public: IdBasedDecisionCache* getInnerIdBasedDecisionCache()
  {
    return &this->innerIdBasedDecisionCache;
  }


  //============================================================================
  // CacheHaving Implementation

  /// @sa CacheHaving::clearCache()
  public: virtual void clearCache();

}; // class

} // namespace

#endif

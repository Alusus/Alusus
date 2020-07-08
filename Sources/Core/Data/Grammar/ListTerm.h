/**
 * @file Core/Data/Grammar/ListTerm.h
 * Contains the header of class Core::Data::Grammar::ListTerm.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_LISTTERM_H
#define CORE_DATA_GRAMMAR_LISTTERM_H

namespace Core::Data::Grammar
{

/**
 * @brief The base of terms that consists of a list of terms.
 * @ingroup core_data_grammar
 *
 * Classes for terms that consist of list of terms can inherit from this class
 * to benefit from the list manipulation functions. This includes Alternate
 * terms and Concatenate terms. <br/>
 * This can be used for two types of term lists: <br/>
 * -Static list of terms. <br/>
 * -Dynamic list of terms. This is done by a list of data applied on a single
 *  term (which is the case in `alternate` and `concatenate` grammar commands).
 *
 * Static lists also support filtering of the terms in the list. Filtering can
 * either be done through a list of boolean flags, a flag for each term, or
 * through a single index number that specifies the single enabled term.
 */
class ListTerm : public Term, public MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(ListTerm, Term, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(MapContaining<TiObject>)
  ));


  //============================================================================
  // Member Variables

  /// @sa getTerms()
  protected: SharedPtr<List> terms;

  /// @sa getData()
  protected: SharedPtr<Node> filter;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (terms, List, SHARED_REF, setTerms(value), terms.get()),
    (filter, Node, SHARED_REF, setFilter(value), filter.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ListTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ListTerm);

  public: virtual ~ListTerm()
  {
    RESET_OWNED_SHAREDPTR(this->terms);
    RESET_OWNED_SHAREDPTR(this->filter);
  }


  //============================================================================
  // Member Functions

  public: void reset()
  {
    RESET_OWNED_SHAREDPTR(this->terms);
    RESET_OWNED_SHAREDPTR(this->filter);
  }

  public: void setTerms(const SharedPtr<List> &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->terms, t);
  }
  private: void setTerms(List *t)
  {
    this->setTerms(getSharedPtr(t));
  }

  /**
   * @brief Get the terms object.
   * This can either be of type Term or List depending on whether the term is
   * dynamic or static.
   */
  public: SharedPtr<List> const& getTerms() const
  {
    return this->terms;
  }

  /// Get a specific term from the list.
  public: Term* getTerm(Int index = 0) const;

  public: void setFilter(const SharedPtr<Node> &f)
  {
    UPDATE_OWNED_SHAREDPTR(this->filter, f);
  }
  private: void setFilter(Node *f)
  {
    this->setFilter(getSharedPtr(f));
  }

  public: SharedPtr<Node> const& getFilter() const
  {
    return this->filter;
  }

}; // class

} // namespace

#endif

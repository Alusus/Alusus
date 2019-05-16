/**
 * @file Core/Data/Grammar/ListTerm.h
 * Contains the header of class Core::Data::Grammar::ListTerm.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
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
class ListTerm : public Term, public DataHaving, public MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(ListTerm, Term, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(DataHaving, MapContaining<TiObject>)
  ));


  //============================================================================
  // Member Variables

  /// @sa getTerms()
  protected: SharedPtr<Node> terms;

  /// @sa getData()
  protected: SharedPtr<Node> data;

  /// @sa set_target_var()
  protected: SharedPtr<Reference> targetRef;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Term,
    (targetRef, Reference, SHARED_REF, setTargetRef(value), targetRef.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (terms, Node, SHARED_REF, setTerms(value), terms.get()),
    (data, Node, SHARED_REF, setData(value), data.get())
  );


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ListTerm);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ListTerm, this->validate());

  public: virtual ~ListTerm()
  {
    RESET_OWNED_SHAREDPTR(this->terms);
    RESET_OWNED_SHAREDPTR(this->data);
  }


  //============================================================================
  // Member Functions

  protected: void validate() const;

  /// Set a static list for this list term.
  public: void setStatic(SharedPtr<List> const &terms,
                         SharedPtr<Node> const &filter=SharedPtr<Node>());

  /// Set a dynamic list for this list term.
  public: void setDynamic(const SharedPtr<Term> &term, SharedPtr<Node> const &data,
                          const SharedPtr<Reference> &ref);

  /// Unset the list.
  public: void reset();

  /**
   * @brief Get whether a static list is currently set.
   * If the object is not initialized, the function returns false.
   */
  public: Bool isStatic() const
  {
    if (this->terms != 0 && this->terms->isA<List>()) return true;
    else return false;
  }

  /**
   * @brief Get whether a dynamic list is currently set.
   * If the object is not initialized, the function returns false.
   */
  public: Bool isDynamic() const
  {
    if (this->terms != 0 && this->terms->isDerivedFrom<Term>()) return true;
    else return false;
  }

  private: void setTerms(Node *t)
  {
    UPDATE_OWNED_SHAREDPTR(this->terms, getSharedPtr(t));
  }

  /**
   * @brief Get the terms object.
   * This can either be of type Term or List depending on whether the term is
   * dynamic or static.
   */
  public: SharedPtr<Node> const& getTerms() const
  {
    return this->terms;
  }

  /// Get a specific term from the list.
  public: SharedPtr<Term> getTerm(Int index = 0) const;

  private: void setData(Node *d)
  {
    UPDATE_OWNED_SHAREDPTR(this->data, getSharedPtr(d));
  }

  /**
   * @brief Get the data object.
   * This can either be the filter if the list is static or the data if the
   * term is dynamic.
   */
  public: SharedPtr<Node> const& getData() const
  {
    return this->data;
  }

  /// Set the target ref used for the dynamic list (data driven list terms).
  private: void setTargetRef(Reference *ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->targetRef, getSharedPtr(ref));
  }

  /**
   * @brief Get the reference to the target var used for the dynamic list.
   * This is the reference to the stack variable used as a loop variable by
   * data driven list terms.
   */
  public: const SharedPtr<Reference>& getTargetRef() const
  {
    return this->targetRef;
  }


  //============================================================================
  // DataHaving Implementation

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} // namespace

#endif

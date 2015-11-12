/**
 * @file Core/Data/ListTerm.h
 * Contains the header of class Core::Data::ListTerm.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_LISTTERM_H
#define DATA_LISTTERM_H

namespace Core { namespace Data
{

/**
 * @brief The base of terms that consists of a list of terms.
 * @ingroup data_terms
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
class ListTerm : public Term, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(ListTerm, Term, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Term, DataOwner);


  //============================================================================
  // Member Variables

  /// @sa getTerms()
  protected: SharedPtr<Node> terms;

  /// @sa getData()
  protected: SharedPtr<Node> data;

  /// @sa set_target_var()
  protected: SharedPtr<Reference> targetRef;


  //============================================================================
  // Constructor / Destructor

  /// Initialize an empty list term.
  protected: ListTerm() : Term(0)
  {
  }

  /// Initialize the object with either a static list or a dynamic list.
  protected: ListTerm(const std::initializer_list<Argument<TermElement>> &args);

  public: virtual ~ListTerm()
  {
    RESET_OWNED_SHAREDPTR(this->terms);
    RESET_OWNED_SHAREDPTR(this->data);
    RESET_OWNED_SHAREDPTR(this->targetRef);
  }


  //============================================================================
  // Member Functions

  /// Set a static list for this list term.
  public: void setStatic(SharedPtr<SharedList> const &terms,
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
    if (this->terms != 0 && this->terms->isA<SharedList>()) return true;
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

  /**
   * @brief Get the terms object.
   * This can either be of type Term or SharedList depending on whether the term is
   * dynamic or static.
   */
  public: SharedPtr<Node> const& getTerms() const
  {
    return this->terms;
  }

  /// Get a specific term from the list.
  public: SharedPtr<Term> getTerm(Int index = 0) const;

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
  public: void setTargetRef(const SharedPtr<Reference>& ref);

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
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} } // namespace

#endif

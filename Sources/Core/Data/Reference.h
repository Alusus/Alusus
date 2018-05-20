/**
 * @file Core/Data/Reference.h
 * Contains the header of class Core::Data::Reference.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_REFERENCE_H
#define CORE_DATA_REFERENCE_H

namespace Core { namespace Data
{

/**
 * @brief A reference to an object within a data tree.
 * This class is the base class of all reference classes and it represents a
 * single node within a reference chain. Instances of classes derived from this
 * class can be chained together to reach objects within other objects in a
 * hierarchical way.
 * @ingroup core_data
 */
class Reference : public Node, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(Reference, Node, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Node, DataOwner);


  //============================================================================
  // Member Variables

  /// @sa isSingleValued()
  private: Bool singleValued;

  /// @sa setNext()
  private: SharedPtr<Reference> next;

  /// @sa setResultValidator()
  private: SharedPtr<Validator> resultValidator;


  //============================================================================
  // Constructor

  protected: Reference(Bool sv=false) : singleValued(sv)
  {
  }

  public: virtual ~Reference()
  {
    RESET_OWNED_SHAREDPTR(this->next);
    RESET_OWNED_SHAREDPTR(this->resultValidator);
  }


  //============================================================================
  // Operators

  /// @sa compare()
  public: Bool operator==(Reference const &r) const
  {
    return this->compare(&r);
  }

  /// @sa compare()
  public: Bool operator==(SharedPtr<Reference> const &r) const
  {
    return this->compare(r.get());
  }


  //============================================================================
  // Member Functions

  /// @name Main Functions
  /// @{

  /**
   * @brief Set the next object in the reference chain.
   * The next object in the chain will be referencing an object within whatever
   * is referenced by this object.
   */
  public: void setNext(SharedPtr<Reference> const &n)
  {
    UPDATE_OWNED_SHAREDPTR(this->next, n);
  }

  /// @sa setNext()
  public: SharedPtr<Reference> const& getNext() const
  {
    return this->next;
  }

  /**
   * @brief Set a validator object to be used for result validation.
   * This validator will be used by seekers to validate the data. The reference
   * doesn't have to validate the data itself.
   */
  public: void setResultValidator(SharedPtr<Validator> const &v)
  {
    UPDATE_OWNED_SHAREDPTR(this->resultValidator, v);
  }

  /// @sa setResultValidator()
  public: SharedPtr<Validator> const& getResultValidator() const
  {
    return this->resultValidator;
  }

  /**
   * @brief Get the last Reference object in the chain.
   * This is useful for things like getting the immediate name
   * of a variable rather than the full path leading to it. For example, if
   * this chain represents "module1.map1.var1" then calling this function will
   * return the Reference object corresponding to "var1".
   */
  public: static Reference* getLastNode(Reference *reference);

  /**
   * @brief A SharedPtr version of getLastNode.
   * @sa getLastNode()
   */
  public: static SharedPtr<Reference> const& getLastNode(SharedPtr<Reference> const &reference);

  /**
   * @brief Compares this reference with another reference.
   * Implementation should only compare properties specific to the inherited
   * class and should call the parent implementation to compare the rest of
   * the chain.
   */
  public: virtual Bool compare(Reference const *r) const;

  /**
   * @brief Inform the object of the intended usage for this reference.
   * The intended use helps the object determine caching criteria.
   * @sa ReferenceUsageCriteria
   */
  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria) = 0;

  /**
   * @brief Returns whether this reference can only match a single value.
   * This function should return true if it this reference can match only a
   * single value within a source. This is useful for optimization purposes.
   */
  public: Bool isSingleValued() const
  {
    return this->singleValued;
  }

  /// @}

  /// @name Matching Functions
  /// @{

  /**
   * @brief Set a value for all matches on the given parent object.
   * @return True if any match is found.
   */
  public: virtual Bool setValue(Provider *provider, TiObject *parent,
                                TiObject *obj) const
  {
    Bool ret = false;
    this->setValue(provider, parent, [obj,&ret](Int index, TiObject *&o)->RefOp {
      o = obj;
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

  /**
   * @brief Remove all matches on the given parent object.
   * @return True if any match is found.
   */
  public: virtual Bool removeValue(Provider *provider, TiObject *parent) const
  {
    Bool ret = false;
    this->removeValue(provider, parent, [&ret](Int index, TiObject *o)->RefOp {
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

  /**
   * @brief Get the first value matched from the given parent object.
   * @return True if a match was found.
   */
  public: virtual Bool getValue(Provider *provider, TiObject *parent,
                                TiObject *&result) const
  {
    Bool ret = false;
    this->forEachValue(provider, parent, [&ret,&result](Int index, TiObject *o)->RefOp {
      result = o;
      ret = true;
      return RefOp::STOP;
    });
    return ret;
  }

  /**
   * @brief Set values on the given parent object.
   * @param handler A lambda function to be called for each match that should
   *                determine whether to set the value for that match and
   *                whether to move to the next match or stop.
   */
  public: virtual void setValue(Provider *provider, TiObject *parent,
                                ReferenceSetLambda handler) const = 0;

  /**
   * @brief Remove matched values from the given parent object.
   * @param handler A lambda function to be called for each match that should
   *                determine whether to remove the value for that match and
   *                whether to move to the next match or stop.
   */
  public: virtual void removeValue(Provider *provider, TiObject *parent,
                                   ReferenceRemoveLambda handler) const = 0;

  /**
   * @brief Loop on all matches in the given parent object.
   * @param handler A lambda function to be called for each match. The return
   *                vlue of the lambda determines whether to proceed in the
   *                matching or stop.
   */
  public: virtual void forEachValue(Provider *provider, TiObject *parent,
                                    ReferenceForeachLambda handler) const = 0;

  /// @}

  /// @name DataOwner Implementation
  /// @{

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->next != 0) this->next->unsetIndexes(from, to);
  }

  /// @}

}; // class

} } // namespace

#endif

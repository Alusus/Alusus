/**
 * @file Core/Data/Reference.h
 * Contains the header of class Core::Data::Reference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_REFERENCE_H
#define DATA_REFERENCE_H

namespace Core { namespace Data
{

/**
 * @brief A reference to an object within a data tree.
 * This class is the base class of all reference classes and it represents a
 * single node within a reference chain. Instances of classes derived from this
 * class can be chained together to reach objects within other objects in a
 * hierarchical way.
 * @ingroup data
 */
class Reference : public IdentifiableObject, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(Reference, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, DataOwner);


  //============================================================================
  // Member Variables

  /// @sa setNext()
  private: SharedPtr<Reference> next;

  /// @sa setResultValidator()
  private: SharedPtr<Validator> resultValidator;


  //============================================================================
  // Constructor

  protected: Reference()
  {
  }

  public: virtual ~Reference()
  {
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
    this->next = n;
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
    this->resultValidator = v;
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

  /// @}

  /// @name Abstract Functions
  /// @{

  /**
   * @brief Inform the object of the intended usage for this reference.
   * The intended use helps the object determine caching criteria.
   * @sa ReferenceUsageCriteria
   */
  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria) = 0;

  /**
   * @brief Set a value on the given parent object.
   * @param index A reference to a variable that will store an index value used
   *              for multi match searches. This will allow a next call to
   *              continue the search from where it left. If the value was set
   *              to -1 after the call it means no more matches are there.
   *              Single match references will always set it to -1.
   * @return True if the operation was successful.
   */
  public: virtual Bool setValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj, Int &index) const = 0;

  /**
   * @brief Remove the value from the given parent object.
   * @param index A reference to a variable that will store an index value used
   *              for multi match searches. This will allow a next call to
   *              continue the search from where it left. If the value was set
   *              to -1 after the call it means no more matches are there.
   *              Single match references will always set it to -1.
   * @return True if the operation was successful.
   */
  public: virtual Bool removeValue(Provider *provider, IdentifiableObject *parent, Int &index) const = 0;

  /**
   * @brief Get the value from the given parent object.
   * @param index A reference to a variable that will store an index value used
   *              for multi match searches. This will allow a next call to
   *              continue the search from where it left. If the value was set
   *              to -1 after the call it means no more matches are there.
   *              Single match references will always set it to -1.
   * @return True if the operation was successful.
   */
  public: virtual Bool getValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *&result, Int &index) const = 0;

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

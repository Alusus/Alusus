/**
 * @file Core/Data/SearchReference.h
 * Contains the header of class Core::Data::SearchReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SEARCHREFERENCE_H
#define DATA_SEARCHREFERENCE_H

namespace Core { namespace Data
{

// TODO: DOC

class SearchReference : public Reference
{
  //============================================================================
  // Type Info

  TYPE_INFO(SearchReference, Reference, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: SharedPtr<Validator> searchValidator;
  private: Bool forward;
  private: Int matchLimitationIndex;

  private: ReferenceUsageCriteria usageCriteria;

  private: mutable Int cachedIndex;


  //============================================================================
  // Constructor

  public: SearchReference() :
    forward(true), matchLimitationIndex(-1),
    usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
  }

  public: SearchReference(SharedPtr<Validator> validator, Bool f=true, Int matchLimitation=-1) :
    searchValidator(validator), forward(f), matchLimitationIndex(matchLimitation),
    usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
    if (matchLimitation < -1) {
      throw EXCEPTION(InvalidArgumentException, STR("matchLimitation"), STR("Must be >= -1"));
    }
  }

  public: virtual ~SearchReference()
  {
    RESET_OWNED_SHAREDPTR(this->searchValidator);
  }


  //============================================================================
  // Member Functions

  public: void setSearchValidator(SharedPtr<Validator> v)
  {
    UPDATE_OWNED_SHAREDPTR(this->searchValidator, v);
    this->cachedIndex = -1;
  }

  public: SharedPtr<Validator> const& getSearchValidator() const
  {
    return this->searchValidator;
  }

  public: void setSearchDirection(Bool forward)
  {
    this->forward = forward;
    this->cachedIndex = -1;
  }

  public: Bool getSearchDirection() const
  {
    return this->forward;
  }

  /**
   * @brief Set the index to which to limit the reference matches.
   * For references with multiple matches, this method can be used to limit the
   * matches to a single index value making it act like a single match reference
   * and forcing it to return a specific match. For example, setting this value
   * to 2 will make this reference always use the third match in the
   * set/get/remove methods. The set/get/remove methods will act like a single
   * match reference and always return -1 in the index parameter. Passing -1 to
   * this method as the index value will remove the match limitation.
   */
  public: void setMatchLimitation(Int index)
  {
    if (index < -1) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be >= -1"));
    }
    this->matchLimitationIndex = index;
    this->cachedIndex = -1;
  }

  /// @sa setMatchLimitation()
  public: Int getMatchLimitation() const
  {
    return this->matchLimitationIndex;
  }

  private: Bool isCacheUsable() const
  {
    if ((this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_SINGLE_MATCH) ||
        (this->usageCriteria == ReferenceUsageCriteria::SINGLE_DATA_MULTI_MATCH && this->matchLimitationIndex != -1)) {
      return true;
    } else {
      return false;
    }
  }

  public: virtual Bool compare(Reference const *r) const;

  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria)
  {
    this->usageCriteria = criteria;
    this->cachedIndex = -1;
  }

  public: virtual void setValue(Provider *provider, IdentifiableObject *parent,
                                ReferenceSetLambda handler) const;

  public: virtual void removeValue(Provider *provider, IdentifiableObject *parent,
                                   ReferenceRemoveLambda handler) const;

  public: virtual void forEachValue(Provider *provider, IdentifiableObject *parent,
                                    ReferenceForeachLambda handler) const;

  private: Int getListIndex(Container const *container, Int i) const
  {
    return this->forward==true ? i : container->getCount()-1-i;
  }

  private: Int findMatch(Container const *container, Int index, Int skips=0) const;


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->cachedIndex >= from && this->cachedIndex <= to) this->cachedIndex = -1;
    Reference::unsetIndexes(from, to);
  }

}; // class

} } // namespace

#endif

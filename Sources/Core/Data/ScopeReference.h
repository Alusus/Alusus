/**
 * @file Core/Data/ScopeReference.h
 * Contains the header of class Core::Data::ScopeReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SCOPEREFERENCE_H
#define CORE_DATA_SCOPEREFERENCE_H

namespace Core { namespace Data
{

// TODO: DOC

class ScopeReference : public Reference
{
  //============================================================================
  // Type Info

  TYPE_INFO(ScopeReference, Reference, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str scope;
  private: Bool upward;

  private: ReferenceUsageCriteria usageCriteria;
  private: mutable Int cachedIndex;


  //============================================================================
  // Constructor

  public: ScopeReference() : usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
  }

  public: ScopeReference(Char const *s, Bool up = true) : scope(s), upward(up),
    usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
  }

  public: ScopeReference(Char const *s, Word l, Bool up = true) : scope(s, l), upward(up),
    usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
  }

  public: virtual ~ScopeReference()
  {
  }


  //============================================================================
  // Member Functions

  public: void setScope(Char const *s)
  {
    this->scope = s;
    this->cachedIndex = -1;
  }

  public: void setScope(Char const *s, Int l)
  {
    this->scope.assign(s, l);
    this->cachedIndex = -1;
  }

  public: Str const& getScope() const
  {
    return this->scope;
  }

  public: void setSearchDirection(Bool upward)
  {
    this->upward = upward;
    this->cachedIndex = -1;
  }

  public: Bool getSearchDirection() const
  {
    return this->upward;
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

  private: Int getListIndex(NamedListContainer const *container, Int i) const
  {
    return this->upward==true ? i : container->getCount()-1-i;
  }

  private: template<class C> Int findScope(C const *container, Int index) const
  {
    ASSERT(container != 0);
    if (index >= container->getCount()) return -1;
    for (Int i = std::max(0, index); i < container->getCount(); ++i) {
      if (this->scope.empty() || this->scope == container->getName(this->getListIndex(container,i))) return i;
    }
    return -1;
  }


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

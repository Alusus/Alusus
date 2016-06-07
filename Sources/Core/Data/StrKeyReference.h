/**
 * @file Core/Data/StrKeyReference.h
 * Contains the header of class Core::Data::StrKeyReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_STRKEYREFERENCE_H
#define CORE_DATA_STRKEYREFERENCE_H

namespace Core { namespace Data
{

// TODO: DOC

class StrKeyReference : public Reference
{
  //============================================================================
  // Type Info

  TYPE_INFO(StrKeyReference, Reference, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str key;
  private: ReferenceUsageCriteria usageCriteria;
  private: mutable Int cachedIndex;


  //============================================================================
  // Constructor

  public: StrKeyReference()
    : Reference(true), usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
  }

  public: StrKeyReference(Char const *k)
    : Reference(true), key(k), usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
  }

  public: StrKeyReference(Char const *k, Int s)
    : Reference(true), key(k, s), usageCriteria(ReferenceUsageCriteria::MULTI_DATA), cachedIndex(-1)
  {
  }

  public: virtual ~StrKeyReference()
  {
  }


  //============================================================================
  // Member Functions

  public: void setKey(Char const *k)
  {
    this->key = k;
    this->cachedIndex = -1;
  }

  public: void setKey(Char const *k, Int s)
  {
    this->key.assign(k, s);
    this->cachedIndex = -1;
  }

  public: Str const& getKey() const
  {
    return this->key;
  }

  public: virtual Bool compare(Reference const *r) const;

  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria)
  {
    this->usageCriteria = criteria;
    this->cachedIndex = -1;
  }

  public: virtual void setValue(Provider *provider, TiObject *parent,
                                ReferenceSetLambda handler) const;

  public: virtual void removeValue(Provider *provider, TiObject *parent,
                                   ReferenceRemoveLambda handler) const;

  public: virtual void forEachValue(Provider *provider, TiObject *parent,
                                    ReferenceForeachLambda handler) const;

  public: virtual Bool setValue(Provider *provider, TiObject *parent,
                                TiObject *obj) const;

  public: virtual Bool getValue(Provider *provider, TiObject *parent,
                                TiObject *&result) const;

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->cachedIndex >= from && this->cachedIndex <= to) this->cachedIndex = -1;
    Reference::unsetIndexes(from, to);
  }

}; // class

} } // namespace

#endif

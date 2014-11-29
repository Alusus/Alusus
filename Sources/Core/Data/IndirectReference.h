/**
 * @file Core/Data/IndirectReference.h
 * Contains the header of class Core::Data::IndirectReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_INDIRECTREFERENCE_H
#define DATA_INDIRECTREFERENCE_H

namespace Core { namespace Data
{

// TODO: DOC

class IndirectReference : public Reference
{
  //============================================================================
  // Type Info

  TYPE_INFO(IndirectReference, Reference, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> key;


  //============================================================================
  // Constructor

  public: IndirectReference(SharedPtr<Reference> const &k = SharedPtr<Reference>()) : key(k)
  {
  }

  public: virtual ~IndirectReference()
  {
  }


  //============================================================================
  // Member Functions

  public: void setKey(SharedPtr<Reference> const &k)
  {
    this->key = k;
  }

  public: SharedPtr<Reference> const& getKey() const
  {
    return this->key;
  }

  public: virtual Bool compare(Reference const *r) const;

  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria)
  {
  }

  public: virtual Bool setValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj, Int &index) const;

  public: virtual Bool removeValue(Provider *provider, IdentifiableObject *parent, Int &index) const;

  public: virtual Bool getValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *&result, Int &index) const;

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} } // namespace

#endif

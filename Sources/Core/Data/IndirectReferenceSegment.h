/**
 * @file Core/Data/IndirectReferenceSegment.h
 * Contains the header of class Core::Data::IndirectReferenceSegment.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_INDIRECT_REFERENCE_SEGMENT_H
#define DATA_INDIRECT_REFERENCE_SEGMENT_H

namespace Core { namespace Data
{

// TODO: DOC

class IndirectReferenceSegment : public ReferenceSegment
{
  //============================================================================
  // Type Info

  TYPE_INFO(IndirectReferenceSegment, ReferenceSegment, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> key;


  //============================================================================
  // Constructor

  public: IndirectReferenceSegment(const SharedPtr<Reference> &k = SharedPtr<Reference>()) : key(k)
  {
  }

  public: virtual ~IndirectReferenceSegment()
  {
  }


  //============================================================================
  // Member Functions

  public: void setKey(const SharedPtr<Reference> &k)
  {
    this->key = k;
  }

  public: const SharedPtr<Reference>& getKey() const
  {
    return this->key;
  }

  /// Compares this reference with another reference.
  public: virtual Bool compare(const ReferenceSegment *r) const;

  public: virtual void setShared(const Provider *provider, IdentifiableObject *parent,
                                 const SharedPtr<IdentifiableObject> &obj) const;

  public: virtual Bool trySetShared(const Provider *provider, IdentifiableObject *parent,
                                    const SharedPtr<IdentifiableObject> &obj) const;

  public: virtual void setPlain(const Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj) const;

  public: virtual Bool trySetPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *obj) const;

  public: virtual void remove(const Provider *provider, IdentifiableObject *parent) const;

  public: virtual Bool tryRemove(const Provider *provider, IdentifiableObject *parent) const;

  public: virtual const SharedPtr<IdentifiableObject>& getShared(const Provider *provider,
                                                                 IdentifiableObject *parent) const;

  public: virtual Bool tryGetShared(const Provider *provider, IdentifiableObject *parent,
                                    SharedPtr<IdentifiableObject> &result) const;

  public: virtual IdentifiableObject* getPlain(const Provider *provider, IdentifiableObject *parent) const;

  public: virtual Bool tryGetPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *&result) const;

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} } // namespace

#endif

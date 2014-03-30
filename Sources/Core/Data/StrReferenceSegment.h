/**
 * @file Core/Data/StrReferenceSegment.h
 * Contains the header of class Data::StrReferenceSegment.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_STR_REFERENCE_SEGMENT_H
#define DATA_STR_REFERENCE_SEGMENT_H

namespace Core { namespace Data
{

// TODO: DOC

class StrReferenceSegment : public ReferenceSegment
{
  //============================================================================
  // Type Info

  TYPE_INFO(StrReferenceSegment, ReferenceSegment, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Str key;
  private: mutable Int index;


  //============================================================================
  // Constructor

  public: StrReferenceSegment() : index(-1)
  {
  }

  public: StrReferenceSegment(const Char *k) : key(k), index(-1)
  {
  }

  public: StrReferenceSegment(const Char *k, Int s) : key(k, s), index(-1)
  {
  }

  public: virtual ~StrReferenceSegment()
  {
  }


  //============================================================================
  // Member Functions

  public: void setKey(const Char *k)
  {
    this->key = k;
    this->index = -1;
  }

  public: void setKey(const Char *k, Int s)
  {
    this->key.assign(k, s);
    this->index = -1;
  }

  public: const Str& getKey() const
  {
    return this->key;
  }

  /// Compares this reference with another reference.
  public: virtual Bool compare(const ReferenceSegment *r) const;

  public: virtual void set(const Provider *provider, IdentifiableObject *parent,
                           const SharedPtr<IdentifiableObject> &obj) const;

  public: virtual Bool trySet(const Provider *provider, IdentifiableObject *parent,
                              const SharedPtr<IdentifiableObject> &obj) const;

  public: virtual void setPlain(const Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj) const;

  public: virtual Bool trySetPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *obj) const;

  public: virtual void remove(const Provider *provider, IdentifiableObject *parent) const;

  public: virtual Bool tryRemove(const Provider *provider, IdentifiableObject *parent) const;

  public: virtual const SharedPtr<IdentifiableObject>& get(const Provider *provider,
                                                           IdentifiableObject *parent) const;

  public: virtual Bool tryGet(const Provider *provider, IdentifiableObject *parent,
                              SharedPtr<IdentifiableObject> &result) const;

  public: virtual IdentifiableObject* getPlain(const Provider *provider, IdentifiableObject *parent) const;

  public: virtual Bool tryGetPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *&result) const;

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->index >= from && this->index <= to) this->index = -1;
    ReferenceSegment::unsetIndexes(from, to);
  }

}; // class

} } // namespace

#endif

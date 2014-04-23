/**
 * @file Core/Data/ReferenceSegment.h
 * Contains the header of class Core::Data::ReferenceSegment.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_REFERENCE_SEGMENT_H
#define DATA_REFERENCE_SEGMENT_H

namespace Core { namespace Data
{

// TODO:

class ReferenceSegment : public IdentifiableObject, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReferenceSegment, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, DataOwner);


  //============================================================================
  // Member Variables

  /// @sa getNext()
  private: SharedPtr<ReferenceSegment> next;


  //============================================================================
  // Constructor

  protected: ReferenceSegment()
  {
  }

  public: virtual ~ReferenceSegment()
  {
  }


  //============================================================================
  // Operators

  /// @sa compare()
  public: Bool operator==(const ReferenceSegment &r) const
  {
    return this->compare(&r) == 0;
  }

  public: Bool operator==(const SharedPtr<ReferenceSegment> &r) const
  {
    return this->compare(r.get()) == 0;
  }


  //============================================================================
  // Member Functions

  public: void setNext(const SharedPtr<ReferenceSegment> &n)
  {
    this->next = n;
  }

  public: const SharedPtr<ReferenceSegment>& getNext() const
  {
    return this->next;
  }

  /// Compares this reference with another reference.
  public: virtual Bool compare(const ReferenceSegment *r) const = 0;

  public: virtual void setShared(const Provider *provider, IdentifiableObject *parent,
                                 const SharedPtr<IdentifiableObject> &obj) const = 0;

  public: virtual Bool trySetShared(const Provider *provider, IdentifiableObject *parent,
                                    const SharedPtr<IdentifiableObject> &obj) const = 0;

  public: virtual void setPlain(const Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj) const = 0;

  public: virtual Bool trySetPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *obj) const = 0;

  public: virtual void remove(const Provider *provider, IdentifiableObject *parent) const = 0;

  public: virtual Bool tryRemove(const Provider *provider, IdentifiableObject *parent) const = 0;

  public: virtual const SharedPtr<IdentifiableObject>& getShared(const Provider *provider,
                                                                 IdentifiableObject *parent) const = 0;

  public: virtual Bool tryGetShared(const Provider *provider, IdentifiableObject *parent,
                              SharedPtr<IdentifiableObject> &result) const = 0;

  public: virtual IdentifiableObject* getPlain(const Provider *provider, IdentifiableObject *parent) const = 0;

  public: virtual Bool tryGetPlain(const Provider *provider, IdentifiableObject *parent,
                                   IdentifiableObject *&result) const = 0;


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to)
  {
    if (this->next != 0) this->next->unsetIndexes(from, to);
  }

}; // class

} } // namespace

#endif

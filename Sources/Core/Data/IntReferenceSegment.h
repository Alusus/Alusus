/**
 * @file Core/Data/IntReferenceSegment.h
 * Contains the header of class Data::IntReferenceSegment.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_INT_REFERENCE_SEGMENT_H
#define DATA_INT_REFERENCE_SEGMENT_H

namespace Core { namespace Data
{

// TODO: DOC

class IntReferenceSegment : public ReferenceSegment
{
  //============================================================================
  // Type Info

  TYPE_INFO(IntReferenceSegment, ReferenceSegment, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Int index;


  //============================================================================
  // Constructor

  public: IntReferenceSegment(Int i = -1) : index(i)
  {
  }

  public: virtual ~IntReferenceSegment()
  {
  }


  //============================================================================
  // Member Functions

  public: void setIndex(Int i)
  {
    this->index = i;
  }

  public: Int getIndex() const
  {
    return this->index;
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

}; // class

} } // namespace

#endif

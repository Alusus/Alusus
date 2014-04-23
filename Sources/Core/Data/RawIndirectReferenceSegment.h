/**
 * @file Core/Data/RawIndirectReferenceSegment.h
 * Contains the header of class Core::Data::RawIndirectReferenceSegment.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_RAW_INDIRECT_REFERENCE_SEGMENT_H
#define DATA_RAW_INDIRECT_REFERENCE_SEGMENT_H

namespace Core { namespace Data
{

// TODO: DOC

class RawIndirectReferenceSegment : public ReferenceSegment
{
  //============================================================================
  // Type Info

  TYPE_INFO(RawIndirectReferenceSegment, ReferenceSegment, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: const Char *qualifier;
  private: Int size;


  //============================================================================
  // Constructor

  public: RawIndirectReferenceSegment() : qualifier(0), size(0)
  {
  }

  public: RawIndirectReferenceSegment(const Char *q, Int c) : qualifier(q), size(c)
  {
  }

  public: virtual ~RawIndirectReferenceSegment()
  {
  }


  //============================================================================
  // Member Functions

  public: void setQualifier(const Char *q, Int s)
  {
    this->qualifier = q;
    this->size = s;
  }

  public: const Char* getQualifier() const
  {
    return this->qualifier;
  }

  public: Int getSize() const
  {
    return this->size;
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

}; // class

} } // namespace

#endif

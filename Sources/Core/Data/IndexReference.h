/**
 * @file Core/Data/IndexReference.h
 * Contains the header of class Core::Data::IndexReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_INDEXREFERENCE_H
#define DATA_INDEXREFERENCE_H

namespace Core { namespace Data
{

// TODO: DOC

class IndexReference : public Reference
{
  //============================================================================
  // Type Info

  TYPE_INFO(IndexReference, Reference, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Member Variables

  private: Int index;


  //============================================================================
  // Constructor

  public: IndexReference(Int i = -1) : index(i)
  {
  }

  public: virtual ~IndexReference()
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

  public: virtual Bool compare(Reference const *r) const;

  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria)
  {
  }

  public: virtual Bool setShared(Provider *provider, IdentifiableObject *parent,
                                 SharedPtr<IdentifiableObject> const &obj, Int &index) const;

  public: virtual Bool setPlain(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj, Int &index) const;

  public: virtual Bool remove(Provider *provider, IdentifiableObject *parent, Int &index) const;

  public: virtual Bool getShared(Provider *provider, IdentifiableObject *parent,
                                 SharedPtr<IdentifiableObject> &result, Int &index) const;

  public: virtual Bool getPlain(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *&result, Int &index) const;

}; // class

} } // namespace

#endif

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

  public: IndexReference(Int i = -1) : Reference(true), index(i)
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

  public: virtual void setValue(Provider *provider, IdentifiableObject *parent,
                                ReferenceSetLambda handler) const;

  public: virtual void removeValue(Provider *provider, IdentifiableObject *parent,
                                   ReferenceRemoveLambda handler) const;

  public: virtual void forEachValue(Provider *provider, IdentifiableObject *parent,
                                    ReferenceForeachLambda handler) const;

  public: virtual Bool setValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj) const;

  public: virtual Bool getValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *&result) const;

}; // class

} } // namespace

#endif

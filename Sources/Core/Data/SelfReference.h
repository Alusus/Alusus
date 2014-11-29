/**
 * @file Core/Data/SelfReference.h
 * Contains the header of class Core::Data::SelfReference.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SELFREFERENCE_H
#define DATA_SELFREFERENCE_H

namespace Core { namespace Data
{

// TODO: DOC

class SelfReference : public Reference
{
  //============================================================================
  // Type Info

  TYPE_INFO(SelfReference, Reference, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Constructor

  public: SelfReference()
  {
  }

  public: virtual ~SelfReference()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual Bool compare(Reference const *r) const
  {
    const SelfReference *ir = io_cast<const SelfReference>(r);
    if (ir != 0) return Reference::compare(r);
    else return false;
  }

  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria)
  {
  }

  public: virtual Bool setValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *obj, Int &index) const
  {
    return false;
  }

  public: virtual Bool removeValue(Provider *provider, IdentifiableObject *parent, Int &index) const
  {
    return false;
  }

  public: virtual Bool getValue(Provider *provider, IdentifiableObject *parent,
                                IdentifiableObject *&result, Int &index) const
  {
    if (index == -1) return false;
    result = parent;
    index = -1;
    return true;
  }

}; // class

} } // namespace

#endif

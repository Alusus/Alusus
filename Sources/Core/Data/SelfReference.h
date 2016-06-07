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

#ifndef CORE_DATA_SELFREFERENCE_H
#define CORE_DATA_SELFREFERENCE_H

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
    const SelfReference *ir = tio_cast<const SelfReference>(r);
    if (ir != 0) return Reference::compare(r);
    else return false;
  }

  public: virtual void setUsageCriteria(ReferenceUsageCriteria criteria)
  {
  }

  public: virtual void setValue(Provider *provider, TiObject *parent,
                                ReferenceSetLambda handler) const
  {
  }

  public: virtual void removeValue(Provider *provider, TiObject *parent,
                                   ReferenceRemoveLambda handler) const
  {
  }

  public: virtual void forEachValue(Provider *provider, TiObject *parent,
                                    ReferenceForeachLambda handler) const
  {
    handler(0, parent);
  }

}; // class

} } // namespace

#endif

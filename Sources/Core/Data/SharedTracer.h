/**
 * @file Core/Data/SharedTracer.h
 * Contains the header of interface Data::SharedTracer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SHAREDTRACER_H
#define DATA_SHAREDTRACER_H

namespace Core { namespace Data
{

// TODO: DOC

class SharedTracer : public Tracer
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(SharedTracer, Tracer, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual SharedPtr<IdentifiableObject> traceSharedValue(const SharedPtr<IdentifiableObject> &val) = 0;

  public: virtual void traceSharedValue(const SharedPtr<IdentifiableObject> &val, SharedModulePairedPtr &retVal) = 0;

}; // class

} } // namespace

#endif

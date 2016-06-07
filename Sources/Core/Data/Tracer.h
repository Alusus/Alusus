/**
 * @file Core/Data/Tracer.h
 * Contains the header of interface Data::Tracer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_TRACER_H
#define CORE_DATA_TRACER_H

namespace Core { namespace Data
{

// TODO: DOC

class Tracer : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Tracer, TiInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void traceValue(TiObject *val, TiObject *&retVal, Module *&retModule) = 0;

  public: virtual TiObject* traceValue(TiObject *val)
  {
    TiObject *retVal;
    Module *retModule;
    this->traceValue(val, retVal, retModule);
    return retVal;
  }

}; // class

} } // namespace

#endif

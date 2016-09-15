/**
* @file Spp/ExecutionContext.h
*
* @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SPP_EXECUTIONCONTEXT_H
#define SPP_EXECUTIONCONTEXT_H

namespace Spp
{

using namespace Core;

class ExecutionContext
{
  //============================================================================
  // Member Variables

  private: Int pointerBitCount;


  //============================================================================
  // Constructors & Destructor

  public: ExecutionContext(Int pointerBits) : pointerBitCount(pointerBits)
  {
  }


  //============================================================================
  // Member Functions

  public: Int getPointerBitCount() const
  {
    return this->pointerBitCount;
  }

}; // class

} // namespace

#endif

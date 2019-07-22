/**
 * @file Core/Data/CacheHaving.h
 * Contains the header of interface Data::CacheHaving.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CACHEHAVING_H
#define CORE_DATA_CACHEHAVING_H

namespace Core::Data
{

// TODO: DOC

class CacheHaving : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(CacheHaving, TiInterface, "Core.Data", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual void clearCache() = 0;

}; // class

} // namespace

#endif

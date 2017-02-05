/**
 * @file Core/Data/Container.h
 * Contains the header of interface Data::Container.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_CONTAINER_H
#define CORE_DATA_CONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class Container : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Container, TiInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void set(Int index, TiObject *val) = 0;

  public: virtual void remove(Int index) = 0;

  public: virtual Word getCount() const = 0;

  public: virtual TiObject* get(Int index) const = 0;

}; // class

} } // namespace

#endif

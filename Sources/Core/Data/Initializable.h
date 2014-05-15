/**
 * @file Core/Data/Initializable.h
 * Contains the header of interface Data::Initializable.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_INITIALIZABLE_H
#define DATA_INITIALIZABLE_H

namespace Core { namespace Data
{

// TODO: DOC

class Provider;
class Module;

class Initializable : public IdentifiableInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Initializable, IdentifiableInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void initialize(Provider *provider, const SharedPtr<Module> &module) = 0;

}; // class

} } // namespace

#endif

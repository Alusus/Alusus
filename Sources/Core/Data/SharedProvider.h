/**
 * @file Core/Data/SharedProvider.h
 * Contains the header of interface Data::SharedProvider.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SHAREDPROVIDER_H
#define DATA_SHAREDPROVIDER_H

namespace Core { namespace Data
{

// TODO: DOC

class Reference;

class SharedProvider : public Provider
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(SharedProvider, Provider, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  /// @name Data Setting Functions
  /// @{

  public: virtual void setSharedValue(Reference *ref, const SharedPtr<IdentifiableObject> &obj) = 0;

  public: virtual void setSharedValue(const Char *qualifier, const SharedPtr<IdentifiableObject> &obj) = 0;

  /// @}

  /// @name Exception Based Shared Data Retrieval Functions
  /// @{

  public: virtual const SharedPtr<IdentifiableObject>& getSharedValue(Reference *ref) const = 0;

  public: virtual void getSharedValue(Reference *ref, SharedPtr<IdentifiableObject> &retVal,
                                      SharedPtr<Module> &retModule) const = 0;

  public: virtual const SharedPtr<IdentifiableObject>& getSharedValue(const Char *qualifier) const = 0;

  public: virtual void getSharedValue(const Char *qualifier, SharedPtr<IdentifiableObject> &retVal,
                                      SharedPtr<Module> &retModule) const = 0;

  /// @}

  /// @name Return-Value Based Shared Data Retrieval Functions
  /// @{

  public: virtual Bool tryGetSharedValue(Reference *ref, SharedPtr<IdentifiableObject> &retVal) const = 0;

  public: virtual Bool tryGetSharedValue(Reference *ref, SharedPtr<IdentifiableObject> &retVal,
                                         SharedPtr<Module> &retModule) const = 0;

  public: virtual Bool tryGetSharedValue(const Char *qualifier, SharedPtr<IdentifiableObject> &retVal) const = 0;

  public: virtual Bool tryGetSharedValue(const Char *qualifier, SharedPtr<IdentifiableObject> &retVal,
                                         SharedPtr<Module> &retModule) const = 0;

  /// @}

}; // class

} } // namespace

#endif

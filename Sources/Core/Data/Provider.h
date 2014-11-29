/**
 * @file Core/Data/Provider.h
 * Contains the header of interface Data::Provider.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

namespace Core { namespace Data
{

// TODO: DOC

class Reference;
class Module;

class Provider : public IdentifiableInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Provider, IdentifiableInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  /// @name Data Setting Functions
  /// @{

  public: virtual void set(Reference const *ref, IdentifiableObject *val) = 0;

  public: virtual void set(Char const *qualifier, IdentifiableObject *val) = 0;

  public: virtual Bool trySet(Reference const *ref, IdentifiableObject *val) = 0;

  public: virtual Bool trySet(Char const *qualifier, IdentifiableObject *val) = 0;

  /// @}

  /// @name Data Removal Functions
  /// @{

  public: virtual void remove(Reference const *ref) = 0;

  public: virtual void remove(Char const *qualifier) = 0;

  public: virtual Bool tryRemove(Reference const *ref) = 0;

  public: virtual Bool tryRemove(Char const *qualifier) = 0;

  /// @}

  /// @name Exception Based Data Retrieval Functions
  /// @{

  public: virtual IdentifiableObject* get(Reference const *ref) = 0;

  public: virtual void get(Reference const *ref, PlainModulePairedPtr &retVal) = 0;

  public: virtual IdentifiableObject* get(Char const *qualifier) = 0;

  public: virtual void get(Char const *qualifier, PlainModulePairedPtr &retVal) = 0;

  /// @}

  /// @name Return-Value Based Data Retrieval Functions
  /// @{

  public: virtual Bool tryGet(Reference const *ref, IdentifiableObject *&retVal) = 0;

  public: virtual Bool tryGet(Reference const *ref, PlainModulePairedPtr &retVal) = 0;

  public: virtual Bool tryGet(Char const *qualifier, IdentifiableObject *&retVal) = 0;

  public: virtual Bool tryGet(Char const *qualifier, PlainModulePairedPtr &retVal) = 0;

  /// @}

}; // class

} } // namespace

#endif

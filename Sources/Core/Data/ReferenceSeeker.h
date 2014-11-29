/**
 * @file Core/Data/ReferenceSeeker.h
 * Contains the header of class Core::Data::ReferenceSeeker.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_REFERENCESEEKER_H
#define DATA_REFERENCESEEKER_H

namespace Core { namespace Data
{

// TODO: DOC

class ReferenceSeeker
{
  //============================================================================
  // Member Variables

  private: Provider *dataProvider;


  //============================================================================
  // Constructor & Destructor

  public: ReferenceSeeker(Provider *prov=0) : dataProvider(prov)
  {
  }

  public: ~ReferenceSeeker()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setDataProvider(Provider *prov)
  {
    this->dataProvider = prov;
  }

  public: Provider* getDataProvider() const
  {
    return this->dataProvider;
  }

  /// @}

  /// @name Data Read Functions
  /// @{

  public: IdentifiableObject* get(Reference const *seg, IdentifiableObject *parent) const;

  public: IdentifiableObject* tryGet(Reference const *seg, IdentifiableObject *parent) const;

  public: Bool tryGet(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *&result) const;

  public: void get(Reference const *seg, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const;

  public: Bool tryGet(Reference const *seg, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const;

  /// @}

  /// @name Data Write Functions
  /// @{

  public: void set(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const;

  public: Bool trySet(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(Reference const *seg, IdentifiableObject *parent) const;

  public: Bool tryRemove(Reference const *seg, IdentifiableObject *parent) const;

  /// @}

}; // class

} } // namespace

#endif

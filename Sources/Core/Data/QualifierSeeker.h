/**
 * @file Core/Data/QualifierSeeker.h
 * Contains the header of class Core::Data::QualifierSeeker.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_QUALIFIERSEEKER_H
#define DATA_QUALIFIERSEEKER_H

namespace Core { namespace Data
{

// TODO: DOC

class QualifierSeeker
{
  //============================================================================
  // Member Variables

  private: Provider *dataProvider;
  private: mutable ReferenceParser parser;


  //============================================================================
  // Constructor & Destructor

  public: QualifierSeeker(Provider *prov=0) : dataProvider(prov)
  {
  }

  public: ~QualifierSeeker()
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

  public: IdentifiableObject* get(Char const *qualifier, IdentifiableObject *parent) const;

  public: IdentifiableObject* tryGet(Char const *qualifier, IdentifiableObject *parent) const;

  public: Bool tryGet(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *&result) const;

  public: void get(Char const *qualifier, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const;

  public: Bool tryGet(Char const *qualifier, IdentifiableObject *parent, PlainModulePairedPtr &retVal) const;

  /// @}

  /// @name Data Write Functions
  /// @{

  public: void set(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const;

  public: Bool trySet(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(Char const *qualifier, IdentifiableObject *parent) const;

  public: Bool tryRemove(Char const *qualifier, IdentifiableObject *parent) const;

  /// @}

}; // class

} } // namespace

#endif

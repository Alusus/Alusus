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

  private: Provider const *dataProvider;
  private: mutable ReferenceParser parser;


  //============================================================================
  // Constructor & Destructor

  public: QualifierSeeker(Provider const *prov=0) : dataProvider(prov)
  {
  }

  public: ~QualifierSeeker()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setDataProvider(Provider const *prov)
  {
    this->dataProvider = prov;
  }

  public: Provider const* getDataProvider() const
  {
    return this->dataProvider;
  }

  /// @}

  /// @name Data Read Functions
  /// @{

  public: SharedPtr<IdentifiableObject> getShared(Char const *qualifier, IdentifiableObject const *parent) const;

  public: Bool tryGetShared(Char const *qualifier, IdentifiableObject const *parent,
                            SharedPtr<IdentifiableObject> &result) const;

  public: void getShared(Char const *qualifier, IdentifiableObject const *parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Char const *qualifier, IdentifiableObject const *parent,
                            SharedModulePairedPtr &retVal) const;

  public: void getShared(Char const *qualifier, SharedPtr<IdentifiableObject> const &parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Char const *qualifier, SharedPtr<IdentifiableObject> const &parent,
                            SharedModulePairedPtr &retVal) const;

  /// @}

  /// @name Plain Data Read Functions
  /// @{

  public: IdentifiableObject* getPlain(Char const *qualifier, IdentifiableObject const *parent) const;

  public: Bool tryGetPlain(Char const *qualifier, IdentifiableObject const *parent, IdentifiableObject *&result) const;

  public: void getPlain(Char const *qualifier, IdentifiableObject *parent,
                        PlainModulePairedPtr &retVal) const;

  public: Bool tryGetPlain(Char const *qualifier, IdentifiableObject *parent,
                           PlainModulePairedPtr &retVal) const;

  /// @}

  /// @name Data Write Functions
  /// @{

  public: void setShared(Char const *qualifier, IdentifiableObject *parent,
                         SharedPtr<IdentifiableObject> const &val) const;

  public: Bool trySetShared(Char const *qualifier, IdentifiableObject *parent,
                            SharedPtr<IdentifiableObject> const &val) const;

  public: void setPlain(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const;

  public: Bool trySetPlain(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(Char const *qualifier, IdentifiableObject *parent) const;

  public: Bool tryRemove(Char const *qualifier, IdentifiableObject *parent) const;

  /// @}

}; // class

} } // namespace

#endif

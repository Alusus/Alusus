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

  private: Provider const *dataProvider;


  //============================================================================
  // Constructor & Destructor

  public: ReferenceSeeker(Provider const *prov=0) : dataProvider(prov)
  {
  }

  public: ~ReferenceSeeker()
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

  /// @name Shared Data Read Functions
  /// @{

  public: SharedPtr<IdentifiableObject> getShared(Reference const *seg, IdentifiableObject const *parent) const;

  public: Bool tryGetShared(Reference const *seg, IdentifiableObject const *parent,
                            SharedPtr<IdentifiableObject> &result) const;

  public: void getShared(Reference const *seg, IdentifiableObject const *parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Reference const *seg, IdentifiableObject const *parent,
                            SharedModulePairedPtr &retVal) const;

  public: void getShared(Reference const *seg, SharedPtr<IdentifiableObject> const &parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Reference const *seg, SharedPtr<IdentifiableObject> const &parent,
                            SharedModulePairedPtr &retVal) const;

  /// @}

  /// @name Plain Data Read Functions
  /// @{

  public: IdentifiableObject* getPlain(Reference const *seg, IdentifiableObject const *parent) const;

  public: Bool tryGetPlain(Reference const *seg, IdentifiableObject const *parent, IdentifiableObject *&result) const;

  public: void getPlain(Reference const *seg, IdentifiableObject *parent,
                        PlainModulePairedPtr &retVal) const;

  public: Bool tryGetPlain(Reference const *seg, IdentifiableObject *parent,
                           PlainModulePairedPtr &retVal) const;

  /// @}

  /// @name Data Write Functions
  /// @{

  public: void setShared(Reference const *seg, IdentifiableObject *parent,
                         SharedPtr<IdentifiableObject> const &val) const;

  public: Bool trySetShared(Reference const *seg, IdentifiableObject *parent,
                            SharedPtr<IdentifiableObject> const &val) const;

  public: void setPlain(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const;

  public: Bool trySetPlain(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *val) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(Reference const *seg, IdentifiableObject *parent) const;

  public: Bool tryRemove(Reference const *seg, IdentifiableObject *parent) const;

  /// @}

}; // class

} } // namespace

#endif

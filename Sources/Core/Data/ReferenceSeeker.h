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

  /// @name Shared Data Read Functions
  /// @{

  public: SharedPtr<IdentifiableObject> getShared(Reference const *seg, IdentifiableObject *parent) const;

  public: template<class T> SharedPtr<T> getShared(Reference const *seg, IdentifiableObject *parent) const
  {
    return this->getShared(seg, parent).io_cast<T>();
  }

  public: SharedPtr<IdentifiableObject> tryGetShared(Reference const *seg, IdentifiableObject *parent) const;

  public: template<class T> SharedPtr<T> tryGetShared(Reference const *seg, IdentifiableObject *parent) const
  {
    return this->tryGetShared(seg, parent).io_cast<T>();
  }

  public: Bool tryGetShared(Reference const *seg, IdentifiableObject *parent,
                            SharedPtr<IdentifiableObject> &result) const;

  public: template<class T> Bool tryGetShared(Reference const *seg, IdentifiableObject *parent,
                                              SharedPtr<T> &result) const
  {
    SharedPtr<IdentifiableObject> res;
    if (!this->tryGetShared(seg, parent, res)) return false;
    result = res.io_cast<T>();
    return true;
  }

  public: void getShared(Reference const *seg, IdentifiableObject *parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Reference const *seg, IdentifiableObject *parent,
                            SharedModulePairedPtr &retVal) const;

  public: void getShared(Reference const *seg, SharedPtr<IdentifiableObject> const &parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Reference const *seg, SharedPtr<IdentifiableObject> const &parent,
                            SharedModulePairedPtr &retVal) const;

  /// @}

  /// @name Plain Data Read Functions
  /// @{

  public: IdentifiableObject* getPlain(Reference const *seg, IdentifiableObject *parent) const;

  public: template<class T> T* getPlain(Reference const *seg, IdentifiableObject *parent) const
  {
    return io_cast<T>(this->getPlain(seg, parent));
  }

  public: IdentifiableObject* tryGetPlain(Reference const *seg, IdentifiableObject *parent) const;

  public: template<class T> T* tryGetPlain(Reference const *seg, IdentifiableObject *parent) const
  {
    return io_cast<T>(this->tryGetPlain(seg, parent));
  }

  public: Bool tryGetPlain(Reference const *seg, IdentifiableObject *parent, IdentifiableObject *&result) const;

  public: template<class T> Bool tryGetPlain(Reference const *seg, IdentifiableObject *parent, T *&result) const
  {
    IdentifiableObject *res = 0;
    if (!this->tryGetPlain(seg, parent, res)) return false;
    result = io_cast<T>(res);
    return true;
  }

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

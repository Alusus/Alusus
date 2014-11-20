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

  public: SharedPtr<IdentifiableObject> getShared(Char const *qualifier, IdentifiableObject *parent) const;

  public: template<class T> SharedPtr<T> getShared(Char const *qualifier, IdentifiableObject *parent) const
  {
    return this->getShared(qualifier, parent).io_cast<T>();
  }

  public: SharedPtr<IdentifiableObject> tryGetShared(Char const *qualifier, IdentifiableObject *parent) const;

  public: template<class T> SharedPtr<T> tryGetShared(Char const *qualifier, IdentifiableObject *parent) const
  {
    return this->tryGetShared(qualifier, parent).io_cast<T>();
  }

  public: Bool tryGetShared(Char const *qualifier, IdentifiableObject *parent,
                            SharedPtr<IdentifiableObject> &result) const;

  public: template<class T> Bool tryGetShared(Char const *qualifier, IdentifiableObject *parent,
                                              SharedPtr<T> &result) const
  {
    SharedPtr<IdentifiableObject> res;
    if (!this->tryGetShared(qualifier, parent, res)) return false;
    result = res.io_cast<T>();
    return true;
  }

  public: void getShared(Char const *qualifier, IdentifiableObject *parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Char const *qualifier, IdentifiableObject *parent,
                            SharedModulePairedPtr &retVal) const;

  public: void getShared(Char const *qualifier, SharedPtr<IdentifiableObject> const &parent,
                         SharedModulePairedPtr &retVal) const;

  public: Bool tryGetShared(Char const *qualifier, SharedPtr<IdentifiableObject> const &parent,
                            SharedModulePairedPtr &retVal) const;

  /// @}

  /// @name Plain Data Read Functions
  /// @{

  public: IdentifiableObject* getPlain(Char const *qualifier, IdentifiableObject *parent) const;

  public: template<class T> T* getPlain(Char const *qualifier, IdentifiableObject *parent) const
  {
    return io_cast<T>(this->getPlain(qualifier, parent));
  }

  public: IdentifiableObject* tryGetPlain(Char const *qualifier, IdentifiableObject *parent) const;

  public: template<class T> T* tryGetPlain(Char const *qualifier, IdentifiableObject *parent) const
  {
    return io_cast<T>(this->tryGetPlain(qualifier, parent));
  }

  public: Bool tryGetPlain(Char const *qualifier, IdentifiableObject *parent, IdentifiableObject *&result) const;

  public: template<class T> Bool tryGetPlain(Char const *qualifier, IdentifiableObject *parent, T *&result) const
  {
    IdentifiableObject *res = 0;
    if (!this->tryGetPlain(qualifier, parent, res)) return false;
    result = io_cast<T>(res);
    return true;
  }

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

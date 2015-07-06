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

  public: IdentifiableObject* get(Reference const *ref, IdentifiableObject *source) const;

  public: IdentifiableObject* tryGet(Reference const *ref, IdentifiableObject *source) const;

  public: void get(Reference const *ref, IdentifiableObject *source, IdentifiableObject *&retVal,
                   TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) const;

  public: Bool tryGet(Reference const *ref, IdentifiableObject *source, IdentifiableObject *&retVal,
                      TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) const;

  public: template<class T> Bool tryGet(Reference const *ref, IdentifiableObject *source,
                                        IdentifiableObject *&retVal, T *&retParent) const
  {
    IdentifiableObject *retIoParent;
    Bool result = this->tryGet(ref, source, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
    return result;
  }

  /// @}

  /// @name Data Write Functions
  /// @{

  public: void set(Reference const *ref, IdentifiableObject *target, IdentifiableObject *val) const;

  public: Bool trySet(Reference const *ref, IdentifiableObject *target, IdentifiableObject *val) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(Reference const *ref, IdentifiableObject *target) const;

  public: Bool tryRemove(Reference const *ref, IdentifiableObject *target) const;

  /// @}

}; // class

} } // namespace

#endif

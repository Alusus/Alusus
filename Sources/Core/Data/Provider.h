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

  public: virtual Bool trySet(Reference const *ref, IdentifiableObject *val) = 0;

  public: virtual Bool trySet(Char const *qualifier, IdentifiableObject *val) = 0;

  public: virtual void set(Reference const *ref, IdentifiableObject *val)
  {
    if (!this->trySet(ref, val)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: virtual void set(Char const *qualifier, IdentifiableObject *val)
  {
    if (!this->trySet(qualifier, val)) {
      throw EXCEPTION(GenericException, STR("Qualifier pointing to a missing element/tree."));
    }
  }

  /// @}

  /// @name Data Removal Functions
  /// @{

  public: virtual Bool tryRemove(Reference const *ref) = 0;

  public: virtual Bool tryRemove(Char const *qualifier) = 0;

  public: virtual void remove(Reference const *ref)
  {
    if (!this->tryRemove(ref)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: virtual void remove(Char const *qualifier)
  {
    if (!this->tryRemove(qualifier)) {
      throw EXCEPTION(GenericException, STR("Qualifier pointing to a missing element/tree."));
    }
  }

  /// @}

  /// @name Data Retrieval Functions
  /// @{

  public: virtual Bool tryGet(Reference const *ref, IdentifiableObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) = 0;

  public: virtual Bool tryGet(Char const *qualifier, IdentifiableObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) = 0;

  public: virtual IdentifiableObject* tryGet(Reference const *ref)
  {
    IdentifiableObject *result = 0;
    this->tryGet(ref, result);
    return result;
  }

  public: virtual IdentifiableObject* tryGet(Char const *qualifier)
  {
    IdentifiableObject *result = 0;
    this->tryGet(qualifier, result);
    return result;
  }

  public: template<class T> Bool tryGet(Reference const *ref, IdentifiableObject *&retVal, T *&retParent)
  {
    IdentifiableObject *retIoParent;
    Bool result = this->tryGet(ref, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
    return result;
  }

  public: template<class T> Bool tryGet(Char const *qualifier, IdentifiableObject *&retVal, T *&retParent)
  {
    IdentifiableObject *retIoParent;
    Bool result = this->tryGet(qualifier, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
    return result;
  }

  public: virtual void get(Reference const *ref, IdentifiableObject *&retVal,
                           TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0)
  {
    if (!this->tryGet(ref, retVal, parentTypeInfo, retParent)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: virtual void get(Char const *qualifier, IdentifiableObject *&retVal,
                           TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0)
  {
    if (!this->tryGet(qualifier, retVal, parentTypeInfo, retParent)) {
      throw EXCEPTION(GenericException, STR("Qualifier pointing to a missing element/tree."));
    }
  }

  public: virtual IdentifiableObject* get(Reference const *ref)
  {
    IdentifiableObject *retVal = 0;
    this->get(ref, retVal);
    return retVal;
  }

  public: virtual IdentifiableObject* get(Char const *qualifier)
  {
    IdentifiableObject *retVal = 0;
    this->get(qualifier, retVal);
    return retVal;
  }

  public: template<class T> void get(Reference const *ref, IdentifiableObject *&retVal, T *&retParent)
  {
    IdentifiableObject *retIoParent;
    this->get(ref, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
  }

  public: template<class T> void get(Char const *qualifier, IdentifiableObject *&retVal, T *&retParent)
  {
    IdentifiableObject *retIoParent;
    this->get(qualifier, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
  }

  /// @}

}; // class

} } // namespace

#endif

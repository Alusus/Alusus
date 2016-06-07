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

#ifndef CORE_DATA_PROVIDER_H
#define CORE_DATA_PROVIDER_H

namespace Core { namespace Data
{

// TODO: DOC

class Reference;
class Module;

class Provider : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Provider, TiInterface, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  /// @name Data Setting Functions
  /// @{

  public: virtual Bool trySet(Reference const *ref, TiObject *val)
  {
    Bool result = false;
    this->set(ref, [=,&result](Int index, TiObject *&obj)->RefOp {
      obj = val;
      result = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return result;
  }

  public: virtual Bool trySet(Char const *qualifier, TiObject *val)
  {
    Bool result = false;
    this->set(qualifier, [=,&result](Int index, TiObject *&obj)->RefOp {
      obj = val;
      result = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return result;
  }

  public: virtual void set(Reference const *ref, TiObject *val)
  {
    if (!this->trySet(ref, val)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: virtual void set(Char const *qualifier, TiObject *val)
  {
    if (!this->trySet(qualifier, val)) {
      throw EXCEPTION(GenericException, STR("Qualifier pointing to a missing element/tree."));
    }
  }

  public: virtual void set(Reference const *ref, SeekerSetLambda handler) = 0;

  public: virtual void set(Char const *qualifier, SeekerSetLambda handler) = 0;

  /// @}

  /// @name Data Removal Functions
  /// @{

  public: virtual Bool tryRemove(Reference const *ref)
  {
    Bool ret = false;
    this->remove(ref, [&ret](Int index, TiObject *o)->RefOp {
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

  public: virtual Bool tryRemove(Char const *qualifier)
  {
    Bool ret = false;
    this->remove(qualifier, [&ret](Int index, TiObject *o)->RefOp {
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

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

  public: virtual void remove(Reference const *ref, SeekerRemoveLambda handler) = 0;

  public: virtual void remove(Char const *qualifier, SeekerRemoveLambda handler) = 0;

  /// @}

  /// @name Data Retrieval Functions
  /// @{

  public: virtual Bool tryGet(Reference const *ref, TiObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, TiObject **retParent=0)
  {
    Bool ret = false;
    this->forEach(ref, [&ret,&retVal,&retParent](Int index, TiObject *o, TiObject *p)->RefOp {
      retVal = o;
      if (retParent != 0) *retParent = p;
      ret = true;
      return RefOp::STOP;
    }, parentTypeInfo);
    return ret;
  }

  public: virtual Bool tryGet(Char const *qualifier, TiObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, TiObject **retParent=0)
  {
    Bool ret = false;
    this->forEach(qualifier, [&ret,&retVal,&retParent](Int index, TiObject *o, TiObject *p)->RefOp {
      retVal = o;
      if (retParent != 0) *retParent = p;
      ret = true;
      return RefOp::STOP;
    }, parentTypeInfo);
    return ret;
  }

  public: virtual TiObject* tryGet(Reference const *ref)
  {
    TiObject *result = 0;
    this->tryGet(ref, result);
    return result;
  }

  public: virtual TiObject* tryGet(Char const *qualifier)
  {
    TiObject *result = 0;
    this->tryGet(qualifier, result);
    return result;
  }

  public: template<class T> Bool tryGet(Reference const *ref, TiObject *&retVal, T *&retParent)
  {
    TiObject *retIoParent;
    Bool result = this->tryGet(ref, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
    return result;
  }

  public: template<class T> Bool tryGet(Char const *qualifier, TiObject *&retVal, T *&retParent)
  {
    TiObject *retIoParent;
    Bool result = this->tryGet(qualifier, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
    return result;
  }

  public: virtual void get(Reference const *ref, TiObject *&retVal,
                           TypeInfo const *parentTypeInfo=0, TiObject **retParent=0)
  {
    if (!this->tryGet(ref, retVal, parentTypeInfo, retParent)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: virtual void get(Char const *qualifier, TiObject *&retVal,
                           TypeInfo const *parentTypeInfo=0, TiObject **retParent=0)
  {
    if (!this->tryGet(qualifier, retVal, parentTypeInfo, retParent)) {
      throw EXCEPTION(GenericException, STR("Qualifier pointing to a missing element/tree."));
    }
  }

  public: virtual TiObject* get(Reference const *ref)
  {
    TiObject *retVal = 0;
    this->get(ref, retVal);
    return retVal;
  }

  public: virtual TiObject* get(Char const *qualifier)
  {
    TiObject *retVal = 0;
    this->get(qualifier, retVal);
    return retVal;
  }

  public: template<class T> void get(Reference const *ref, TiObject *&retVal, T *&retParent)
  {
    TiObject *retIoParent;
    this->get(ref, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
  }

  public: template<class T> void get(Char const *qualifier, TiObject *&retVal, T *&retParent)
  {
    TiObject *retIoParent;
    this->get(qualifier, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
  }

  public: virtual void forEach(Reference const *ref, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo=0) = 0;

  public: virtual void forEach(Char const *qualifier, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo=0) = 0;

  /// @}

}; // class

} } // namespace

#endif

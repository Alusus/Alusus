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

  /// @name Data Write Functions
  /// @{

  public: void set(Reference const *ref, IdentifiableObject *target, IdentifiableObject *val) const
  {
    if (!this->trySet(ref, target, val)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool trySet(Reference const *ref, IdentifiableObject *target, IdentifiableObject *val) const
  {
    Bool ret = false;
    this->set(ref, target, [=,&ret](Int index, IdentifiableObject *&o)->RefOp {
      o = val;
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

  public: RefOp set(Reference const *ref, IdentifiableObject *target, SeekerSetLambda handler) const
  {
    return this->set(ref, target, handler, 0);
  }

  private: RefOp set(Reference const *ref, IdentifiableObject *target, SeekerSetLambda handler,
                     Int *index) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(Reference const *ref, IdentifiableObject *target) const
  {
    if (!this->tryRemove(ref, target)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryRemove(Reference const *ref, IdentifiableObject *target) const
  {
    Bool ret = false;
    this->remove(ref, target, [&ret](Int index, IdentifiableObject *o)->RefOp {
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

  public: RefOp remove(Reference const *ref, IdentifiableObject *target, SeekerRemoveLambda handler) const
  {
    return this->remove(ref, target, handler, 0);
  }

  private: RefOp remove(Reference const *ref, IdentifiableObject *target, SeekerRemoveLambda handler,
                        Int *index) const;

  /// @}

  /// @name Data Read Functions
  /// @{

  public: IdentifiableObject* get(Reference const *ref, IdentifiableObject *source) const
  {
    IdentifiableObject *result;
    if (!this->tryGet(ref, source, result)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
    return result;
  }

  public: IdentifiableObject* tryGet(Reference const *ref, IdentifiableObject *source) const
  {
    IdentifiableObject *result = 0;
    this->tryGet(ref, source, result);
    return result;
  }

  public: void get(Reference const *ref, IdentifiableObject *source, IdentifiableObject *&retVal,
                   TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) const
  {
    if (!this->tryGet(ref, source, retVal, parentTypeInfo, retParent)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryGet(Reference const *ref, IdentifiableObject *source, IdentifiableObject *&retVal,
                      TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) const
  {
    Bool ret = false;
    this->forEach(ref, source, [=,&ret,&retVal](Int i, IdentifiableObject *obj, IdentifiableObject *parent)->RefOp {
      retVal = obj;
      if (retParent) *retParent = parent;
      ret = true;
      return RefOp::STOP;
    }, parentTypeInfo);
    return ret;
  }

  public: template<class T> Bool tryGet(Reference const *ref, IdentifiableObject *source,
                                        IdentifiableObject *&retVal, T *&retParent) const
  {
    IdentifiableObject *retIoParent;
    Bool result = this->tryGet(ref, source, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
    return result;
  }

  public: RefOp forEach(Reference const *ref, IdentifiableObject *source, SeekerForeachLambda handler,
                        TypeInfo const *parentTypeInfo=0) const
  {
    return this->forEach(ref, source, handler, parentTypeInfo, 0, 0);
  }

  private: RefOp forEach(Reference const *ref, IdentifiableObject *source, SeekerForeachLambda handler,
                         TypeInfo const *parentTypeInfo, IdentifiableObject *parent, Int *index) const;

  /// @}

}; // class

} } // namespace

#endif

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

#ifndef CORE_DATA_QUALIFIERSEEKER_H
#define CORE_DATA_QUALIFIERSEEKER_H

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

  /// @name Data Write Functions
  /// @{

  public: void set(Char const *qualifier, IdentifiableObject *target, IdentifiableObject *val) const
  {
    if (!this->trySet(qualifier, target, val)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool trySet(Char const *qualifier, IdentifiableObject *target, IdentifiableObject *val) const
  {
    Bool ret = false;
    this->set(qualifier, target, [=,&ret](Int index, IdentifiableObject *&o)->RefOp {
      o = val;
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

  public: RefOp set(Char const *qualifier, IdentifiableObject *target, SeekerSetLambda handler) const
  {
    return this->set(qualifier, target, handler, 0);
  }

  private: RefOp set(Char const *qualifier, IdentifiableObject *target, SeekerSetLambda handler,
                     Int *index) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(Char const *qualifier, IdentifiableObject *target) const
  {
    if (!this->tryRemove(qualifier, target)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryRemove(Char const *qualifier, IdentifiableObject *target) const
  {
    Bool ret = false;
    this->remove(qualifier, target, [&ret](Int index, IdentifiableObject *o)->RefOp {
      ret = true;
      return RefOp::PERFORM_AND_MOVE;
    });
    return ret;
  }

  public: RefOp remove(Char const *qualifier, IdentifiableObject *target, SeekerRemoveLambda handler) const
  {
    return this->remove(qualifier, target, handler, 0);
  }

  private: RefOp remove(Char const *qualifier, IdentifiableObject *target, SeekerRemoveLambda handler,
                        Int *index) const;

  /// @}

  /// @name Data Read Functions
  /// @{

  public: IdentifiableObject* get(Char const *qualifier, IdentifiableObject *source) const
  {
    IdentifiableObject *result;
    if (!this->tryGet(qualifier, source, result)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
    return result;
  }

  public: IdentifiableObject* tryGet(Char const *qualifier, IdentifiableObject *source) const
  {
    IdentifiableObject *result = 0;
    this->tryGet(qualifier, source, result);
    return result;
  }

  public: void get(Char const *qualifier, IdentifiableObject *source, IdentifiableObject *&retVal,
                   TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) const
  {
    if (!this->tryGet(qualifier, source, retVal, parentTypeInfo, retParent)) {
      throw EXCEPTION(GenericException, STR("Reference pointing to a missing element/tree."));
    }
  }

  public: Bool tryGet(Char const *qualifier, IdentifiableObject *source, IdentifiableObject *&retVal,
                      TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0) const
  {
    Bool ret = false;
    this->forEach(qualifier, source, [=,&ret,&retVal](Int i, IdentifiableObject *obj, IdentifiableObject *parent)->RefOp {
      retVal = obj;
      if (retParent) *retParent = parent;
      ret = true;
      return RefOp::STOP;
    }, parentTypeInfo);
    return ret;
  }

  public: template<class T> Bool tryGet(Char const *qualifier, IdentifiableObject *source,
                                        IdentifiableObject *&retVal, T *&retParent) const
  {
    IdentifiableObject *retIoParent;
    Bool result = this->tryGet(qualifier, source, retVal, T::getTypeInfo(), &retIoParent);
    retParent = static_cast<T*>(retIoParent);
    return result;
  }

  public: RefOp forEach(Char const *qualifier, IdentifiableObject *source, SeekerForeachLambda handler,
                        TypeInfo const *parentTypeInfo=0) const
  {
    return this->forEach(qualifier, source, handler, parentTypeInfo, 0, 0);
  }

  private: RefOp forEach(Char const *qualifier, IdentifiableObject *source, SeekerForeachLambda handler,
                         TypeInfo const *parentTypeInfo, IdentifiableObject *parent, Int *index) const;

  /// @}

}; // class

} } // namespace

#endif

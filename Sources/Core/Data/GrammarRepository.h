/**
 * @file Core/Data/GrammarRepository.h
 * Contains the header of class Core::Data::GrammarRepository.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_GRAMMARREPOSITORY_H
#define DATA_GRAMMARREPOSITORY_H

namespace Core { namespace Data
{

/**
 * @brief A stack of IdentifiableObject derived data objects.
 */
class GrammarRepository : public IdentifiableObject,
                          public virtual SharedProvider, public virtual SharedTracer
{
  //============================================================================
  // Type Info

  TYPE_INFO(GrammarRepository, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(IdentifiableObject, SharedProvider, SharedTracer);


  //============================================================================
  // Member Variables

  private: SharedRepository repository;


  //============================================================================
  // Constructor & Destructor

  public: GrammarRepository();

  public: virtual ~GrammarRepository()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: SharedPtr<GrammarModule> getRoot() const
  {
    return this->repository.getLevelData(GrammarScopeIndex::ROOT).s_cast<GrammarModule>();
  }

  public: GrammarModule* getPlainRoot() const
  {
    return this->repository.getLevelData(GrammarScopeIndex::ROOT).s_cast_get<GrammarModule>();
  }

  public: void setModule(SharedPtr<Module> const &module);

  public: SharedPtr<Module> getModule() const
  {
    return this->repository.getLevelData(GrammarScopeIndex::MODULE).s_cast<Module>();
  }

  public: Module* getPlainModule() const
  {
    return this->repository.getLevelData(GrammarScopeIndex::MODULE).s_cast_get<Module>();
  }

  public: SharedPtr<Module> getPModule() const
  {
    return this->repository.getLevelData(GrammarScopeIndex::PMODULE).s_cast<Module>();
  }

  public: Module* getPlainPModule() const
  {
    return this->repository.getLevelData(GrammarScopeIndex::PMODULE).s_cast_get<Module>();
  }

  public: void clear();

  /// @}

  /// @name SharedProvider Implementation
  /// @{

  public: virtual void setSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> const &val);

  public: virtual void setSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> const &val);

  public: virtual Bool trySetSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> const &val);

  public: virtual Bool trySetSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> const &val);

  public: virtual SharedPtr<IdentifiableObject> getSharedValue(Reference const *ref)
  {
    return this->repository.getSharedValue(ref);
  }

  public: virtual void getSharedValue(Reference const *ref, SharedModulePairedPtr &retVal)
  {
    this->repository.getSharedValue(ref, retVal);
  }

  public: virtual SharedPtr<IdentifiableObject> getSharedValue(Char const *qualifier)
  {
    return this->repository.getSharedValue(qualifier);
  }

  public: virtual void getSharedValue(Char const *qualifier, SharedModulePairedPtr &retVal)
  {
    this->repository.getSharedValue(qualifier, retVal);
  }

  public: virtual Bool tryGetSharedValue(Reference const *ref, SharedPtr<IdentifiableObject> &retVal)
  {
    return this->repository.tryGetSharedValue(ref, retVal);
  }

  public: virtual Bool tryGetSharedValue(Reference const *ref, SharedModulePairedPtr &retVal)
  {
    return this->repository.tryGetSharedValue(ref, retVal);
  }

  public: virtual Bool tryGetSharedValue(Char const *qualifier, SharedPtr<IdentifiableObject> &retVal)
  {
    return this->repository.tryGetSharedValue(qualifier, retVal);
  }

  public: virtual Bool tryGetSharedValue(Char const *qualifier, SharedModulePairedPtr &retVal)
  {
    return this->repository.tryGetSharedValue(qualifier, retVal);
  }

  /// @}

  /// @name Provider Implementation
  /// @{

  public: virtual void removeValue(Reference const *ref)
  {
    this->repository.removeValue(ref);
  }

  public: virtual void removeValue(Char const *qualifier)
  {
    this->repository.removeValue(qualifier);
  }

  public: virtual Bool tryRemoveValue(Reference const *ref)
  {
    return this->repository.tryRemoveValue(ref);
  }

  public: virtual Bool tryRemoveValue(Char const *qualifier)
  {
    return this->repository.tryRemoveValue(qualifier);
  }

  public: virtual IdentifiableObject* getPlainValue(Reference const *ref)
  {
    return this->repository.getPlainValue(ref);
  }

  public: virtual void getPlainValue(Reference const *ref, PlainModulePairedPtr &retVal)
  {
    this->repository.getPlainValue(ref, retVal);
  }

  public: virtual IdentifiableObject* getPlainValue(Char const *qualifier)
  {
    return this->repository.getPlainValue(qualifier);
  }

  public: virtual void getPlainValue(Char const *qualifier, PlainModulePairedPtr &retVal)
  {
    this->repository.getPlainValue(qualifier, retVal);
  }

  public: virtual Bool tryGetPlainValue(Reference const *ref, IdentifiableObject *&retVal)
  {
    return this->repository.tryGetPlainValue(ref, retVal);
  }

  public: virtual Bool tryGetPlainValue(Reference const *ref, PlainModulePairedPtr &retVal)
  {
    return this->repository.tryGetPlainValue(ref, retVal);
  }

  public: virtual Bool tryGetPlainValue(Char const *qualifier, IdentifiableObject *&retVal)
  {
    return this->repository.tryGetPlainValue(qualifier, retVal);
  }

  public: virtual Bool tryGetPlainValue(Char const *qualifier, PlainModulePairedPtr &retVal)
  {
    return this->repository.tryGetPlainValue(qualifier, retVal);
  }

  /// @}

  /// @name SharedTracer Implementation
  /// @{

  public: virtual IdentifiableObject* tracePlainValue(IdentifiableObject *val);

  public: virtual void tracePlainValue(IdentifiableObject *val, PlainModulePairedPtr &retVal);

  public: virtual SharedPtr<IdentifiableObject> traceSharedValue(const SharedPtr<IdentifiableObject> &val);

  public: virtual void traceSharedValue(const SharedPtr<IdentifiableObject> &val, SharedModulePairedPtr &retVal);

  /// @}

}; // class

} } // namespace

#endif

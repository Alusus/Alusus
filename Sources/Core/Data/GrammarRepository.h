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
                          public virtual Provider, public virtual Tracer
{
  //============================================================================
  // Type Info

  TYPE_INFO(GrammarRepository, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(IdentifiableObject, Provider, Tracer);


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

  /// @name Provider Implementation
  /// @{

  public: virtual Bool trySet(Reference const *ref, IdentifiableObject *val);

  public: virtual Bool trySet(Char const *qualifier, IdentifiableObject *val);

  public: virtual Bool tryRemove(Reference const *ref)
  {
    return this->repository.tryRemove(ref);
  }

  public: virtual Bool tryRemove(Char const *qualifier)
  {
    return this->repository.tryRemove(qualifier);
  }

  using Provider::tryGet;

  public: virtual Bool tryGet(Reference const *ref, IdentifiableObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0)
  {
    return this->repository.tryGet(ref, retVal, parentTypeInfo, retParent);
  }

  public: virtual Bool tryGet(Char const *qualifier, IdentifiableObject *&retVal,
                              TypeInfo const *parentTypeInfo=0, IdentifiableObject **retParent=0)
  {
    return this->repository.tryGet(qualifier, retVal, parentTypeInfo, retParent);
  }

  /// @}

  /// @name Tracer Implementation
  /// @{

  using Tracer::traceValue;

  public: virtual void traceValue(IdentifiableObject *val, IdentifiableObject *&retVal, Module *&retModule);

  /// @}

}; // class

} } // namespace

#endif

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

#ifndef CORE_DATA_GRAMMARREPOSITORY_H
#define CORE_DATA_GRAMMARREPOSITORY_H

namespace Core { namespace Data
{

/**
 * @brief A stack of TiObject derived data objects.
 */
class GrammarRepository : public TiObject,
                          public virtual Provider, public virtual Tracer
{
  //============================================================================
  // Type Info

  TYPE_INFO(GrammarRepository, TiObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(TiObject, Provider, Tracer);


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

  public: void initializeObject(TiObject *obj);

  /// @}

  /// @name Provider Implementation
  /// @{

  public: using Provider::set;
  public: using Provider::remove;

  public: virtual void set(Reference const *ref, SeekerSetLambda handler);

  public: virtual void set(Char const *qualifier, SeekerSetLambda handler);

  public: virtual void remove(Reference const *ref, SeekerRemoveLambda handler)
  {
    this->repository.remove(ref, handler);
  }

  public: virtual void remove(Char const *qualifier, SeekerRemoveLambda handler)
  {
    this->repository.remove(qualifier, handler);
  }

  public: virtual void forEach(Reference const *ref, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo=0)
  {
    this->repository.forEach(ref, handler, parentTypeInfo);
  }

  public: virtual void forEach(Char const *qualifier, SeekerForeachLambda handler,
                               TypeInfo const *parentTypeInfo=0)
  {
    this->repository.forEach(qualifier, handler, parentTypeInfo);
  }

  /// @}

  /// @name Tracer Implementation
  /// @{

  using Tracer::traceValue;

  public: virtual void traceValue(TiObject *val, TiObject *&retVal, Module *&retModule);

  /// @}

}; // class

} } // namespace

#endif

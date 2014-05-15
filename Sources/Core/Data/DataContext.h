/**
 * @file Core/Data/DataContext.h
 * Contains the header of class Core::Data::DataContext.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_DATACONTEXT_H
#define DATA_DATACONTEXT_H

namespace Core { namespace Data
{

// TODO: DOC

class DataContext : public IdentifiableObject, public virtual PlainProvider
{
  //============================================================================
  // Type Info

  TYPE_INFO(DataContext, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, PlainProvider);


  //============================================================================
  // Member Variables

  private: Module *rootModule;
  private: Module *currentModule;
  private: VariableStack *variableStack;
  private: Map *currentArgumentList;

  private: ReferenceSeeker referenceSeeker;
  private: QualifierSeeker qualifierSeeker;


  //============================================================================
  // Signals

  // TODO: Do we need this?

  //public: SIGNAL(moduleSwitchNotifier, (Module *old_module, Module *new_module), (old_module, new_module));


  //============================================================================
  // Constructor & Destructor

  public: DataContext(Module *r = 0) :
    rootModule(r), currentModule(0), variableStack(0), currentArgumentList(0),
    referenceSeeker(static_cast<Provider*>(this)), qualifierSeeker(static_cast<Provider*>(this))
  {
  }

  public: virtual ~DataContext()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setRootModule(Module *r)
  {
    this->rootModule = r;
    this->currentModule = 0;
    if (r != 0) this->switchCurrentModule(r);
  }

  public: Module* getRootModule() const
  {
    return this->rootModule;
  }

  public: void switchCurrentModule(Module *m);

  public: Module* getCurrentModule() const
  {
    return this->currentModule;
  }

  public: void setVariableStack(VariableStack *vs)
  {
    this->variableStack = vs;
  }

  public: VariableStack* getVariableStack() const
  {
    return this->variableStack;
  }

  public: void setCurrentArgumentList(Map *args)
  {
    this->currentArgumentList = args;
  }

  public: Map* getCurrentArgumentList() const
  {
    return this->currentArgumentList;
  }

  public: void copyFrom(const DataContext *context)
  {
    this->rootModule = context->getRootModule();
    this->variableStack = context->getVariableStack();
    this->currentModule = context->getCurrentModule();
    this->currentArgumentList = context->getCurrentArgumentList();
  }

  /// @}

  /// @name Data Scope Functions
  /// @{

  private: IdentifiableObject* getStartingParent(ReferenceScope scope) const;

  private: IdentifiableObject* getStartingParent(const Char *&qualifier) const;

  private: IdentifiableObject* tryGetStartingParent(ReferenceScope scope) const;

  private: IdentifiableObject* tryGetStartingParent(const Char *&qualifier) const;

  /// @}

  /// @name PlainProvider Implementation
  /// @{

  public: virtual void setPlainValue(Reference *ref, IdentifiableObject *val)
  {
    this->referenceSeeker.setPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()), val);
  }

  public: virtual void setPlainValue(const Char *qualifier, IdentifiableObject *val)
  {
    this->qualifierSeeker.setPlain(qualifier, this->getStartingParent(qualifier), val);
  }

  public: virtual Bool trySetPlainValue(Reference *ref, IdentifiableObject *val)
  {
    return this->referenceSeeker.trySetPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()), val);
  }

  public: virtual Bool trySetPlainValue(const Char *qualifier, IdentifiableObject *val)
  {
    return this->qualifierSeeker.trySetPlain(qualifier, this->getStartingParent(qualifier), val);
  }

  public: virtual void removeValue(Reference *ref)
  {
    this->referenceSeeker.remove(ref->getSegment().get(), this->getStartingParent(ref->getScope()));
  }

  public: virtual void removeValue(const Char *qualifier)
  {
    this->qualifierSeeker.remove(qualifier, this->getStartingParent(qualifier));
  }

  public: virtual Bool tryRemoveValue(Reference *ref)
  {
    return this->referenceSeeker.tryRemove(ref->getSegment().get(), this->getStartingParent(ref->getScope()));
  }

  public: virtual Bool tryRemoveValue(const Char *qualifier)
  {
    return this->qualifierSeeker.tryRemove(qualifier, this->getStartingParent(qualifier));
  }

  public: virtual IdentifiableObject* getPlainValue(Reference *ref) const
  {
    return this->referenceSeeker.getPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()));
  }

  public: virtual void getPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
  {
    return this->referenceSeeker.getPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()),
                                 retVal, retModule);
  }

  public: virtual IdentifiableObject* getPlainValue(const Char *qualifier) const
  {
    return this->qualifierSeeker.getPlain(qualifier, this->getStartingParent(qualifier));
  }

  public: virtual void getPlainValue(const Char *qualifier, IdentifiableObject *&retVal,
                                     Module *&retModule) const
  {
    return this->qualifierSeeker.getPlain(qualifier, this->getStartingParent(qualifier), retVal, retModule);
  }

  public: virtual Bool tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal) const
  {
    auto parent = this->tryGetStartingParent(ref->getScope());
    if (parent == 0) return false;
    return this->referenceSeeker.tryGetPlain(ref->getSegment().get(), parent, retVal);
  }

  public: virtual Bool tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
  {
    auto parent = this->tryGetStartingParent(ref->getScope());
    if (parent == 0) return false;
    return this->referenceSeeker.tryGetPlain(ref->getSegment().get(), parent, retVal, retModule);
  }

  public: virtual Bool tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal) const
  {
    auto parent = this->tryGetStartingParent(qualifier);
    if (parent == 0) return false;
    return this->qualifierSeeker.tryGetPlain(qualifier, parent, retVal);
  }

  public: virtual Bool tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal,
                                        Module *&retModule) const
  {
    auto parent = this->tryGetStartingParent(qualifier);
    if (parent == 0) return false;
    return this->qualifierSeeker.tryGetPlain(qualifier, parent, retVal, retModule);
  }

  /// @}

}; // class

} } // namespace

#endif

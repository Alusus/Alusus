/**
 * @file Core/Data/ParsingGrammarContext.h
 * Contains the header of class Core::Data::ParsingGrammarContext.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_PARSING_GRAMMAR_CONTEXT_H
#define DATA_PARSING_GRAMMAR_CONTEXT_H

namespace Core { namespace Data
{

// TODO: DOC

class ParsingGrammarContext : public IdentifiableObject, public virtual Provider
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParsingGrammarContext, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, Provider);


  //============================================================================
  // Member Variables

  private: GrammarModule *rootModule;
  private: Module *currentModule;
  private: VariableStack *variableStack;
  private: Map *currentArgumentList;

  private: Seeker seeker;
  private: RawSeeker rawSeeker;


  //============================================================================
  // Signals

  // TODO: Do we need these?
  //public: SIGNAL(lexerModuleSwitchNotifier, (Module *old_module, Module *new_module), (old_module, new_module));
  //public: SIGNAL(parserModuleSwitchNotifier, (Module *old_module, Module *new_module), (old_module, new_module));


  //============================================================================
  // Constructor & Destructor

  public: ParsingGrammarContext(GrammarModule *r = 0) :
    rootModule(r), currentModule(0), variableStack(0), currentArgumentList(0),
    seeker(static_cast<Provider*>(this)), rawSeeker(static_cast<Provider*>(this))
  {
  }

  public: virtual ~ParsingGrammarContext()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setRootModule(GrammarModule *r)
  {
    this->rootModule = r;
    this->currentModule = 0;
    if (r != 0) this->switchCurrentModule(r);
  }

  public: GrammarModule* getRootModule() const
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

  public: void copyFrom(const ParsingGrammarContext *context)
  {
    this->rootModule = context->getRootModule();
    this->variableStack = context->getVariableStack();
    this->currentModule = context->getCurrentModule();
    this->currentArgumentList = context->getCurrentArgumentList();
  }

  public: Module* findCurrentLexerModule() const
  {
    return this->findAssociatedLexerModule(this->currentModule);
  }

  public: Module* findAssociatedLexerModule(Module *module) const;

  // TODO: Find module for other dimensions.

  /// @}

  /// @name Data Access Functions
  /// @{

  private: IdentifiableObject* getStartingParent(ReferenceScope scope) const;

  private: IdentifiableObject* getStartingParent(const Char *&qualifier) const;

  public: void setValue(Reference *ref, IdentifiableObject *val)
  {
    this->seeker.setPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()), val);
  }

  public: void removeValue(Reference *ref)
  {
    this->seeker.remove(ref->getSegment().get(), this->getStartingParent(ref->getScope()));
  }

  public: IdentifiableObject* getValue(Reference *ref) const
  {
    return this->seeker.getPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()));
  }

  public: void getValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
  {
    this->seeker.getPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()),
                          retVal, retModule);
  }

  /// @}

  //============================================================================
  // Provider Implementation

  /// @name Provider Implementation
  /// @{

  public: virtual IdentifiableObject* getPlainValue(Reference *ref) const
  {
    return this->seeker.getPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()));
  }

  public: virtual void getPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
  {
    return this->seeker.getPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()),
                                 retVal, retModule);
  }

  public: virtual IdentifiableObject* getPlainValue(const Char *qualifier) const
  {
    return this->rawSeeker.getPlain(qualifier, this->getStartingParent(qualifier));
  }

  public: virtual void getPlainValue(const Char *qualifier, IdentifiableObject *&retVal,
                                     Module *&retModule) const
  {
    return this->rawSeeker.getPlain(qualifier, this->getStartingParent(qualifier), retVal, retModule);
  }

  public: virtual Bool tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal) const
  {
    return this->seeker.tryGetPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()),
                                    retVal);
  }

  public: virtual Bool tryGetPlainValue(Reference *ref, IdentifiableObject *&retVal, Module *&retModule) const
  {
    return this->seeker.tryGetPlain(ref->getSegment().get(), this->getStartingParent(ref->getScope()),
                                    retVal, retModule);
  }

  public: virtual Bool tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal) const
  {
    return this->rawSeeker.tryGetPlain(qualifier, this->getStartingParent(qualifier), retVal);
  }

  public: virtual Bool tryGetPlainValue(const Char *qualifier, IdentifiableObject *&retVal,
                                        Module *&retModule) const
  {
    return this->rawSeeker.tryGetPlain(qualifier, this->getStartingParent(qualifier), retVal, retModule);
  }

  /// @}

}; // class

} } // namespace

#endif

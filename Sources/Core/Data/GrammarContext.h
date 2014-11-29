/**
 * @file Core/Data/GrammarContext.h
 * Contains the header of class Core::Data::GrammarContext.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_GRAMMARCONTEXT_H
#define DATA_GRAMMARCONTEXT_H

namespace Core { namespace Data
{

// TODO: DOC

class GrammarContext : public IdentifiableObject, public virtual Tracer
{
  //============================================================================
  // Type Info

  TYPE_INFO(GrammarContext, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(IdentifiableObject, Tracer);


  //============================================================================
  // Member Variables

  private: PlainRepository repository;


  //============================================================================
  // Constructor & Destructor

  public: GrammarContext();


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: PlainRepository const* getRepository() const
  {
    return &this->repository;
  }

  public: void copyFrom(GrammarContext const *context)
  {
    this->repository.copyFrom(context->getRepository());
  }

  public: void setRoot(Module *module)
  {
    this->repository.setLevel(module, GrammarScopeIndex::ROOT);
    this->repository.setLevel(module, GrammarScopeIndex::MODULE);
    this->repository.setLevel(0, GrammarScopeIndex::PMODULE);
  }

  public: Module* getRoot() const
  {
    return static_cast<Module*>(this->repository.getLevelData(GrammarScopeIndex::ROOT));
  }

  public: void setModule(Module *module)
  {
    this->repository.setLevel(module, GrammarScopeIndex::MODULE);
    if (module != 0 && module->isA<Data::GrammarModule>()) {
      this->repository.setLevel(static_cast<GrammarModule*>(module)->getParent(), GrammarScopeIndex::PMODULE);
    } else {
      this->repository.setLevel(0, GrammarScopeIndex::PMODULE);
    }
  }

  public: Module* getModule() const
  {
    return static_cast<Module*>(this->repository.getLevelData(GrammarScopeIndex::MODULE));
  }

  public: Module* getPModule() const
  {
    return static_cast<Module*>(this->repository.getLevelData(GrammarScopeIndex::PMODULE));
  }

  public: void setStack(VariableStack *stack)
  {
    this->repository.setLevel(stack, GrammarScopeIndex::STACK);
  }

  public: VariableStack* getStack() const
  {
    return static_cast<VariableStack*>(this->repository.getLevelData(GrammarScopeIndex::STACK));
  }

  public: void setArgs(IdentifiableObject *args)
  {
    this->repository.setLevel(args, GrammarScopeIndex::ARGS);
  }

  public: IdentifiableObject* getArgs() const
  {
    return this->repository.getLevelData(GrammarScopeIndex::ARGS);
  }

  /// @}

  /// @name Misc Functions
  /// @{

  public: IdentifiableObject* traceValue(IdentifiableObject *val, Module *module)
  {
    PlainModulePairedPtr retVal;
    this->traceValue(val, retVal, module);
    return retVal.object;
  }

  public: void traceValue(IdentifiableObject *val, PlainModulePairedPtr &retVal, Module *module);

  /// @}

  /// @name Tracer Implementation
  /// @{

  public: virtual IdentifiableObject* traceValue(IdentifiableObject *val)
  {
    return this->traceValue(val, 0);
  }

  public: virtual void traceValue(IdentifiableObject *val, PlainModulePairedPtr &retVal)
  {
    this->traceValue(val, retVal, 0);
  }

  /// @}

  /// @name Term Helper Functions
  /// @{

  public: void getListTermData(ListTerm *term, PlainModulePairedPtr &retVal, Module *module=0);

  public: Word getListTermChildCount(ListTerm *term, PlainModulePairedPtr const &listData) const;

  public: void getListTermChild(ListTerm *term, Int index, PlainModulePairedPtr &listData,
                                Term *&retTerm, PlainModulePairedPtr &retData) const;

  public: void useListTermChild(ListTerm *term, Int index, PlainModulePairedPtr &listData,
                                Term *&retTerm, PlainModulePairedPtr *retData);

  public: Integer* getTokenTermId(TokenTerm *term, Module *module=0);

  public: IdentifiableObject* getTokenTermText(TokenTerm *term, Module *module=0);

  public: void getReferencedCharGroup(Reference const *ref, CharGroupDefinition *&charGroupDef, Module *module=0);

  public: void getReferencedSymbol(Reference const *ref, Module *&retModule, SymbolDefinition *&retDef,
                                   Module *module=0);

  public: Integer* getMultiplyTermMax(MultiplyTerm *term, Module *module=0);

  public: Integer* getMultiplyTermMin(MultiplyTerm *term, Module *module=0);

  public: Integer* getMultiplyTermPriority(MultiplyTerm *term, Module *module=0);

  /// @}

  /// @name Symbol Definition Helper Functions
  /// @{

  public: Term* getSymbolTerm(const SymbolDefinition *definition, Module *module=0);

  public: SharedMap* getSymbolVars(const SymbolDefinition *definition, Module *module=0);

  /// @}

  /// @name Other Helper Functions
  /// @{

  public: Module* getAssociatedLexerModule(Module *module=0);

  /// @}

}; // class

} } // namespace

#endif

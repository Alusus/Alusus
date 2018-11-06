/**
 * @file Core/Data/Grammar/Context.h
 * Contains the header of class Core::Data::Grammar::Context.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_CONTEXT_H
#define CORE_DATA_GRAMMAR_CONTEXT_H

namespace Core::Data::Grammar
{

class Context : public TiObject, public virtual ExMapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Context, TiObject, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(ExMapContaining<TiObject>)
  ));


  //============================================================================
  // Member Variables

  private: Module *root = 0;
  private: Module *module = 0;
  private: Module *bmodule = 0;
  private: VariableStack *stack = 0;
  private: TiObject *args = 0;


  //============================================================================
  // Constructor & Destructor

  public: Context()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void copyFrom(Context const *context)
  {
    this->root = context->getRoot();
    this->module = context->getModule();
    this->bmodule = context->getBmodule();
    this->stack = context->getStack();
    this->args = context->getArgs();
  }

  public: void setRoot(Module *r)
  {
    this->root = r;
  }

  public: Module* getRoot() const
  {
    return this->root;
  }

  public: void setModule(Module *m)
  {
    this->module = m;
    if (this->module != 0) this->bmodule = m->getBase();
    else this->bmodule = 0;
  }

  public: Module* getModule() const
  {
    return this->module;
  }

  public: Module* getBmodule() const
  {
    return this->bmodule;
  }

  public: void setStack(VariableStack *s)
  {
    this->stack = s;
  }

  public: VariableStack* getStack() const
  {
    return this->stack;
  }

  public: void setArgs(TiObject *a)
  {
    this->args = a;
  }

  public: TiObject* getArgs() const
  {
    return this->args;
  }

  /// @}

  /// @name Misc Functions
  /// @{

  public: TiObject* traceValue(TiObject *val)
  {
    TiObject *retVal;
    Module *retModule;
    this->traceValue(val, retVal, retModule);
    return retVal;
  }

  public: void traceValue(TiObject *val, TiObject *&retVal, Module *&retModule)
  {
    this->traceValue(val, 0, retVal, retModule);
  }

  public: TiObject* traceValue(TiObject *val, Module *module)
  {
    TiObject *retVal;
    Module *retModule;
    this->traceValue(val, module, retVal, retModule);
    return retVal;
  }

  public: void traceValue(TiObject *val, Module *module, TiObject *&retVal, Module *&retModule);

  /// @}

  /// @name Term Helper Functions
  /// @{

  public: void getListTermData(ListTerm *term, PlainPairedPtr &retVal, Module *module=0);

  public: Word getListTermChildCount(ListTerm *term, PlainPairedPtr const &listData) const;

  public: void getListTermChild(ListTerm *term, Int index, PlainPairedPtr &listData,
                                Term *&retTerm, PlainPairedPtr &retData) const;

  public: void useListTermChild(ListTerm *term, Int index, PlainPairedPtr &listData,
                                Term *&retTerm, PlainPairedPtr *retData);

  public: TiInt* getTokenTermId(TokenTerm *term, Module *module=0);

  public: TiObject* getTokenTermText(TokenTerm *term, Module *module=0);

  public: void getReferencedCharGroup(Reference const *ref, CharGroupDefinition *&charGroupDef, Module *module=0);

  public: void getReferencedSymbol(Reference const *ref, Module *&retModule, SymbolDefinition *&retDef,
                                   Module *module=0);

  public: TiInt* getMultiplyTermMax(MultiplyTerm *term, Module *module=0);

  public: TiInt* getMultiplyTermMin(MultiplyTerm *term, Module *module=0);

  public: TiInt* getMultiplyTermPriority(MultiplyTerm *term, Module *module=0);

  public: TiInt* getTermFlags(Term *term, Module *module=0);

  /// @}

  /// @name Symbol Definition Helper Functions
  /// @{

  public: Term* getSymbolTerm(SymbolDefinition const *definition, Module *module=0);

  public: Map* getSymbolVars(SymbolDefinition const *definition, Module *module=0);

  public: TiInt* getSymbolPriority(SymbolDefinition const *definition, Module *module=0);

  public: TiInt* getSymbolFlags(SymbolDefinition const *definition, Module *module=0);

  /// @}

  /// @name Other Helper Functions
  /// @{

  public: Module* getAssociatedLexerModule(Module *module=0);

  public: List* getAssociatedErrorSyncBlockPairs(Module *module=0);

  /// @}

  /// @name MapContaining Implementation
  /// @{

  public: virtual void setElement(Int index, TiObject *val);
  public: virtual Int setElement(Char const *key, TiObject *val);

  public: virtual Word getElementCount() const
  {
    return 5;
  }

  public: virtual TiObject* getElement(Int index) const;
  public: virtual TiObject* getElement(Char const *key) const;

  public: virtual TypeInfo* getElementNeededType(Int index) const;
  public: virtual TypeInfo* getElementNeededType(Char const *key) const;

  public: virtual HoldMode getElementHoldMode(Int index) const
  {
    return HoldMode::PLAIN_REF;
  }
  public: virtual HoldMode getElementHoldMode(Char const *key) const
  {
    return HoldMode::PLAIN_REF;
  }

  public: virtual SbStr const& getElementKey(Int index) const;

  public: virtual Int findElementIndex(Char const *key) const;

  /// @}

}; // class

} // namespace

#endif

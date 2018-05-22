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

class Context : public TiObject, public virtual Basic::MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Context, TiObject, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Basic::MapContaining<TiObject>)
  ));


  //============================================================================
  // Member Variables

  private: GrammarModule *root = 0;
  private: GrammarModule *module = 0;
  private: GrammarModule *bmodule = 0;
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

  public: void setRoot(GrammarModule *r)
  {
    this->root = r;
  }

  public: GrammarModule* getRoot() const
  {
    return this->root;
  }

  public: void setModule(GrammarModule *m)
  {
    this->module = m;
    if (this->module != 0) this->bmodule = m->getBase();
    else this->bmodule = 0;
  }

  public: GrammarModule* getModule() const
  {
    return this->module;
  }

  public: GrammarModule* getBmodule() const
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
    GrammarModule *retModule;
    this->traceValue(val, retVal, retModule);
    return retVal;
  }

  public: void traceValue(TiObject *val, TiObject *&retVal, GrammarModule *&retModule)
  {
    this->traceValue(val, 0, retVal, retModule);
  }

  public: TiObject* traceValue(TiObject *val, GrammarModule *module)
  {
    TiObject *retVal;
    GrammarModule *retModule;
    this->traceValue(val, module, retVal, retModule);
    return retVal;
  }

  public: void traceValue(TiObject *val, GrammarModule *module, TiObject *&retVal, GrammarModule *&retModule);

  /// @}

  /// @name Term Helper Functions
  /// @{

  public: void getListTermData(ListTerm *term, PlainPairedPtr &retVal, GrammarModule *module=0);

  public: Word getListTermChildCount(ListTerm *term, PlainPairedPtr const &listData) const;

  public: void getListTermChild(ListTerm *term, Int index, PlainPairedPtr &listData,
                                Term *&retTerm, PlainPairedPtr &retData) const;

  public: void useListTermChild(ListTerm *term, Int index, PlainPairedPtr &listData,
                                Term *&retTerm, PlainPairedPtr *retData);

  public: TiInt* getTokenTermId(TokenTerm *term, GrammarModule *module=0);

  public: TiObject* getTokenTermText(TokenTerm *term, GrammarModule *module=0);

  public: void getReferencedCharGroup(Reference const *ref, CharGroupDefinition *&charGroupDef, GrammarModule *module=0);

  public: void getReferencedSymbol(Reference const *ref, GrammarModule *&retModule, SymbolDefinition *&retDef,
                                   GrammarModule *module=0);

  public: TiInt* getMultiplyTermMax(MultiplyTerm *term, GrammarModule *module=0);

  public: TiInt* getMultiplyTermMin(MultiplyTerm *term, GrammarModule *module=0);

  public: TiInt* getMultiplyTermPriority(MultiplyTerm *term, GrammarModule *module=0);

  public: TiInt* getTermFlags(Term *term, GrammarModule *module=0);

  /// @}

  /// @name Symbol Definition Helper Functions
  /// @{

  public: Term* getSymbolTerm(SymbolDefinition const *definition, GrammarModule *module=0);

  public: Map* getSymbolVars(SymbolDefinition const *definition, GrammarModule *module=0);

  public: TiInt* getSymbolPriority(SymbolDefinition const *definition, GrammarModule *module=0);

  public: TiInt* getSymbolFlags(SymbolDefinition const *definition, GrammarModule *module=0);

  /// @}

  /// @name Other Helper Functions
  /// @{

  public: GrammarModule* getAssociatedLexerModule(GrammarModule *module=0);

  public: List* getAssociatedErrorSyncBlockPairs(GrammarModule *module=0);

  /// @}

  /// @name MapContaining Implementation
  /// @{

  public: virtual void setElement(Int index, TiObject *val);
  public: virtual Int setElement(Char const *key, TiObject *val);

  public: virtual void removeElement(Int index);
  public: virtual Int removeElement(Char const *key);

  public: virtual Word getElementCount() const
  {
    return 5;
  }

  public: virtual TiObject* getElement(Int index) const;
  public: virtual TiObject* getElement(Char const *key) const;

  public: virtual SbStr const& getElementKey(Int index) const;

  public: virtual Int findElementIndex(Char const *key) const;

  /// @}

}; // class

} // namespace

#endif

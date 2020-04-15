/**
 * @file Core/Data/Grammar/Context.h
 * Contains the header of class Core::Data::Grammar::Context.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_CONTEXT_H
#define CORE_DATA_GRAMMAR_CONTEXT_H

namespace Core::Data::Grammar
{

class Context : public TiObject, public MapContaining<TiObject>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Context, TiObject, "Core.Data.Grammar", "Core", "alusus.org", (
    INHERITANCE_INTERFACES(MapContaining<TiObject>)
  ));


  //============================================================================
  // Member Variables

  private: Module *root = 0;
  private: Module *module = 0;
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
  }

  public: Module* getModule() const
  {
    return this->module;
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

  public: TiObject* traceValue(TiObject *val);

  /// @}

  /// @name Term Helper Functions
  /// @{

  public: TiObject* getListTermFilter(ListTerm *term);

  public: Word getListTermChildCount(ListTerm *term, TiObject *listFilter) const;

  public: Term* getListTermChild(ListTerm *term, Int index, TiObject *listFilter) const;

  public: TiInt* getTokenTermId(TokenTerm *term);

  public: TiObject* getTokenTermText(TokenTerm *term);

  public: CharGroupDefinition* getReferencedCharGroup(Reference const *ref);

  public: SymbolDefinition* getReferencedSymbol(Reference const *ref);

  public: TiInt* getMultiplyTermMax(MultiplyTerm *term);

  public: TiInt* getMultiplyTermMin(MultiplyTerm *term);

  public: TiInt* getTermFlags(Term *term);

  /// @}

  /// @name Symbol Definition Helper Functions
  /// @{

  public: Map* getSymbolVars(SymbolDefinition const *definition);

  public: TiInt* getSymbolFlags(SymbolDefinition const *definition);

  /// @}

  /// @name Other Helper Functions
  /// @{

  public: LexerModule* getAssociatedLexerModule(Module *module=0);

  public: List* getAssociatedErrorSyncBlockPairs(Module *module=0);

  /// @}

  /// @name MapContaining Implementation
  /// @{

  public: virtual void setElement(Int index, TiObject *val);
  public: virtual Int setElement(Char const *key, TiObject *val);

  public: virtual Word getElementCount() const
  {
    return 4;
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

  public: virtual SbStr const getElementKey(Int index) const;

  public: virtual Int findElementIndex(Char const *key) const;

  /// @}

}; // class

} // namespace

#endif

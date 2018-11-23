/**
 * @file Core/Data/Grammar/Module.h
 * Contains the header of class Core::Data::Grammar::Module.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_MODULE_H
#define CORE_DATA_GRAMMAR_MODULE_H

namespace Core::Data::Grammar
{

// TODO: DOC

class Module : public NbMap, public virtual Binding, public virtual Inheriting, public virtual IdHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(Module, NbMap, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Binding, Inheriting, IdHaving)
  ));


  //============================================================================
  // Types

  /**
   * @brief An enumeration for metadata elements of Module.
   * @ingroup core_data
   *
   * This enumeration is used to refer to an element within Module's
   * meta data. It's needed by Module::metaChangeNotifier.
   */
  protected: s_enum(MetaElement,
    START_REF=1, LEXER_MODULE_REF=2, ERROR_SYNC_BLOCK_PAIRS_REF=4, ALL=static_cast<Word>(-1)
  );


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> baseRef;
  private: Module *base = 0;

  private: SharedPtr<Reference> startRef;
  private: SharedPtr<Reference> lexerModuleRef;
  private: SharedPtr<Reference> errorSyncBlockPairsRef;

  private: Word ownership = 0;

  // TODO: Add the other dimensions.


  //============================================================================
  // Signals & Slots

  public: Signal<void, Module*> destroyNotifier;
  public: Signal<void, Module*, Word> metaChangeNotifier;

  public: Slot<void, Module*> baseDestroySlot = {
    this, &Module::onBaseDestroyed
  };

  public: Slot<void, Module*, Word> baseMetaChangeSlot = {
    this, &Module::onBaseMetaChanged
  };


  //============================================================================
  // Implementations

  IMPLEMENT_IDHAVING(Module);

  IMPLEMENT_BINDING(Binding,
    (id, TiWord, VALUE, setId(value), &id),
    (baseRef, Reference, SHARED_REF, setBaseRef(value), baseRef.get()),
    (startRef, Reference, SHARED_REF, setStartRef(value), startRef.get()),
    (lexerModuleRef, Reference, SHARED_REF, setLexerModuleRef(value), lexerModuleRef.get()),
    (errorSyncBlockPairsRef, Reference, SHARED_REF, setErrorSyncBlockPairsRef(value), errorSyncBlockPairsRef.get())
  );


  //============================================================================
  // Constructor & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Module);

  IMPLEMENT_ATTR_CONSTRUCTOR(Module);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Module);

  public: Module(
    Bool useIndex, std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements
  ) : NbMap(useIndex)
  {
    ATTR_INITIALIZATION_LOOP(attrs)
    MAP_ELEMENT_INITIALIZATION_LOOP(elements)
  }

  public: virtual ~Module();

  public: static SharedPtr<Module> create(
    Bool useIndex, std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements
  ) {
    return std::make_shared<Module>(useIndex, attrs, elements);
  }


  //============================================================================
  // Member Functions

  /// @name Inheritance Functions
  /// @{

  public: void setBaseRef(const SharedPtr<Reference> &ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->baseRef, ref);
  }
  private: void setBaseRef(Reference *ref)
  {
    this->setBaseRef(getSharedPtr(ref));
  }

  public: const SharedPtr<Reference>& getBaseRef() const
  {
    return this->baseRef;
  }

  public: void setBase(Module *b)
  {
    if (this->base != 0) this->detachFromBase();
    if (b != 0) this->attachToBase(b);
  }

  public: Module* getBaseModule() const
  {
    return this->base;
  }

  private: void attachToBase(Module *p);

  private: void detachFromBase();

  private: void onBaseDestroyed(Module *obj)
  {
    this->detachFromBase();
  }

  private: void onBaseMetaChanged(Module *obj, Word element);

  /// @}

  /// @name Attributes Functions
  /// @{

  public: void setLexerModuleRef(const SharedPtr<Reference> &lmr)
  {
    UPDATE_OWNED_SHAREDPTR(this->lexerModuleRef, lmr);
    this->ownership |= Module::MetaElement::LEXER_MODULE_REF;
    this->metaChangeNotifier.emit(this, Module::MetaElement::LEXER_MODULE_REF);
  }
  private: void setLexerModuleRef(Reference *lmr)
  {
    this->setLexerModuleRef(getSharedPtr(lmr));
  }

  public: void resetLexerModuleRef()
  {
    RESET_OWNED_SHAREDPTR(this->lexerModuleRef);
    if (this->base != 0) this->lexerModuleRef = this->base->getLexerModuleRef();
    this->ownership &= ~Module::MetaElement::LEXER_MODULE_REF;
    this->metaChangeNotifier.emit(this, Module::MetaElement::LEXER_MODULE_REF);
  }

  public: const SharedPtr<Reference>& getLexerModuleRef() const
  {
    return this->lexerModuleRef;
  }

  public: void setErrorSyncBlockPairsRef(const SharedPtr<Reference> &ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef, ref);
    this->ownership |= Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF;
    this->metaChangeNotifier.emit(this, Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF);
  }
  private: void setErrorSyncBlockPairsRef(Reference *ref)
  {
    this->setErrorSyncBlockPairsRef(getSharedPtr(ref));
  }

  public: void resetErrorSyncBlockPairsRef()
  {
    RESET_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef);
    if (this->base != 0) this->errorSyncBlockPairsRef = this->base->getErrorSyncBlockPairsRef();
    this->ownership &= ~Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF;
    this->metaChangeNotifier.emit(this, Module::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF);
  }

  public: const SharedPtr<Reference>& getErrorSyncBlockPairsRef() const
  {
    return this->errorSyncBlockPairsRef;
  }

  public: void setStartRef(const SharedPtr<Reference> &sr)
  {
    UPDATE_OWNED_SHAREDPTR(this->startRef, sr);
    this->ownership |= Module::MetaElement::START_REF;
    this->metaChangeNotifier.emit(this, Module::MetaElement::START_REF);
  }
  private: void setStartRef(Reference *sr)
  {
    this->setStartRef(getSharedPtr(sr));
  }

  public: void resetStartRef()
  {
    RESET_OWNED_SHAREDPTR(this->startRef);
    if (this->base != 0) this->startRef = this->base->getStartRef();
    this->ownership &= ~Module::MetaElement::START_REF;
    this->metaChangeNotifier.emit(this, Module::MetaElement::START_REF);
  }

  public: const SharedPtr<Reference>& getStartRef() const
  {
    return this->startRef;
  }

  /// @}

  /// @name Inheriting Implementation
  /// @{

  public: virtual Reference* getBaseReference() const
  {
    return this->baseRef.get();
  }

  public: virtual void setBase(TiObject *base)
  {
    Module *baseModule = ti_cast<Module>(base);
    if (baseModule == 0) {
      throw EXCEPTION(GenericException, S("Base reference points to an object of an invalid type."));
    }
    this->setBase(baseModule);
  }

  public: virtual TiObject* getBase() const
  {
    return this->getBaseModule();
  }

  /// @}

  /// @name DataHaving Implementation
  /// @{

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

  /// @}

}; // class

} // namespace

#endif

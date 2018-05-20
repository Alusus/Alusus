/**
 * @file Core/Data/Grammar/GrammarModule.h
 * Contains the header of class Core::Data::Grammar::GrammarModule.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_GRAMMARMODULE_H
#define CORE_DATA_GRAMMAR_GRAMMARMODULE_H

namespace Core::Data::Grammar
{

// TODO: DOC

class GrammarModule : public NbMap, public virtual Bindings, public virtual Initializable, public virtual IdHolder
{
  //============================================================================
  // Type Info

  TYPE_INFO(GrammarModule, NbMap, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Bindings, Initializable, IdHolder)
  ));


  //============================================================================
  // Types

  /**
   * @brief An enumeration for metadata elements of GrammarModule.
   * @ingroup core_data
   *
   * This enumeration is used to refer to an element within GrammarModule's
   * meta data. It's needed by GrammarModule::metaChangeNotifier.
   */
  protected: s_enum(MetaElement,
    START_REF=1, LEXER_MODULE_REF=2, ERROR_SYNC_BLOCK_PAIRS_REF=4, ALL=static_cast<Word>(-1)
  );


  //============================================================================
  // Member Variables

  private: SharedPtr<Grammar::Reference> baseRef;
  private: GrammarModule *base = 0;

  private: SharedPtr<Grammar::Reference> startRef;
  private: SharedPtr<Grammar::Reference> lexerModuleRef;
  private: SharedPtr<Grammar::Reference> errorSyncBlockPairsRef;

  private: Word ownership = 0;

  // TODO: Add the other dimensions.


  //============================================================================
  // Signals & Slots

  public: Signal<void, GrammarModule*> destroyNotifier;
  public: Signal<void, GrammarModule*, Word> metaChangeNotifier;

  public: Slot<void, GrammarModule*> baseDestroySlot = {
    this, &GrammarModule::onBaseDestroyed
  };

  public: Slot<void, GrammarModule*, Word> baseMetaChangeSlot = {
    this, &GrammarModule::onBaseMetaChanged
  };


  //============================================================================
  // Implementations

  IMPLEMENT_IDHOLDER(GrammarModule);

  IMPLEMENT_BINDINGS(Bindings,
    (id, TiWord, VALUE, setId(value), &id),
    (baseRef, Reference, SHARED_REF, setBaseRef(value), baseRef.get()),
    (startRef, Reference, SHARED_REF, setStartRef(value), startRef.get()),
    (lexerModuleRef, Reference, SHARED_REF, setLexerModuleRef(value), lexerModuleRef.get()),
    (errorSyncBlockPairsRef, Reference, SHARED_REF, setErrorSyncBlockPairsRef(value), errorSyncBlockPairsRef.get())
  );


  //============================================================================
  // Constructor & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(GrammarModule);

  IMPLEMENT_ATTR_CONSTRUCTOR(GrammarModule);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(GrammarModule);

  public: GrammarModule(
    Bool useIndex, std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements
  ) : NbMap(useIndex)
  {
    ATTR_INITIALIZATION_LOOP(attrs)
    MAP_ELEMENT_INITIALIZATION_LOOP(elements)
  }

  public: virtual ~GrammarModule();

  public: static SharedPtr<GrammarModule> create(
    Bool useIndex, std::initializer_list<Argument> const &attrs, std::initializer_list<Argument> const &elements
  ) {
    return std::make_shared<GrammarModule>(useIndex, attrs, elements);
  }


  //============================================================================
  // Member Functions

  /// @name Inheritance Functions
  /// @{

  public: void setBaseRef(const SharedPtr<Grammar::Reference> &ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->baseRef, ref);
  }
  private: void setBaseRef(Reference *ref)
  {
    this->setBaseRef(getSharedPtr(ref));
  }

  public: const SharedPtr<Grammar::Reference>& getBaseRef() const
  {
    return this->baseRef;
  }

  public: void setBase(GrammarModule *b)
  {
    if (this->base != 0) this->detachFromBase();
    if (b != 0) this->attachToBase(b);
  }

  public: GrammarModule* getBase() const
  {
    return this->base;
  }

  private: virtual void attachToBase(GrammarModule *p);

  private: virtual void detachFromBase();

  private: void onBaseDestroyed(GrammarModule *obj)
  {
    this->detachFromBase();
  }

  private: void onBaseMetaChanged(GrammarModule *obj, Word element);

  /// @}

  /// @name Attributes Functions
  /// @{

  public: void setLexerModuleRef(const SharedPtr<Reference> &lmr)
  {
    UPDATE_OWNED_SHAREDPTR(this->lexerModuleRef, lmr);
    this->ownership |= GrammarModule::MetaElement::LEXER_MODULE_REF;
    this->metaChangeNotifier.emit(this, GrammarModule::MetaElement::LEXER_MODULE_REF);
  }
  private: void setLexerModuleRef(Reference *lmr)
  {
    this->setLexerModuleRef(getSharedPtr(lmr));
  }

  public: void resetLexerModuleRef()
  {
    RESET_OWNED_SHAREDPTR(this->lexerModuleRef);
    if (this->base != 0) this->lexerModuleRef = this->base->getLexerModuleRef();
    this->ownership &= ~GrammarModule::MetaElement::LEXER_MODULE_REF;
    this->metaChangeNotifier.emit(this, GrammarModule::MetaElement::LEXER_MODULE_REF);
  }

  public: const SharedPtr<Reference>& getLexerModuleRef() const
  {
    return this->lexerModuleRef;
  }

  public: void setErrorSyncBlockPairsRef(const SharedPtr<Reference> &ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef, ref);
    this->ownership |= GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF;
    this->metaChangeNotifier.emit(this, GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF);
  }
  private: void setErrorSyncBlockPairsRef(Reference *ref)
  {
    this->setErrorSyncBlockPairsRef(getSharedPtr(ref));
  }

  public: void resetErrorSyncBlockPairsRef()
  {
    RESET_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef);
    if (this->base != 0) this->errorSyncBlockPairsRef = this->base->getErrorSyncBlockPairsRef();
    this->ownership &= ~GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF;
    this->metaChangeNotifier.emit(this, GrammarModule::MetaElement::ERROR_SYNC_BLOCK_PAIRS_REF);
  }

  public: const SharedPtr<Reference>& getErrorSyncBlockPairsRef() const
  {
    return this->errorSyncBlockPairsRef;
  }

  public: void setStartRef(const SharedPtr<Reference> &sr)
  {
    UPDATE_OWNED_SHAREDPTR(this->startRef, sr);
    this->ownership |= GrammarModule::MetaElement::START_REF;
    this->metaChangeNotifier.emit(this, GrammarModule::MetaElement::START_REF);
  }
  private: void setStartRef(Reference *sr)
  {
    this->setStartRef(getSharedPtr(sr));
  }

  public: void resetStartRef()
  {
    RESET_OWNED_SHAREDPTR(this->startRef);
    if (this->base != 0) this->startRef = this->base->getStartRef();
    this->ownership &= ~GrammarModule::MetaElement::START_REF;
    this->metaChangeNotifier.emit(this, GrammarModule::MetaElement::START_REF);
  }

  public: const SharedPtr<Grammar::Reference>& getStartRef() const
  {
    return this->startRef;
  }

  /// @}

  /// @name Initializable Implementation
  /// @{

  public: virtual void initialize(TiObject *context);

  /// @}

  /// @name DataOwner Implementation
  /// @{

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

  /// @}

}; // class

} // namespace

#endif

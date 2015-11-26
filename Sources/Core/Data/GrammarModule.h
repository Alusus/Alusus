/**
 * @file Core/Data/GrammarModule.h
 * Contains the header of class Core::Data::GrammarModule.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_GRAMMARMODULE_H
#define DATA_GRAMMARMODULE_H

namespace Core { namespace Data
{

// TODO: DOC

class GrammarModule : public Module, public virtual Initializable
{
  //============================================================================
  // Type Info

  TYPE_INFO(GrammarModule, Module, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_1(Module, Initializable);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> parentReference;
  private: GrammarModule *parent;

  private: SharedPtr<Reference> startRef;

  private: SharedPtr<Reference> lexerModuleRef;

  private: SharedPtr<Reference> errorSyncBlockPairsRef;

  private: Word ownership;

  // TODO: Add the other dimensions.


  //============================================================================
  // Signals

  public: SIGNAL(destroyNotifier, (GrammarModule *obj), (obj));

  public: SIGNAL(metaChangeNotifier, (GrammarModule *obj, Word element), (obj, element));


  //============================================================================
  // Constructor & Destructor

  public: GrammarModule() : ownership(0), parent(0)
  {
  }

  public: GrammarModule(const std::initializer_list<Argument<Char const*>> &args);

  public: virtual ~GrammarModule();

  public: static SharedPtr<GrammarModule> create(const std::initializer_list<Argument<Char const*>> &args)
  {
    return std::make_shared<GrammarModule>(args);
  }


  //============================================================================
  // Member Functions

  /// @name General Functions
  /// @{

  public: void setLexerModuleRef(const SharedPtr<Reference> &lmr)
  {
    UPDATE_OWNED_SHAREDPTR(this->lexerModuleRef, lmr);
    this->ownership |= GrammarModuleMetaElement::LEXER_MODULE_REF;
    this->metaChangeNotifier.emit(this, GrammarModuleMetaElement::LEXER_MODULE_REF);
  }

  public: void resetLexerModuleRef()
  {
    RESET_OWNED_SHAREDPTR(this->lexerModuleRef);
    if (this->parent != 0) this->lexerModuleRef = this->parent->getLexerModuleRef();
    this->ownership &= ~GrammarModuleMetaElement::LEXER_MODULE_REF;
    this->metaChangeNotifier.emit(this, GrammarModuleMetaElement::LEXER_MODULE_REF);
  }

  public: const SharedPtr<Reference>& getLexerModuleRef() const
  {
    return this->lexerModuleRef;
  }

  public: void setErrorSyncBlockPairsRef(const SharedPtr<Reference> &ref)
  {
    UPDATE_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef, ref);
    this->ownership |= GrammarModuleMetaElement::ERROR_SYNC_BLOCK_PAIRS_REF;
    this->metaChangeNotifier.emit(this, GrammarModuleMetaElement::ERROR_SYNC_BLOCK_PAIRS_REF);
  }

  public: void resetErrorSyncBlockPairsRef()
  {
    RESET_OWNED_SHAREDPTR(this->errorSyncBlockPairsRef);
    if (this->parent != 0) this->errorSyncBlockPairsRef = this->parent->getErrorSyncBlockPairsRef();
    this->ownership &= ~GrammarModuleMetaElement::ERROR_SYNC_BLOCK_PAIRS_REF;
    this->metaChangeNotifier.emit(this, GrammarModuleMetaElement::ERROR_SYNC_BLOCK_PAIRS_REF);
  }

  public: const SharedPtr<Reference>& getErrorSyncBlockPairsRef() const
  {
    return this->errorSyncBlockPairsRef;
  }

  public: void setStartRef(const SharedPtr<Reference> &sr)
  {
    UPDATE_OWNED_SHAREDPTR(this->startRef, sr);
    this->ownership |= GrammarModuleMetaElement::START_REF;
    this->metaChangeNotifier.emit(this, GrammarModuleMetaElement::START_REF);
  }

  public: void resetStartRef()
  {
    RESET_OWNED_SHAREDPTR(this->startRef);
    if (this->parent != 0) this->startRef = this->parent->getStartRef();
    this->ownership &= ~GrammarModuleMetaElement::START_REF;
    this->metaChangeNotifier.emit(this, GrammarModuleMetaElement::START_REF);
  }

  public: const SharedPtr<Reference>& getStartRef() const
  {
    return this->startRef;
  }

  /// @}

  /// @name Inheritance Functions
  /// @{

  public: void setParentReference(const SharedPtr<Reference> &pr)
  {
    UPDATE_OWNED_SHAREDPTR(this->parentReference, pr);
  }

  public: const SharedPtr<Reference>& getParentReference() const
  {
    return this->parentReference;
  }

  public: void setParent(GrammarModule *p)
  {
    if (this->parent != 0) this->detachFromParent();
    if (p != 0) this->attachToParent(p);
  }

  public: GrammarModule* getParent() const
  {
    return this->parent;
  }

  private: virtual void attachToParent(GrammarModule *p);

  private: virtual void detachFromParent();

  private: void onParentDestroyed(GrammarModule *obj)
  {
    this->detachFromParent();
  }

  private: void onParentMetaChanged(GrammarModule *obj, Word element);

  /// @}


  //============================================================================
  // Initializable Implementation

  /// @name Initializable Implementation
  /// @{

  public: virtual void initialize(IdentifiableObject *owner);

  /// @}


  //============================================================================
  // DataOwner Implementation

  /// @name DataOwner Implementation
  /// @{

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

  /// @}

}; // class

} } // namespace

#endif

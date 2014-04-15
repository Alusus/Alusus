/**
 * @file Core/Data/GrammarModule.h
 * Contains the header of class Core::Data::GrammarModule.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_GRAMMAR_MODULE_H
#define DATA_GRAMMAR_MODULE_H

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

  public: GrammarModule(const std::initializer_list<Argument<const Char*>> &args);

  public: virtual ~GrammarModule()
  {
    if (this->parent != 0) this->detachFromParent();
    this->destroyNotifier.emit(this);
  }

  public: static SharedPtr<GrammarModule> create(const std::initializer_list<Argument<const Char*>> &args)
  {
    return std::make_shared<GrammarModule>(args);
  }


  //============================================================================
  // Member Functions

  /// @name General Functions
  /// @{

  public: void setLexerModuleRef(const SharedPtr<Reference> &lmr)
  {
    this->lexerModuleRef = lmr;
    this->metaChangeNotifier.emit(this, GrammarModuleMetaElement::LEXER_MODULE_REF);
  }
  public: const SharedPtr<Reference>& getLexerModuleRef() const
  {
    return this->lexerModuleRef;
  }

  public: void setStartRef(const SharedPtr<Reference> &sr)
  {
    this->startRef = sr;
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
    this->parentReference = pr;
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

  public: virtual void initialize(Manager *manager, const SharedPtr<Module> &module);

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

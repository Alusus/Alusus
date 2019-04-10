/**
 * @file Core/Data/Grammar/SymbolDefinition.h
 * Contains the header of class Core::Data::Grammar::SymbolDefinition.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_GRAMMAR_SYMBOLDEFINITION_H
#define CORE_DATA_GRAMMAR_SYMBOLDEFINITION_H

namespace Core::Data::Grammar
{

// TODO: DOC

class SymbolDefinition : public Node,
                         public virtual Binding, public virtual MapContaining<TiObject>,
                         public virtual Inheriting, public virtual IdHaving, public virtual DataHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(SymbolDefinition, Node, "Core.Data.Grammar", "Core", "alusus.net", (
    INHERITANCE_INTERFACES(Binding, MapContaining<TiObject>, Inheriting, IdHaving, DataHaving)
  ));


  //============================================================================
  // Types

  /**
   * @brief An enumeration for the different member elements in SymbolDefinition.
   * @ingroup core_data
   *
   * This enumeration is used to refer to an element within SymbolDefinition.
   * This is needed by SymbolDefinition::changeNotifier as well as the
   * constructors.
   */
  public: s_enum(Element,
    PARENT_REF=1, TERM=2, VAR_DEFS=4, VARS=8, HANDLER=16, PRIORITY=32, FLAGS=64, ATTRIBUTES=128, MODIFIER_TRANS=256,
    ALL=static_cast<Word>(-1)
  );

  /**
   * @brief An enumeration for change operations of SymbolDefinition.
   * @ingroup core_data
   *
   * This enumration is used to specify the type of the operation happening on
   * SymbolDefinition. This is used by SymbolDefinition::changeNotifier.
   *
   * UPDATE: An element is being updated.
   * DESTROY: The entire Symbol is being destroyed.
   */
  public: s_enum(ChangeOp, UPDATE, DESTROY);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> baseRef;

  private: SymbolDefinition *base = 0;

  private: TioSharedPtr priority;

  /**
     * @brief Flags for parsing and code generation features.
     *
     * These flags helps the parser or the parsing handler determine different
     * features associated with this production.
     */
  private: TioSharedPtr flags;

  private: SharedPtr<Node> term;

  private: SharedPtr<Node> varDefs;

  private: SharedPtr<Node> vars;

  private: SharedPtr<Node> attributes;

  private: SharedPtr<Map> modifierTranslations;

  private: SharedPtr<BuildHandler> handler;

  private: Word ownership = 0;

  // TODO: Add the list of prefixes and suffixes this definition can receive.


  //============================================================================
  // Implementations

  IMPLEMENT_IDHAVING(SymbolDefinition);

  IMPLEMENT_BINDING(Binding,
    (id, TiWord, VALUE, setId(value), &id),
    (baseRef, Reference, SHARED_REF, setBaseRef(value), baseRef.get()),
    (priority, TiObject, SHARED_REF, setPriority(value), priority.get()),
    (flags, TiObject, SHARED_REF, setFlags(value), flags.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (term, Node, SHARED_REF, setTerm(value), term.get()),
    (varDefs, Node, SHARED_REF, setVarDefs(value), varDefs.get()),
    (vars, Node, SHARED_REF, setVars(value), vars.get()),
    (attributes, Node, SHARED_REF, setAttributes(value), attributes.get()),
    (modifierTranslations, Map, SHARED_REF, setModifierTranslations(value), modifierTranslations.get()),
    (handler, BuildHandler, SHARED_REF, setBuildHandler(value), handler.get())
  );


  //============================================================================
  // Signals & Slots

  public: Signal<void, SymbolDefinition*, SymbolDefinition::ChangeOp, Word> changeNotifier;

  private: Slot<void, SymbolDefinition*, SymbolDefinition::ChangeOp, Word> parentContentChangeSlot = {
    this, &SymbolDefinition::onParentElementChanged
  };


  //============================================================================
  // Constructor & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(SymbolDefinition);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(SymbolDefinition);

  public: virtual ~SymbolDefinition();


  //============================================================================
  // Member Functions

  public: void setBaseRef(SharedPtr<Reference> const &p)
  {
    this->baseRef = p;
  }
  private: void setBaseRef(Reference *p)
  {
    this->setBaseRef(getSharedPtr(p));
  }

  public: SharedPtr<Reference> const& getBaseRef() const
  {
    return this->baseRef;
  }

  public: void setBase(SymbolDefinition *p)
  {
    if (this->base != 0) this->detachFromBase();
    if (p != 0) this->attachToBase(p);
  }

  public: SymbolDefinition* getBaseSymbolDefinition() const
  {
    return this->base;
  }

  private: void attachToBase(SymbolDefinition *p);

  private: void detachFromBase();

  private: void inheritFromParent();

  private: void removeInheritted();

  private: void onParentElementChanged(SymbolDefinition *obj, SymbolDefinition::ChangeOp op, Word elmt);

  public: void setPriority(TioSharedPtr const &p)
  {
    if (p != 0 && !p->isA<TiInt>() && !p->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("p"), S("Must be of type TiInt or Reference."));
    }
    this->priority = p;
    this->ownership |= SymbolDefinition::Element::PRIORITY;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::PRIORITY);
  }

  private: void setPriority(TiObject *p)
  {
    this->setPriority(getSharedPtr(p));
  }

  public: void resetPriority()
  {
    this->priority.reset();
    if (this->base != 0) this->priority = this->base->getPriority();
    this->ownership &= ~SymbolDefinition::Element::PRIORITY;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::PRIORITY);
  }

  public: TioSharedPtr const& getPriority() const
  {
    return this->priority;
  }

  /**
     * @brief Set the flags for parsing and code generation features.
     *
     * These flags helps the parser or the parsing handler determine different
     * features associated with this production.
     */
  public: void setFlags(TioSharedPtr const &f)
  {
    if (f != 0 && !f->isA<TiInt>() && !f->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("f"), S("Must be of type TiInt or Reference."));
    }
    this->flags = f;
    this->ownership |= SymbolDefinition::Element::FLAGS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::FLAGS);
  }

  private: void setFlags(TiObject *f)
  {
    this->setFlags(getSharedPtr(f));
  }

  public: void resetFlags()
  {
    this->flags.reset();
    if (this->base != 0) this->flags = this->base->getFlags();
    this->ownership &= ~SymbolDefinition::Element::FLAGS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::FLAGS);
  }

  /**
     * @brief Get the flags for parsing and code generation features.
     * @sa setFlags()
     */
  public: TioSharedPtr const& getFlags() const
  {
    return this->flags;
  }

  public: void setTerm(SharedPtr<Node> const &t)
  {
    if (!t->isDerivedFrom<Term>() && !t->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("t"), S("Must be of type Term or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->term, t);
    this->ownership |= SymbolDefinition::Element::TERM;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::TERM);
  }

  private: void setTerm(Node *t)
  {
    this->setTerm(getSharedPtr(t));
  }

  public: void resetTerm()
  {
    RESET_OWNED_SHAREDPTR(this->term);
    if (this->base != 0) this->term = this->base->getTerm();
    this->ownership &= ~SymbolDefinition::Element::TERM;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::TERM);
  }

  public: SharedPtr<Node> const& getTerm() const
  {
    return this->term;
  }

  public: void setVarDefs(SharedPtr<Node> const &vd)
  {
    if (vd != 0 && !vd->isA<Map>() && !vd->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("vd"), S("Must be of type Map or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->varDefs, vd);
    this->ownership |= SymbolDefinition::Element::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::VAR_DEFS);
  }

  private: void setVarDefs(Node *vd)
  {
    this->setVarDefs(getSharedPtr(vd));
  }

  public: void resetVarDefs()
  {
    RESET_OWNED_SHAREDPTR(this->varDefs);
    if (this->base != 0) this->varDefs = this->base->getVarDefs();
    this->ownership &= ~SymbolDefinition::Element::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::VAR_DEFS);
  }

  public: SharedPtr<Node> const& getVarDefs() const
  {
    return this->varDefs;
  }

  public: void setVars(SharedPtr<Node> const &v)
  {
    if (v != 0 && !v->isA<Map>() && !v->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, S("v"), S("Must be of type Map or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->vars, v);
    this->ownership |= SymbolDefinition::Element::VARS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::VARS);
  }

  private: void setVars(Node *v)
  {
    this->setVars(getSharedPtr(v));
  }

  public: void resetVars()
  {
    RESET_OWNED_SHAREDPTR(this->vars);
    if (this->base != 0) this->vars = this->base->getVars();
    this->ownership &= ~SymbolDefinition::Element::VARS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::VARS);
  }

  public: SharedPtr<Node> const& getVars() const
  {
    return this->vars;
  }

  public: void setAttributes(SharedPtr<Node> const &a)
  {
    UPDATE_OWNED_SHAREDPTR(this->attributes, a);
    this->ownership |= SymbolDefinition::Element::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::ATTRIBUTES);
  }

  private: void setAttributes(Node *a)
  {
    this->setAttributes(getSharedPtr(a));
  }

  public: void resetAttributes()
  {
    RESET_OWNED_SHAREDPTR(this->attributes);
    if (this->base != 0) this->attributes = this->base->getAttributes();
    this->ownership &= ~SymbolDefinition::Element::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::ATTRIBUTES);
  }

  public: SharedPtr<Node> const& getAttributes() const
  {
    return this->attributes;
  }

  public: void setModifierTranslations(SharedPtr<Map> const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->modifierTranslations, t);
    this->ownership |= SymbolDefinition::Element::MODIFIER_TRANS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::MODIFIER_TRANS);
  }

  private: void setModifierTranslations(Map *t)
  {
    this->setModifierTranslations(getSharedPtr(t));
  }

  public: void resetModifierTranslations()
  {
    RESET_OWNED_SHAREDPTR(this->modifierTranslations);
    if (this->base != 0) this->modifierTranslations = this->base->getModifierTranslations();
    this->ownership &= ~SymbolDefinition::Element::MODIFIER_TRANS;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::MODIFIER_TRANS);
  }

  public: SharedPtr<Map> const& getModifierTranslations() const
  {
    return this->modifierTranslations;
  }

  public: SbStr const& getTranslatedModifierKeyword(Char const *keyword) const;

  /**
     * @brief Set the operation handler object.
     *
     * The operation handler is the object that receives the parsing events and
     * generates the required output results.
     * Once set, modifying this value is not allowed. This is to prevent
     * accidentally modifying the handler while being in use by some state.
     */
  public: void setBuildHandler(SharedPtr<BuildHandler> const &h)
  {
    UPDATE_OWNED_SHAREDPTR(this->handler, h);
    this->ownership |= SymbolDefinition::Element::HANDLER;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::HANDLER);
  }

  private: void setBuildHandler(BuildHandler *h)
  {
    this->setBuildHandler(getSharedPtr(h));
  }

  public: void resetBuildHandler()
  {
    RESET_OWNED_SHAREDPTR(this->handler);
    if (this->base != 0) this->handler = this->base->getBuildHandler();
    this->ownership &= ~SymbolDefinition::Element::HANDLER;
    this->changeNotifier.emit(this, SymbolDefinition::ChangeOp::UPDATE, SymbolDefinition::Element::HANDLER);
  }

  /**
     * @brief Get the operation handler object.
     *
     * The operation handler is the object that receives the parsing events and
     * generates the required output results.
     */
  public: SharedPtr<BuildHandler> const& getBuildHandler() const
  {
    return this->handler;
  }


  //============================================================================
  // Inheriting Implementation

  public: virtual Reference* getBaseReference() const
  {
    return this->baseRef.get();
  }

  public: virtual void setBase(TiObject *base)
  {
    SymbolDefinition *baseDef = ti_cast<SymbolDefinition>(base);
    if (baseDef == 0) {
      throw EXCEPTION(GenericException, S("Base reference points to an object of an invalid type."));
    }
    this->setBase(baseDef);
  }

  public: virtual TiObject* getBase() const
  {
    return this->getBaseSymbolDefinition();
  }


  //============================================================================
  // DataHaving Implementation

  /// @sa DataHaving::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} // namespace

#endif

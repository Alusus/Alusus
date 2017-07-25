/**
 * @file Core/Data/SymbolDefinition.h
 * Contains the header of class Core::Data::SymbolDefinition.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_SYMBOLDEFINITION_H
#define CORE_DATA_SYMBOLDEFINITION_H

namespace Core { namespace Data
{

// TODO: DOC

class SymbolDefinition : public Node,
                         public virtual Bindings, public virtual Initializable,
                         public virtual IdHolder, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(SymbolDefinition, Node, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, Bindings, Initializable, IdHolder, DataOwner);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> parentReference;

  private: SymbolDefinition *parent;

  private: SharedPtr<Node> term;

  private: SharedPtr<Node> varDefs;

  private: SharedPtr<Node> vars;

  private: SharedPtr<BuildHandler> handler;

  private: SharedPtr<Node> priority;

  /**
     * @brief Flags for parsing and code generation features.
     *
     * These flags helps the parser or the parsing handler determine different
     * features associated with this production.
     */
  private: SharedPtr<Node> flags;

  private: SharedPtr<Node> attributes;

  private: Word ownership;

  // TODO: Add the list of prefixes and suffixes this definition can receive.


  //============================================================================
  // Implementations

  IMPLEMENT_IDHOLDER(SymbolDefinition);

  IMPLEMENT_BINDINGS(Bindings, (id, TiWord, VALUE, setId(value), &id));


  //============================================================================
  // Signals & Slots

  public: Signal<void, SymbolDefinition*, SymbolDefChangeOp, Word> changeNotifier;

  private: Slot<void, SymbolDefinition*, SymbolDefChangeOp, Word> parentContentChangeSlot = {
    this, &SymbolDefinition::onParentElementChanged
  };


  //============================================================================
  // Constructor & Destructor

  public: SymbolDefinition() : priority(0), flags(0), ownership(0), parent(0)
  {
  }

  public: SymbolDefinition(const SharedPtr<Reference> &pnt,
                           SharedPtr<Node> const &t,
                           SharedPtr<Node> const &vd,
                           SharedPtr<Node> const &v,
                           SharedPtr<BuildHandler> const &h,
                           SharedPtr<Node> const &p,
                           SharedPtr<Node> const &f,
                           SharedPtr<Node> const &a=SharedPtr<Node>());

  public: SymbolDefinition(const std::initializer_list<Argument<SymbolDefElement>> &args);

  public: virtual ~SymbolDefinition();

  public: static SharedPtr<SymbolDefinition> create(const std::initializer_list<Argument<SymbolDefElement>> &args)
  {
    return std::make_shared<SymbolDefinition>(args);
  }


  //============================================================================
  // Member Functions

  public: void setParentReference(SharedPtr<Reference> const &p)
  {
    UPDATE_OWNED_SHAREDPTR(this->parentReference, p);
  }

  public: SharedPtr<Reference> const& getParentReference() const
  {
    return this->parentReference;
  }

  public: void setParent(SymbolDefinition *p)
  {
    if (this->parent != 0) this->detachFromParent();
    if (p != 0) this->attachToParent(p);
  }

  public: SymbolDefinition* getParent() const
  {
    return this->parent;
  }

  private: void attachToParent(SymbolDefinition *p);

  private: void detachFromParent();

  private: void inheritFromParent();

  private: void removeInheritted();

  private: void onParentElementChanged(SymbolDefinition *obj, SymbolDefChangeOp op, Word elmt);

  public: void setTerm(SharedPtr<Node> const &t)
  {
    if (!t->isDerivedFrom<Term>() && !t->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("t"), STR("Must be of type Term or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->term, t);
    this->ownership |= SymbolDefElement::TERM;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::TERM);
  }

  public: void resetTerm()
  {
    RESET_OWNED_SHAREDPTR(this->term);
    if (this->parent != 0) this->term = this->parent->getTerm();
    this->ownership &= ~SymbolDefElement::TERM;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::TERM);
  }

  public: SharedPtr<Node> const& getTerm() const
  {
    return this->term;
  }

  public: void setVarDefs(SharedPtr<Node> const &vd)
  {
    if (vd != 0 && !vd->isA<SharedMap>() && !vd->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("vd"), STR("Must be of type SharedMap or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->varDefs, vd);
    this->ownership |= SymbolDefElement::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VAR_DEFS);
  }

  public: void resetVarDefs()
  {
    RESET_OWNED_SHAREDPTR(this->varDefs);
    if (this->parent != 0) this->varDefs = this->parent->getVarDefs();
    this->ownership &= ~SymbolDefElement::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VAR_DEFS);
  }

  public: SharedPtr<Node> const& getVarDefs() const
  {
    return this->varDefs;
  }

  public: void setVars(SharedPtr<Node> const &v)
  {
    if (v != 0 && !v->isA<SharedMap>() && !v->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("v"), STR("Must be of type SharedMap or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->vars, v);
    this->ownership |= SymbolDefElement::VARS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VARS);
  }

  public: void resetVars()
  {
    RESET_OWNED_SHAREDPTR(this->vars);
    if (this->parent != 0) this->vars = this->parent->getVars();
    this->ownership &= ~SymbolDefElement::VARS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VARS);
  }

  public: SharedPtr<Node> const& getVars() const
  {
    return this->vars;
  }

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
    this->ownership |= SymbolDefElement::HANDLER;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::HANDLER);
  }

  public: void resetBuildHandler()
  {
    RESET_OWNED_SHAREDPTR(this->handler);
    if (this->parent != 0) this->handler = this->parent->getBuildHandler();
    this->ownership &= ~SymbolDefElement::HANDLER;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::HANDLER);
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

  public: void setPriority(SharedPtr<Node> const &p)
  {
    if (p != 0 && !p->isA<Integer>() && !p->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("p"), STR("Must be of type Integer or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->priority, p);
    this->ownership |= SymbolDefElement::PRIORITY;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::PRIORITY);
  }

  public: void resetPriority()
  {
    RESET_OWNED_SHAREDPTR(this->priority);
    if (this->parent != 0) this->priority = this->parent->getPriority();
    this->ownership &= ~SymbolDefElement::PRIORITY;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::PRIORITY);
  }

  public: SharedPtr<Node> const& getPriority() const
  {
    return this->priority;
  }

  /**
     * @brief Set the flags for parsing and code generation features.
     *
     * These flags helps the parser or the parsing handler determine different
     * features associated with this production.
     */
  public: void setFlags(SharedPtr<Node> const &f)
  {
    if (f != 0 && !f->isA<Integer>() && !f->isDerivedFrom<Reference>()) {
      throw EXCEPTION(InvalidArgumentException, STR("f"), STR("Must be of type Integer or Reference."));
    }
    UPDATE_OWNED_SHAREDPTR(this->flags, f);
    this->ownership |= SymbolDefElement::FLAGS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::FLAGS);
  }

  public: void resetFlags()
  {
    RESET_OWNED_SHAREDPTR(this->flags);
    if (this->parent != 0) this->flags = this->parent->getFlags();
    this->ownership &= ~SymbolDefElement::FLAGS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::FLAGS);
  }

  /**
     * @brief Get the flags for parsing and code generation features.
     * @sa setFlags()
     */
  public: SharedPtr<Node> const& getFlags() const
  {
    return this->flags;
  }

  public: void setAttributes(SharedPtr<Node> const &a)
  {
    UPDATE_OWNED_SHAREDPTR(this->attributes, a);
    this->ownership |= SymbolDefElement::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::ATTRIBUTES);
  }

  public: void resetAttributes()
  {
    RESET_OWNED_SHAREDPTR(this->attributes);
    if (this->parent != 0) this->attributes = this->parent->getAttributes();
    this->ownership &= ~SymbolDefElement::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::ATTRIBUTES);
  }

  public: SharedPtr<Node> const& getAttributes() const
  {
    return this->attributes;
  }


  //============================================================================
  // Initializable Implementation

  public: virtual void initialize(TiObject *owner);


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} } // namespace

#endif

/**
 * @file Core/Data/SymbolDefinition.h
 * Contains the header of class Core::Data::SymbolDefinition.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_SYMBOL_DEFINITION_H
#define DATA_SYMBOL_DEFINITION_H

namespace Core { namespace Data
{

// TODO: DOC

class SymbolDefinition : public IdentifiableObject,
                         public virtual Initializable, public virtual IdOwner, public virtual DataOwner
{
  //============================================================================
  // Type Info

  TYPE_INFO(SymbolDefinition, IdentifiableObject, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_3(IdentifiableObject, Initializable, IdOwner, DataOwner);


  //============================================================================
  // Member Variables

  private: SharedPtr<Reference> parentReference;

  private: WeakPtr<SymbolDefinition> parent;

  private: SymbolDefinition *plainParent;

  private: SharedPtr<IdentifiableObject> term;

  private: SharedPtr<IdentifiableObject> varDefs;

  private: SharedPtr<IdentifiableObject> vars;

  private: SharedPtr<OperationHandler> handler;

  private: Int priority;

  /**
     * @brief Flags for parsing and code generation features.
     *
     * These flags helps the parser or the parsing handler determine different
     * features associated with this production.
     */
  private: Word flags;

  private: SharedPtr<IdentifiableObject> attributes;

  private: Word ownership;

  // TODO: Add the list of prefixes and suffixes this definition can receive.


  //============================================================================
  // Signals

  public: SIGNAL(changeNotifier, (SymbolDefinition *obj, SymbolDefChangeOp op, Word elmt), (obj, op, elmt));


  //============================================================================
  // Constructor & Destructor

  public: SymbolDefinition() : priority(0), flags(0), ownership(0), plainParent(0)
  {
  }

  public: SymbolDefinition(const SharedPtr<Reference> &pnt,
                           SharedPtr<IdentifiableObject> const &t,
                           SharedPtr<IdentifiableObject> const &vd,
                           SharedPtr<IdentifiableObject> const &v,
                           const SharedPtr<OperationHandler> &h,
                           Int p, Word f,
                           SharedPtr<IdentifiableObject> const &a=SharedPtr<IdentifiableObject>()) :
    parentReference(pnt), term(t), varDefs(vd), vars(v), handler(h), priority(p),
    flags(f), attributes(a), ownership(SymbolDefElement::ALL)
  {
    if (t == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Data::SymbolDefinition::setTerm"),
                                     STR("Should not be null."));
    }
    if (!t->isDerivedFrom<Term>() && !t->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("t"), STR("Core::Data::SymbolDefinition::Symbol_Definision"),
                                     STR("Must be of type Term or Reference."));
    }
    if (vd != 0 && !vd->isA<SharedMap>() && !vd->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("vd"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type SharedMap or Reference."));
    }
    if (v != 0 && !v->isA<SharedMap>() && !v->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("v"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type SharedMap or Reference."));
    }
  }

  public: SymbolDefinition(const std::initializer_list<Argument<SymbolDefElement>> &args);

  public: virtual ~SymbolDefinition()
  {
    if (this->plainParent != 0) this->detachFromParent();
    this->changeNotifier.emit(this, SymbolDefChangeOp::DESTROY, 0);
  }

  public: static SharedPtr<SymbolDefinition> create(const std::initializer_list<Argument<SymbolDefElement>> &args)
  {
    return std::make_shared<SymbolDefinition>(args);
  }


  //============================================================================
  // Member Functions

  public: void setParentReference(SharedPtr<Reference> const &p)
  {
    this->parentReference = p;
  }

  public: SharedPtr<Reference> const& getParentReference() const
  {
    return this->parentReference;
  }

  public: void setParent(SharedPtr<SymbolDefinition> const &p)
  {
    this->setParent(p.get());
    this->parent = p;
  }

  public: void setParent(SymbolDefinition *p)
  {
    if (this->plainParent != 0) this->detachFromParent();
    if (p != 0) this->attachToParent(p);
  }

  public: SharedPtr<SymbolDefinition> getParent() const
  {
    return this->parent.lock();
  }

  public: SymbolDefinition* getPlainParent() const
  {
    return this->plainParent;
  }

  private: void attachToParent(SymbolDefinition *p);

  private: void detachFromParent();

  private: void inheritFromParent();

  private: void removeInheritted();

  private: void onParentElementChanged(SymbolDefinition *obj, SymbolDefChangeOp op, Word elmt);

  public: void setTerm(SharedPtr<IdentifiableObject> const &t)
  {
    if (!t->isDerivedFrom<Term>() && !t->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("t"), STR("Core::Data::SymbolDefinition::setTerm"),
                                     STR("Must be of type Term or Reference."));
    }
    this->term = t;
    this->ownership |= SymbolDefElement::TERM;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::TERM);
  }

  public: void resetTerm()
  {
    if (this->plainParent != 0) this->term = this->plainParent->getTerm();
    else this->term.reset();
    this->ownership &= ~SymbolDefElement::TERM;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::TERM);
  }

  public: SharedPtr<IdentifiableObject> const& getTerm() const
  {
    return this->term;
  }

  public: void setVarDefs(SharedPtr<IdentifiableObject> const &vd)
  {
    if (vd != 0 && !vd->isA<SharedMap>() && !vd->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("vd"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type SharedMap or Reference."));
    }
    this->varDefs = vd;
    this->ownership |= SymbolDefElement::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VAR_DEFS);
  }

  public: void resetVarDefs()
  {
    if (this->plainParent != 0) this->varDefs = this->plainParent->getVarDefs();
    else this->varDefs.reset();
    this->ownership &= ~SymbolDefElement::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VAR_DEFS);
  }

  public: SharedPtr<IdentifiableObject> const& getVarDefs() const
  {
    return this->varDefs;
  }

  public: void setVars(SharedPtr<IdentifiableObject> const &v)
  {
    if (v != 0 && !v->isA<SharedMap>() && !v->isDerivedFrom<Reference>()) {
      throw InvalidArgumentException(STR("v"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type SharedMap or Reference."));
    }
    this->vars = v;
    this->ownership |= SymbolDefElement::VARS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VARS);
  }

  public: void resetVars()
  {
    if (this->plainParent != 0) this->vars = this->plainParent->getVars();
    else this->vars.reset();
    this->ownership &= ~SymbolDefElement::VARS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VARS);
  }

  public: SharedPtr<IdentifiableObject> const& getVars() const
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
  public: void setOperationHandler(const SharedPtr<OperationHandler> &h)
  {
    this->handler = h;
    this->ownership |= SymbolDefElement::HANDLER;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::HANDLER);
  }

  public: void resetOperationHandler()
  {
    if (this->plainParent != 0) this->handler = this->plainParent->getOperationHandler();
    else this->handler.reset();
    this->ownership &= ~SymbolDefElement::HANDLER;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::HANDLER);
  }

  /**
     * @brief Get the operation handler object.
     *
     * The operation handler is the object that receives the parsing events and
     * generates the required output results.
     */
  public: const SharedPtr<OperationHandler>& getOperationHandler() const
  {
    return this->handler;
  }

  public: void setPriority(Int p)
  {
    this->priority = p;
    this->ownership |= SymbolDefElement::PRIORITY;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::PRIORITY);
  }

  public: void resetPriority()
  {
    if (this->plainParent != 0) this->priority = this->plainParent->getPriority();
    else this->priority = 0;
    this->ownership &= ~SymbolDefElement::PRIORITY;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::PRIORITY);
  }

  public: Int getPriority() const
  {
    return this->priority;
  }

  /**
     * @brief Set the flags for parsing and code generation features.
     *
     * These flags helps the parser or the parsing handler determine different
     * features associated with this production.
     */
  public: void setFlags(Word f)
  {
    this->flags = f;
    this->ownership |= SymbolDefElement::FLAGS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::FLAGS);
  }

  public: void resetFlags()
  {
    if (this->plainParent != 0) this->flags = this->plainParent->getFlags();
    else this->flags = 0;
    this->ownership &= ~SymbolDefElement::FLAGS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::FLAGS);
  }

  /**
     * @brief Get the flags for parsing and code generation features.
     * @sa setFlags()
     */
  public: Word getFlags() const
  {
    return this->flags;
  }

  public: void setAttributes(SharedPtr<IdentifiableObject> const &a)
  {
    this->attributes = a;
    this->ownership |= SymbolDefElement::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::ATTRIBUTES);
  }

  public: void resetAttributes()
  {
    if (this->plainParent != 0) this->attributes = this->plainParent->getAttributes();
    else this->attributes.reset();
    this->ownership &= ~SymbolDefElement::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::ATTRIBUTES);
  }

  public: SharedPtr<IdentifiableObject> const& getAttributes() const
  {
    return this->attributes;
  }


  //============================================================================
  // Initializable Implementation

  public: virtual void initialize(IdentifiableObject *owner);


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} } // namespace

#endif

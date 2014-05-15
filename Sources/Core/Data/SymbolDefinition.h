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

  private: SymbolDefinition *parent;

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

  public: SymbolDefinition() : priority(0), flags(0), ownership(0), parent(0)
  {
  }

  public: SymbolDefinition(const SharedPtr<Reference> &pnt,
                           const SharedPtr<IdentifiableObject> &t,
                           const SharedPtr<IdentifiableObject> &vd,
                           const SharedPtr<IdentifiableObject> &v,
                           const SharedPtr<OperationHandler> &h,
                           Int p, Word f,
                           const SharedPtr<IdentifiableObject> &a=SharedPtr<IdentifiableObject>()) :
    parentReference(pnt), term(t), varDefs(vd), vars(v), handler(h), priority(p),
    flags(f), attributes(a), ownership(SymbolDefElement::ALL)
  {
    if (t == 0) {
      throw InvalidArgumentException(STR("t"), STR("Core::Data::SymbolDefinition::setTerm"),
                                     STR("Should not be null."));
    }
    if (!t->isDerivedFrom<Term>() && !t->isA<Reference>()) {
      throw InvalidArgumentException(STR("t"), STR("Core::Data::SymbolDefinition::Symbol_Definision"),
                                     STR("Must be of type Term or Reference."));
    }
    if (vd != 0 && !vd->isA<Map>() && !vd->isA<Reference>()) {
      throw InvalidArgumentException(STR("vd"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type Map or Reference."));
    }
    if (v != 0 && !v->isA<Map>() && !v->isA<Reference>()) {
      throw InvalidArgumentException(STR("v"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type Map or Reference."));
    }
  }

  public: SymbolDefinition(const std::initializer_list<Argument<SymbolDefElement>> &args);

  public: virtual ~SymbolDefinition()
  {
    if (this->parent != 0) this->detachFromParent();
    this->changeNotifier.emit(this, SymbolDefChangeOp::DESTROY, 0);
  }

  public: static SharedPtr<SymbolDefinition> create(const std::initializer_list<Argument<SymbolDefElement>> &args)
  {
    return std::make_shared<SymbolDefinition>(args);
  }


  //============================================================================
  // Member Functions

  public: void setParentReference(const SharedPtr<Reference> &p)
  {
    this->parentReference = p;
  }

  public: const SharedPtr<Reference>& getParentReference() const
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

  public: void setTerm(const SharedPtr<IdentifiableObject> &t)
  {
    if (!t->isDerivedFrom<Term>() && !t->isA<Reference>()) {
      throw InvalidArgumentException(STR("t"), STR("Core::Data::SymbolDefinition::setTerm"),
                                     STR("Must be of type Term or Reference."));
    }
    this->term = t;
    this->ownership |= SymbolDefElement::TERM;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::TERM);
  }

  public: void resetTerm()
  {
    if (this->parent != 0) this->term = parent->getTerm();
    else this->term.reset();
    this->ownership &= ~SymbolDefElement::TERM;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::TERM);
  }

  public: const SharedPtr<IdentifiableObject>& getTerm() const
  {
    return this->term;
  }

  public: void setVarDefs(const SharedPtr<IdentifiableObject> &vd)
  {
    if (vd != 0 && !vd->isA<Map>() && !vd->isA<Reference>()) {
      throw InvalidArgumentException(STR("vd"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type Map or Reference."));
    }
    this->varDefs = vd;
    this->ownership |= SymbolDefElement::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VAR_DEFS);
  }

  public: void resetVarDefs()
  {
    if (this->parent != 0) this->varDefs = parent->getVarDefs();
    else this->varDefs.reset();
    this->ownership &= ~SymbolDefElement::VAR_DEFS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VAR_DEFS);
  }

  public: const SharedPtr<IdentifiableObject>& getVarDefs() const
  {
    return this->varDefs;
  }

  public: void setVars(const SharedPtr<IdentifiableObject> &v)
  {
    if (v != 0 && !v->isA<Map>() && !v->isA<Reference>()) {
      throw InvalidArgumentException(STR("v"), STR("Core::Data::SymbolDefinition::SymbolDefinition"),
                                     STR("Must be of type Map or Reference."));
    }
    this->vars = v;
    this->ownership |= SymbolDefElement::VARS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VARS);
  }

  public: void resetVars()
  {
    if (this->parent != 0) this->vars = parent->getVars();
    else this->vars.reset();
    this->ownership &= ~SymbolDefElement::VARS;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::VARS);
  }

  public: const SharedPtr<IdentifiableObject>& getVars() const
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
    if (this->parent != 0) this->handler = parent->getOperationHandler();
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
    if (this->parent != 0) this->priority = parent->getPriority();
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
    if (this->parent != 0) this->flags = parent->getFlags();
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

  public: void setAttributes(const SharedPtr<IdentifiableObject> &a)
  {
    this->attributes = a;
    this->ownership |= SymbolDefElement::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::ATTRIBUTES);
  }

  public: void resetAttributes()
  {
    if (this->parent != 0) this->attributes = parent->getAttributes();
    else this->attributes.reset();
    this->ownership &= ~SymbolDefElement::ATTRIBUTES;
    this->changeNotifier.emit(this, SymbolDefChangeOp::UPDATE, SymbolDefElement::ATTRIBUTES);
  }

  public: const SharedPtr<IdentifiableObject>& getAttributes() const
  {
    return this->attributes;
  }


  //============================================================================
  // Initializable Implementation

  public: virtual void initialize(Provider *provider, const SharedPtr<Module> &module);


  //============================================================================
  // DataOwner Implementation

  /// @sa DataOwner::unsetIndexes()
  public: virtual void unsetIndexes(Int from, Int to);

}; // class

} } // namespace

#endif

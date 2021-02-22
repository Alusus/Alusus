/**
 * @file Core/Data/Ast/Definition.h
 * Contains the header of class Core::Data::Ast::Definition.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_DEFINITION_H
#define CORE_DATA_AST_DEFINITION_H

namespace Core::Data::Ast
{

// TODO: DOC

class Definition : public Node,
                   public Binding, public MapContaining<TiObject>, public MetaHaving, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Definition, Node, "Core.Data.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<TiObject>, MetaHaving, Printable);
  OBJECT_FACTORY(Definition);


  //============================================================================
  // Member Variables

  private: TiStr name;
  private: TioSharedPtr target;
  private: TiBool toMerge;
  private: SharedPtr<List> modifiers;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Definition);

  IMPLEMENT_BINDING(Binding,
    (name, TiStr, VALUE, setName(value), &name),
    (toMerge, TiBool, VALUE, setToMerge(value), &toMerge),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (target, TiObject, SHARED_REF, setTarget(value), target.get()),
    (modifiers, List, SHARED_REF, setModifiers(value), modifiers.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Definition);

  IMPLEMENT_ATTR_CONSTRUCTOR(Definition);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Definition);

  public: virtual ~Definition()
  {
    DISOWN_SHAREDPTR(this->target);
  }


  //============================================================================
  // Member Functions

  public: void setName(Char const *n)
  {
    this->name = n;
  }
  public: void setName(TiStr const *n)
  {
    this->name = n == 0 ? "" : n->get();
  }

  public: TiStr const& getName() const
  {
    return this->name;
  }

  public: void setTarget(TioSharedPtr const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->target, t);
  }
  private: void setTarget(TiObject *t)
  {
    this->setTarget(getSharedPtr(t));
  }

  public: TioSharedPtr const& getTarget() const
  {
    return this->target;
  }

  public: void setToMerge(Bool tm)
  {
    this->toMerge = tm;
  }
  public: void setToMerge(TiBool const *tm)
  {
    this->toMerge = tm == 0 ? false : tm->get();
  }

  public: Bool isToMerge() const
  {
    return this->toMerge.get();
  }

  public: void setModifiers(SharedPtr<List> const &m)
  {
    UPDATE_OWNED_SHAREDPTR(this->modifiers, m);
  }
  private: void setModifiers(List *m)
  {
    this->setModifiers(getSharedPtr(m));
  }

  public: void addModifier(TioSharedPtr const &modifier)
  {
    if (this->modifiers == 0) {
      this->modifiers = List::create({}, { modifier });
    } else {
      this->modifiers->add(modifier);
    }
  }

  public: SharedPtr<List> const& getModifiers() const
  {
    return this->modifiers;
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} // namespace

#endif

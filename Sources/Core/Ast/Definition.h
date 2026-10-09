/**
 * @file Core/Ast/Definition.h
 * Contains the header of class Core::Ast::Definition.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_DEFINITION_H
#define CORE_AST_DEFINITION_H

namespace Core::Ast
{

// TODO: DOC

class Definition : public Node, public MapContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Definition, Node, "Core.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, MapContaining<Node>);
  OBJECT_FACTORY(Definition);


  //============================================================================
  // Member Variables

  private: TiStr name;
  private: SharedPtr<Node> target;
  private: TiBool toMerge;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Node,
    (name, TiStr, VALUE, setName(value), &name),
    (toMerge, TiBool, VALUE, setToMerge(value), &toMerge)
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Node>,
    (target, Node, SHARED_REF, setTarget(value), target.get())
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

  public: void setTarget(SharedPtr<Node> const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->target, t);
  }
  private: void setTarget(Node *t)
  {
    this->setTarget(getSharedPtr(t));
  }

  public: SharedPtr<Node> const& getTarget() const
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


  //============================================================================
  // Printing Functions

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} // namespace

#endif

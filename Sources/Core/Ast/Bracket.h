/**
 * @file Core/Ast/Bracket.h
 * Contains the header of class Core::Ast::Bracket.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_BRACKET_H
#define CORE_AST_BRACKET_H

namespace Core::Ast
{

// TODO: DOC

class Bracket : public Node,
                public Binding, public MapContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(Bracket, Node, "Core.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<Node>);
  OBJECT_FACTORY(Bracket);


  //============================================================================
  // Member Variables

  private: BracketType type;
  private: SharedPtr<Node> operand;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (type, BracketType, VALUE, setType(value), &type),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Node>,
    (operand, Node, SHARED_REF, setOperand(value), operand.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(Bracket, << (this->type == BracketType::ROUND ? S("()") : S("[]")));


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Bracket);

  IMPLEMENT_ATTR_CONSTRUCTOR(Bracket);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Bracket);

  public: virtual ~Bracket()
  {
    DISOWN_SHAREDPTR(this->operand);
  }


  //============================================================================
  // Member Functions

  public: void setType(BracketType const &t)
  {
    this->type = t;
  }
  public: void setType(BracketType const *t)
  {
    this->type = t == 0 ? BracketType::ROUND : t->get();
  }

  public: BracketType const& getType() const
  {
    return this->type;
  }

  public: void setOperand(SharedPtr<Node> const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }
  private: void setOperand(Node *o)
  {
    this->setOperand(getSharedPtr(o));
  }

  public: SharedPtr<Node> const& getOperand() const
  {
    return this->operand;
  }

}; // class

} // namespace

#endif

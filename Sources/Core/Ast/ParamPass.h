/**
 * @file Core/Ast/ParamPass.h
 * Contains the header of class Core::Ast::ParamPass.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_PARAMPASS_H
#define CORE_AST_PARAMPASS_H

namespace Core::Ast
{

// TODO: DOC

class ParamPass : public Node,
                  public Binding, public MapContaining<Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParamPass, Node, "Core.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<Node>);
  OBJECT_FACTORY(ParamPass);


  //============================================================================
  // Member Variables

  private: BracketType type;
  private: SharedPtr<Node> operand;
  private: SharedPtr<Node> param;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (type, BracketType, VALUE, setType(value), &type),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Node>,
    (operand, Node, SHARED_REF, setOperand(value), operand.get()),
    (param, Node, SHARED_REF, setParam(value), param.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(ParamPass, << (this->type == BracketType::ROUND ? S("()") : S("[]")));


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ParamPass);

  IMPLEMENT_ATTR_CONSTRUCTOR(ParamPass);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ParamPass);

  public: virtual ~ParamPass()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->param);
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

  public: void setParam(SharedPtr<Node> const &p)
  {
    UPDATE_OWNED_SHAREDPTR(this->param, p);
  }
  private: void setParam(Node *p)
  {
    this->setParam(getSharedPtr(p));
  }

  public: SharedPtr<Node> const& getParam() const
  {
    return this->param;
  }

}; // class

} // namespace

#endif

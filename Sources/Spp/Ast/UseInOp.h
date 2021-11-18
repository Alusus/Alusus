/**
 * @file Spp/Ast/UseInOp.h
 * Contains the header of class Spp::Ast::UseInOp.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_USEINOP_H
#define SPP_AST_USEINOP_H

namespace Spp::Ast
{

class UseInOp : public Core::Data::Node,
                public Binding, public MapContaining<TiObject>,
                public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(UseInOp, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(UseInOp);


  //============================================================================
  // Member Variables

  private: TiStr operandName;
  private: TioSharedPtr operand;
  private: SharedPtr<Spp::Ast::Block> body;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(UseInOp);

  IMPLEMENT_BINDING(Binding,
    (operandName, TiStr, VALUE, setOperandName(value), &operandName),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (operand, TiObject, SHARED_REF, setOperand(value), operand.get()),
    (body, Spp::Ast::Block, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(UseInOp, << this->operandName.get());


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(UseInOp);

  IMPLEMENT_ATTR_CONSTRUCTOR(UseInOp);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(UseInOp);

  public: virtual ~UseInOp()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setOperandName(Char const *n)
  {
    this->operandName.set(n);
  }
  public: void setOperandName(TiStr const *n)
  {
    this->setOperandName(n == 0 ? S("") : n->get());
  }

  public: TiStr const& getOperandName() const
  {
    return this->operandName;
  }

  public: void setOperand(TioSharedPtr const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }
  private: void setOperand(TiObject *o)
  {
    this->setOperand(getSharedPtr(o));
  }

  public: TioSharedPtr const& getOperand() const
  {
    return this->operand;
  }

  public: void setBody(SharedPtr<Spp::Ast::Block> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Spp::Ast::Block *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Spp::Ast::Block> const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif

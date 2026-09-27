/**
 * @file Spp/Ast/TypeOp.h
 * Contains the header of class Spp::Ast::TypeOp.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_TYPEOP_H
#define SPP_AST_TYPEOP_H

namespace Spp::Ast
{

class TypeOp : public Core::Ast::Node,
               public Binding, public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(TypeOp, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(TypeOp);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> operand;
  private: Type *type = 0;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (operand, Core::Ast::Node, SHARED_REF, setOperand(value), operand.get()),
    (type, Type, PLAIN_REF, setType(value), type)
  );

  IMPLEMENT_AST_MAP_PRINTABLE(TypeOp);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(TypeOp);

  IMPLEMENT_ATTR_CONSTRUCTOR(TypeOp);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(TypeOp);

  public: virtual ~TypeOp()
  {
    DISOWN_SHAREDPTR(this->operand);
  }


  //============================================================================
  // Member Functions

  public: void setOperand(SharedPtr<Core::Ast::Node> const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }
  private: void setOperand(Core::Ast::Node *o)
  {
    this->setOperand(getSharedPtr(o));
  }

  public: SharedPtr<Core::Ast::Node> const& getOperand() const
  {
    return this->operand;
  }

  public: void setType(Type *t)
  {
    this->type = t;
  }

  public: Type* getType() const
  {
    return this->type;
  }

}; // class

} // namespace

#endif

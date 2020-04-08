/**
 * @file Spp/Ast/InitOp.h
 * Contains the header of class Spp::Ast::InitOp.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_INITOP_H
#define SPP_AST_INITOP_H

namespace Spp::Ast
{

class InitOp : public Core::Data::Node,
               public Binding, public MapContaining<TiObject>,
               public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(InitOp, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(InitOp);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;
  private: TioSharedPtr param;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(InitOp);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (operand, TiObject, SHARED_REF, setOperand(value), operand.get()),
    (param, TiObject, SHARED_REF, setParam(value), param.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(InitOp);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(InitOp);

  IMPLEMENT_ATTR_CONSTRUCTOR(InitOp);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(InitOp);

  public: virtual ~InitOp()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->param);
  }


  //============================================================================
  // Member Functions

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

  public: void setParam(TioSharedPtr const &p)
  {
    UPDATE_OWNED_SHAREDPTR(this->param, p);
  }
  private: void setParam(TiObject *p)
  {
    this->setParam(getSharedPtr(p));
  }

  public: TioSharedPtr const& getParam() const
  {
    return this->param;
  }

}; // class

} // namespace

#endif

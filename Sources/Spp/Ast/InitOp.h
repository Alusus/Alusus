/**
 * @file Spp/Ast/InitOp.h
 * Contains the header of class Spp::Ast::InitOp.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_INITOP_H
#define SPP_AST_INITOP_H

namespace Spp::Ast
{

class InitOp : public Core::Ast::Node,
               public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(InitOp, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(InitOp);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> operand;
  private: SharedPtr<Core::Ast::Node> param;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (operand, Core::Ast::Node, SHARED_REF, setOperand(value), operand.get()),
    (param, Core::Ast::Node, SHARED_REF, setParam(value), param.get())
  );


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

  public: void setParam(SharedPtr<Core::Ast::Node> const &p)
  {
    UPDATE_OWNED_SHAREDPTR(this->param, p);
  }
  private: void setParam(Core::Ast::Node *p)
  {
    this->setParam(getSharedPtr(p));
  }

  public: SharedPtr<Core::Ast::Node> const& getParam() const
  {
    return this->param;
  }

}; // class

} // namespace

#endif

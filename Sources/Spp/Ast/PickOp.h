/**
 * @file Spp/Ast/PickOp.h
 * Contains the header of class Spp::Ast::PickOp.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_PICKOP_H
#define SPP_AST_PICKOP_H

namespace Spp::Ast
{

/**
 * @brief The `~pick` operator, which picks the element matching a given type among multiple elements with the same name.
 * @ingroup spp_ast
 *
 * A function is picked if its type matches the given function type, while a variable is picked if its type can be
 * cast to the given type.
 */
class PickOp : public Core::Ast::Node,
               public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(PickOp, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(PickOp);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> operand;
  private: SharedPtr<Core::Ast::Node> targetType;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (operand, Core::Ast::Node, SHARED_REF, setOperand(value), operand.get()),
    (targetType, Core::Ast::Node, SHARED_REF, setTargetType(value), targetType.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(PickOp);

  IMPLEMENT_ATTR_CONSTRUCTOR(PickOp);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(PickOp);

  public: virtual ~PickOp()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->targetType);
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

  public: void setTargetType(SharedPtr<Core::Ast::Node> const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->targetType, t);
  }
  private: void setTargetType(Core::Ast::Node *t)
  {
    this->setTargetType(getSharedPtr(t));
  }

  public: SharedPtr<Core::Ast::Node> const& getTargetType() const
  {
    return this->targetType;
  }

}; // class

} // namespace

#endif

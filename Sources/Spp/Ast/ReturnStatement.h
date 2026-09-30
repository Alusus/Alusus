/**
 * @file Spp/Ast/ReturnStatement.h
 * Contains the header of class Spp::Ast::ReturnStatement.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_RETURNSTATEMENT_H
#define SPP_AST_RETURNSTATEMENT_H

namespace Spp::Ast
{

class ReturnStatement : public Core::Ast::Node,
                        public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReturnStatement, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(ReturnStatement);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> operand;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>, (operand, Core::Ast::Node, SHARED_REF, setOperand(value), operand.get()));


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ReturnStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(ReturnStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ReturnStatement);

  public: virtual ~ReturnStatement()
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

}; // class

} // namespace

#endif

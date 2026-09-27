/**
 * @file Spp/Ast/IfStatement.h
 * Contains the header of class Spp::Ast::IfStatement.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_IFSTATEMENT_H
#define SPP_AST_IFSTATEMENT_H

namespace Spp::Ast
{

class IfStatement : public Core::Ast::Node,
                    public Binding, public MapContaining<Core::Ast::Node>,
                    public Core::Ast::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfStatement, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>,
    Core::Ast::Printable
  );
  OBJECT_FACTORY(IfStatement);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> condition;
  private: SharedPtr<Core::Ast::Node> ifBody;
  private: SharedPtr<Core::Ast::Node> elseBody;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (condition, Core::Ast::Node, SHARED_REF, setCondition(value), condition.get()),
    (ifBody, Core::Ast::Node, SHARED_REF, setIfBody(value), ifBody.get()),
    (elseBody, Core::Ast::Node, SHARED_REF, setElseBody(value), elseBody.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(IfStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(IfStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(IfStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(IfStatement);

  public: virtual ~IfStatement()
  {
    DISOWN_SHAREDPTR(this->condition);
    DISOWN_SHAREDPTR(this->ifBody);
    DISOWN_SHAREDPTR(this->elseBody);
  }


  //============================================================================
  // Member Functions

  public: void setCondition(SharedPtr<Core::Ast::Node> const &cond)
  {
    UPDATE_OWNED_SHAREDPTR(this->condition, cond);
  }
  private: void setCondition(Core::Ast::Node *cond)
  {
    this->setCondition(getSharedPtr(cond));
  }

  public: SharedPtr<Core::Ast::Node> const& getCondition() const
  {
    return this->condition;
  }

  public: void setIfBody(SharedPtr<Core::Ast::Node> const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->ifBody, body);
  }
  private: void setIfBody(Core::Ast::Node *body)
  {
    this->setIfBody(getSharedPtr(body));
  }

  public: SharedPtr<Core::Ast::Node> const& getIfBody() const
  {
    return this->ifBody;
  }

  public: void setElseBody(SharedPtr<Core::Ast::Node> const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->elseBody, body);
  }
  private: void setElseBody(Core::Ast::Node *body)
  {
    this->setElseBody(getSharedPtr(body));
  }

  public: SharedPtr<Core::Ast::Node> const& getElseBody() const
  {
    return this->elseBody;
  }

}; // class

} // namespace

#endif

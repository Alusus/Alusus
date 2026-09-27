/**
 * @file Spp/Ast/WhileStatement.h
 * Contains the header of class Spp::Ast::WhileStatement.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_WHILESTATEMENT_H
#define SPP_AST_WHILESTATEMENT_H

namespace Spp::Ast
{

class WhileStatement : public Core::Ast::Node,
                       public Binding, public MapContaining<Core::Ast::Node>,
                       public Core::Ast::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(WhileStatement, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>,
    Core::Ast::Printable
  );
  OBJECT_FACTORY(WhileStatement);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> condition;
  private: SharedPtr<Core::Ast::Node> body;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (condition, Core::Ast::Node, SHARED_REF, setCondition(value), condition.get()),
    (body, Core::Ast::Node, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(WhileStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(WhileStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(WhileStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(WhileStatement);

  public: virtual ~WhileStatement()
  {
    DISOWN_SHAREDPTR(this->condition);
    DISOWN_SHAREDPTR(this->body);
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

  public: void setBody(SharedPtr<Core::Ast::Node> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Core::Ast::Node *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Core::Ast::Node> const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif

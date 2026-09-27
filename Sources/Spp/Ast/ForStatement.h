/**
 * @file Spp/Ast/ForStatement.h
 * Contains the header of class Spp::Ast::ForStatement.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_FORSTATEMENT_H
#define SPP_AST_FORSTATEMENT_H

namespace Spp::Ast
{

using namespace Core;

class ForStatement : public Core::Ast::Node,
                     public Binding, public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(ForStatement, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(ForStatement);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> initializer;
  private: SharedPtr<Core::Ast::Node> condition;
  private: SharedPtr<Core::Ast::Node> updater;
  private: SharedPtr<Core::Ast::Node> body;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (initializer, Core::Ast::Node, SHARED_REF, setInitializer(value), initializer.get()),
    (condition, Core::Ast::Node, SHARED_REF, setCondition(value), condition.get()),
    (updater, Core::Ast::Node, SHARED_REF, setUpdater(value), updater.get()),
    (body, Core::Ast::Node, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(ForStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ForStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(ForStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ForStatement);

  public: virtual ~ForStatement()
  {
    DISOWN_SHAREDPTR(this->initializer);
    DISOWN_SHAREDPTR(this->condition);
    DISOWN_SHAREDPTR(this->updater);
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setInitializer(SharedPtr<Core::Ast::Node> const &init)
  {
    UPDATE_OWNED_SHAREDPTR(this->initializer, init);
  }
  private: void setInitializer(Core::Ast::Node *init)
  {
    this->setInitializer(getSharedPtr(init));
  }

  public: SharedPtr<Core::Ast::Node> const& getInitializer() const
  {
    return this->initializer;
  }

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

  public: void setUpdater(SharedPtr<Core::Ast::Node> const &updtr)
  {
    UPDATE_OWNED_SHAREDPTR(this->updater, updtr);
  }
  private: void setUpdater(Core::Ast::Node *updtr)
  {
    this->setUpdater(getSharedPtr(updtr));
  }

  public: SharedPtr<Core::Ast::Node> const& getUpdater() const
  {
    return this->updater;
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

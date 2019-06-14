/**
 * @file Spp/Ast/WhileStatement.h
 * Contains the header of class Spp::Ast::WhileStatement.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_WHILESTATEMENT_H
#define SPP_AST_WHILESTATEMENT_H

namespace Spp::Ast
{

class WhileStatement : public Core::Data::Node,
                       public Binding, public MapContaining<TiObject>,
                       public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(WhileStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(WhileStatement);


  //============================================================================
  // Member Variables

  private: TioSharedPtr condition;
  private: TioSharedPtr body;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(WhileStatement);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (condition, TiObject, SHARED_REF, setCondition(value), condition.get()),
    (body, TiObject, SHARED_REF, setBody(value), body.get())
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

  public: void setCondition(TioSharedPtr const &cond)
  {
    UPDATE_OWNED_SHAREDPTR(this->condition, cond);
  }
  private: void setCondition(TiObject *cond)
  {
    this->setCondition(getSharedPtr(cond));
  }

  public: TioSharedPtr const& getCondition() const
  {
    return this->condition;
  }

  public: void setBody(TioSharedPtr const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(TiObject *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: TioSharedPtr const& getBody() const
  {
    return this->body;
  }

}; // class

} // namespace

#endif

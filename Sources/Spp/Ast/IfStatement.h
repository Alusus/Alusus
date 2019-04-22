/**
 * @file Spp/Ast/IfStatement.h
 * Contains the header of class Spp::Ast::IfStatement.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_IFSTATEMENT_H
#define SPP_AST_IFSTATEMENT_H

namespace Spp::Ast
{

class IfStatement : public Core::Data::Node,
                    public Binding, public MapContaining<TiObject>,
                    public Core::Data::Ast::MetaHaving, public Core::Data::Clonable,
                    public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Binding, MapContaining<TiObject>,
                                         Core::Data::Ast::MetaHaving, Core::Data::Clonable,
                                         Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr condition;
  private: TioSharedPtr ifBody;
  private: TioSharedPtr elseBody;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(IfStatement);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (condition, TiObject, SHARED_REF, setCondition(value), condition.get()),
    (ifBody, TiObject, SHARED_REF, setIfBody(value), ifBody.get()),
    (elseBody, TiObject, SHARED_REF, setElseBody(value), elseBody.get())
  );

  IMPLEMENT_AST_CLONABLE(IfStatement);

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

  public: void setIfBody(TioSharedPtr const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->ifBody, body);
  }
  private: void setIfBody(TiObject *body)
  {
    this->setIfBody(getSharedPtr(body));
  }

  public: TioSharedPtr const& getIfBody() const
  {
    return this->ifBody;
  }

  public: void setElseBody(TioSharedPtr const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->elseBody, body);
  }
  private: void setElseBody(TiObject *body)
  {
    this->setElseBody(getSharedPtr(body));
  }

  public: TioSharedPtr const& getElseBody() const
  {
    return this->elseBody;
  }

}; // class

} // namespace

#endif

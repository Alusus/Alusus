/**
 * @file Spp/Ast/IfStatement.h
 * Contains the header of class Spp::Ast::IfStatement.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_IFSTATEMENT_H
#define SPP_AST_IFSTATEMENT_H

namespace Spp { namespace Ast
{

using namespace Core;

class IfStatement : public Core::Data::Node,
                    public virtual Core::Basic::Bindings, public virtual Core::Data::MapContainer,
                    public virtual Core::Data::Ast::Metadata, public virtual Core::Data::Clonable,
                    public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Basic::Bindings, Core::Data::MapContainer,
                                         Core::Data::Ast::Metadata, Core::Data::Clonable,
                                         Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr condition;
  private: TioSharedPtr ifBody;
  private: TioSharedPtr elseBody;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(IfStatement);

  IMPLEMENT_BINDINGS(Bindings,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, VALUE, setSourceLocation(value), &sourceLocation)
  );

  IMPLEMENT_MAP_CONTAINER(MapContainer,
    (TiObject, condition),
    (TiObject, ifBody),
    (TiObject, elseBody)
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

  public: TioSharedPtr const& getCondition() const
  {
    return this->condition;
  }

  public: void setIfBody(TioSharedPtr const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->ifBody, body);
  }

  public: TioSharedPtr const& getIfBody() const
  {
    return this->ifBody;
  }

  public: void setElseBody(TioSharedPtr const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->elseBody, body);
  }

  public: TioSharedPtr const& getElseBody() const
  {
    return this->elseBody;
  }

}; // class

} } // namespace

#endif

/**
 * @file Spp/Ast/WhileStatement.h
 * Contains the header of class Spp::Ast::WhileStatement.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_WHILESTATEMENT_H
#define SPP_AST_WHILESTATEMENT_H

namespace Spp { namespace Ast
{

class WhileStatement : public Core::Data::Node,
                       public virtual Core::Basic::Bindings, public virtual Core::Data::MapContainer,
                       public virtual Core::Data::Ast::Metadata, public virtual Core::Data::Clonable,
                       public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(WhileStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Basic::Bindings, Core::Data::MapContainer,
                                         Core::Data::Ast::Metadata, Core::Data::Clonable,
                                         Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: Core::Basic::TioSharedPtr condition;
  private: Core::Basic::TioSharedPtr body;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(WhileStatement);

  IMPLEMENT_BINDINGS(Bindings,
                     (prodId, Core::Basic::TiWord, VALUE, setProdId(value), &prodId),
                     (sourceLocation, Core::Data::SourceLocation, VALUE, setSourceLocation(value), &sourceLocation));

  IMPLEMENT_MAP_CONTAINER(MapContainer,
    (Core::Basic::TiObject, condition),
    (Core::Basic::TiObject, body)
  );

  IMPLEMENT_AST_CLONABLE(WhileStatement);

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

  public: void setCondition(Core::Basic::TioSharedPtr const &cond)
  {
    UPDATE_OWNED_SHAREDPTR(this->condition, cond);
  }

  public: Core::Basic::TioSharedPtr const& getCondition() const
  {
    return this->condition;
  }

  public: void setBody(Core::Basic::TioSharedPtr const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }

  public: Core::Basic::TioSharedPtr const& getBody() const
  {
    return this->body;
  }

}; // class

} } // namespace

#endif

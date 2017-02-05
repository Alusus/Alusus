/**
 * @file Spp/Ast/WhileStatement.h
 * Contains the header of class Spp::Ast::WhileStatement.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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

using namespace Core;

class WhileStatement : public Core::Data::Node,
                       public virtual Core::Basic::RtBinding, public virtual Core::Data::MapContainer,
                       public virtual Core::Data::Ast::Metadata, public virtual Core::Data::Clonable,
                       public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(WhileStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Basic::RtBinding, Core::Data::MapContainer,
                                         Core::Data::Ast::Metadata, Core::Data::Clonable,
                                         Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr condition;
  private: TioSharedPtr body;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(WhileStatement);

  IMPLEMENT_RTBINDING((prodId, TiWord, VALUE, setProdId(value), &prodId),
                      (sourceLocation, Core::Data::SourceLocation, VALUE, setSourceLocation(value), &sourceLocation));

  IMPLEMENT_MAP_CONTAINER((TiObject, condition),
                          (TiObject, body));

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

  public: void setCondition(TioSharedPtr const &cond)
  {
    UPDATE_OWNED_SHAREDPTR(this->condition, cond);
  }

  public: TioSharedPtr const& getCondition() const
  {
    return this->condition;
  }

  public: void setBody(TioSharedPtr const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }

  public: TioSharedPtr const& getBody() const
  {
    return this->body;
  }

}; // class

} } // namespace

#endif

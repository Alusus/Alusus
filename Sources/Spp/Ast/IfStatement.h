/**
 * @file Spp/Ast/IfStatement.h
 * Contains the header of class Spp::Ast::IfStatement.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
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
                    public virtual Core::Data::MapContainer, public virtual Core::Data::IdHolder,
                    public virtual Core::Data::Ast::MetadataHolder, public virtual Core::Data::Clonable,
                    public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Data::MapContainer, Core::Data::IdHolder,
                                         Core::Data::Ast::MetadataHolder,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr condition;
  private: SharedPtr<Block> ifBody;
  private: SharedPtr<Block> elseBody;

  IMPLEMENT_MAP_CONTAINER((TiObject, condition),
                          (Block, ifBody),
                          (Block, elseBody));

  IMPLEMENT_AST_CLONABLE(IfStatement);

  IMPLEMENT_AST_MAP_PRINTABLE(IfStatement);


  //============================================================================
  // Constructors & Destructor

  public: IfStatement()
  {
  }

  public: IfStatement(TioSharedPtr const &cond, SharedPtr<Block> const &ifb,
                      SharedPtr<Block> const &elseb) : condition(cond), ifBody(ifb), elseBody(elseb)
  {
    OWN_SHAREDPTR(this->condition);
    OWN_SHAREDPTR(this->ifBody);
    OWN_SHAREDPTR(this->elseBody);
  }

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

  public: void setIfBody(SharedPtr<Block> const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->ifBody, body);
  }

  public: SharedPtr<Block> const& getIfBody() const
  {
    return this->ifBody;
  }

  public: void setElseBody(SharedPtr<Block> const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->elseBody, body);
  }

  public: SharedPtr<Block> const& getElseBody() const
  {
    return this->elseBody;
  }

}; // class

} } // namespace

#endif

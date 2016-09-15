/**
 * @file Spp/Ast/ReturnStatement.h
 * Contains the header of class Spp::Ast::ReturnStatement.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_RETURNSTATEMENT_H
#define SPP_AST_RETURNSTATEMENT_H

namespace Spp { namespace Ast
{

class ReturnStatement : public Core::Data::Node,
                        public virtual Core::Data::MapContainer, public virtual Core::Data::Ast::MetadataHolder,
                        public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReturnStatement, Core::Data::Node, "Spp.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Data::MapContainer, Core::Data::Ast::MetadataHolder,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;

  IMPLEMENT_MAP_CONTAINER((TiObject, operand));

  IMPLEMENT_AST_CLONABLE(ReturnStatement);

  IMPLEMENT_AST_MAP_PRINTABLE(ReturnStatement);


  //============================================================================
  // Constructors & Destructor

  public: ReturnStatement()
  {
  }

  public: ReturnStatement(Word pid, Core::Data::SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: ReturnStatement(Word pid, TioSharedPtr const &o) :
    MetadataHolder(pid), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: ReturnStatement(Word pid, Core::Data::SourceLocation const &sl, TioSharedPtr const &o) :
    MetadataHolder(pid, sl), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: virtual ~ReturnStatement()
  {
    DISOWN_SHAREDPTR(this->operand);
  }

  public: static SharedPtr<ReturnStatement> create()
  {
    return std::make_shared<ReturnStatement>();
  }

  public: static SharedPtr<ReturnStatement> create(Word pid, Core::Data::SourceLocation const &sl)
  {
    return std::make_shared<ReturnStatement>(pid, sl);
  }

  public: static SharedPtr<ReturnStatement> create(Word pid, TioSharedPtr const &o)
  {
    return std::make_shared<ReturnStatement>(pid, o);
  }

  public: static SharedPtr<ReturnStatement> create(Word pid, Core::Data::SourceLocation const &sl,
                                                   TioSharedPtr const &o)
  {
    return std::make_shared<ReturnStatement>(pid, sl, o);
  }


  //============================================================================
  // Member Functions

  public: void setOperand(TioSharedPtr const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }

  public: TioSharedPtr const& getOperand() const
  {
    return this->operand;
  }

}; // class

} } // namespace

#endif

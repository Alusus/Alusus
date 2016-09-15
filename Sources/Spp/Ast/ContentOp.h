/**
 * @file Spp/Ast/ContentOp.h
 * Contains the header of class Spp::Ast::ContentOp.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_CONTENTOP_H
#define SPP_AST_CONTENTOP_H

namespace Spp { namespace Ast
{

class ContentOp : public Core::Data::Node,
                  public virtual Core::Data::MapContainer, public virtual Core::Data::Ast::MetadataHolder,
                  public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ContentOp, Core::Data::Node, "Spp.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Data::MapContainer, Core::Data::Ast::MetadataHolder,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;

  IMPLEMENT_MAP_CONTAINER((TiObject, operand));

  IMPLEMENT_AST_CLONABLE(ContentOp);

  IMPLEMENT_AST_MAP_PRINTABLE(ContentOp);


  //============================================================================
  // Constructors & Destructor

  public: ContentOp()
  {
  }

  public: ContentOp(Word pid, Core::Data::SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: ContentOp(Word pid, TioSharedPtr const &o) :
    MetadataHolder(pid), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: ContentOp(Word pid, Core::Data::SourceLocation const &sl, TioSharedPtr const &o) :
    MetadataHolder(pid, sl), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: virtual ~ContentOp()
  {
    DISOWN_SHAREDPTR(this->operand);
  }

  public: static SharedPtr<ContentOp> create()
  {
    return std::make_shared<ContentOp>();
  }

  public: static SharedPtr<ContentOp> create(Word pid, Core::Data::SourceLocation const &sl)
  {
    return std::make_shared<ContentOp>(pid, sl);
  }

  public: static SharedPtr<ContentOp> create(Word pid, TioSharedPtr const &o)
  {
    return std::make_shared<ContentOp>(pid, o);
  }

  public: static SharedPtr<ContentOp> create(Word pid, Core::Data::SourceLocation const &sl, TioSharedPtr const &o)
  {
    return std::make_shared<ContentOp>(pid, sl, o);
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

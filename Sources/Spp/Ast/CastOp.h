/**
 * @file Spp/Ast/CastOp.h
 * Contains the header of class Spp::Ast::CastOp.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_CASTOP_H
#define SPP_AST_CASTOP_H

namespace Spp { namespace Ast
{

class CastOp : public Core::Data::Node,
               public virtual Core::Data::MapContainer, public virtual Core::Data::Ast::MetadataHolder,
               public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(CastOp, Core::Data::Node, "Spp.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Data::MapContainer, Core::Data::Ast::MetadataHolder,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;
  private: TioSharedPtr targetType;

  IMPLEMENT_MAP_CONTAINER((TiObject, operand), (TiObject, targetType));

  IMPLEMENT_AST_CLONABLE(PointerOp);

  IMPLEMENT_AST_MAP_PRINTABLE(CastOp);


  //============================================================================
  // Constructors & Destructor

  public: CastOp()
  {
  }

  public: CastOp(Word pid, Core::Data::SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: CastOp(Word pid, TioSharedPtr const &o, TioSharedPtr const &t) :
    MetadataHolder(pid), operand(o), targetType(t)
  {
    OWN_SHAREDPTR(this->operand);
    OWN_SHAREDPTR(this->targetType);
  }

  public: CastOp(Word pid, Core::Data::SourceLocation const &sl, TioSharedPtr const &o, TioSharedPtr const &t) :
    MetadataHolder(pid, sl), operand(o), targetType(t)
  {
    OWN_SHAREDPTR(this->operand);
    OWN_SHAREDPTR(this->targetType);
  }

  public: virtual ~CastOp()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->targetType);
  }

  public: static SharedPtr<CastOp> create()
  {
    return std::make_shared<CastOp>();
  }

  public: static SharedPtr<CastOp> create(Word pid, Core::Data::SourceLocation const &sl)
  {
    return std::make_shared<CastOp>(pid, sl);
  }

  public: static SharedPtr<CastOp> create(Word pid, TioSharedPtr const &o, TioSharedPtr const &t)
  {
    return std::make_shared<CastOp>(pid, o, t);
  }

  public: static SharedPtr<CastOp> create(Word pid, Core::Data::SourceLocation const &sl,
                                          TioSharedPtr const &o, TioSharedPtr const &t)
  {
    return std::make_shared<CastOp>(pid, sl, o, t);
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

  public: void setTargetType(TioSharedPtr const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->targetType, t);
  }

  public: TioSharedPtr const& getTargetType() const
  {
    return this->targetType;
  }

}; // class

} } // namespace

#endif

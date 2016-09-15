/**
 * @file Spp/Ast/PointerOp.h
 * Contains the header of class Spp::Ast::PointerOp.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_POINTEROP_H
#define SPP_AST_POINTEROP_H

namespace Spp { namespace Ast
{

class PointerOp : public Core::Data::Node,
                  public virtual Core::Data::MapContainer, public virtual Core::Data::Ast::MetadataHolder,
                  public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(PointerOp, Core::Data::Node, "Spp.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Data::MapContainer, Core::Data::Ast::MetadataHolder,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;

  IMPLEMENT_MAP_CONTAINER((TiObject, operand));

  IMPLEMENT_AST_CLONABLE(PointerOp);

  IMPLEMENT_AST_MAP_PRINTABLE(PointerOp);


  //============================================================================
  // Constructors & Destructor

  public: PointerOp()
  {
  }

  public: PointerOp(Word pid, Core::Data::SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: PointerOp(Word pid, TioSharedPtr const &o) :
    MetadataHolder(pid), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: PointerOp(Word pid, Core::Data::SourceLocation const &sl, TioSharedPtr const &o) :
    MetadataHolder(pid, sl), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: virtual ~PointerOp()
  {
    DISOWN_SHAREDPTR(this->operand);
  }

  public: static SharedPtr<PointerOp> create()
  {
    return std::make_shared<PointerOp>();
  }

  public: static SharedPtr<PointerOp> create(Word pid, Core::Data::SourceLocation const &sl)
  {
    return std::make_shared<PointerOp>(pid, sl);
  }

  public: static SharedPtr<PointerOp> create(Word pid, TioSharedPtr const &o)
  {
    return std::make_shared<PointerOp>(pid, o);
  }

  public: static SharedPtr<PointerOp> create(Word pid, Core::Data::SourceLocation const &sl, TioSharedPtr const &o)
  {
    return std::make_shared<PointerOp>(pid, sl, o);
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

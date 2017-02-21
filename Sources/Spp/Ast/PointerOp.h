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
                  public virtual Core::Basic::Bindings, public virtual Core::Data::MapContainer,
                  public virtual Core::Data::Ast::Metadata, public virtual Core::Data::Clonable,
                  public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(PointerOp, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Core::Basic::Bindings, Core::Data::MapContainer, Core::Data::Ast::Metadata,
                                         Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(PointerOp);

  IMPLEMENT_BINDINGS((prodId, TiWord, VALUE, setProdId(value), &prodId),
                      (sourceLocation, Core::Data::SourceLocation, VALUE, setSourceLocation(value), &sourceLocation));

  IMPLEMENT_MAP_CONTAINER((TiObject, operand));

  IMPLEMENT_AST_CLONABLE(PointerOp);

  IMPLEMENT_AST_MAP_PRINTABLE(PointerOp);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(PointerOp);

  IMPLEMENT_ATTR_CONSTRUCTOR(PointerOp);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(PointerOp);

  public: virtual ~PointerOp()
  {
    DISOWN_SHAREDPTR(this->operand);
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

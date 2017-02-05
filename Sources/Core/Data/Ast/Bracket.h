/**
 * @file Core/Data/Ast/Bracket.h
 * Contains the header of class Core::Data::Ast::Bracket.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_BRACKET_H
#define CORE_DATA_AST_BRACKET_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class Bracket : public Node,
                public virtual RtBinding, public virtual MapContainer, public virtual Metadata,
                public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Bracket, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES(Node, RtBinding, MapContainer, Metadata, Clonable, Printable);


  //============================================================================
  // Member Variables

  private: BracketType type;
  private: TioSharedPtr operand;


  //============================================================================
  // Implementations

  IMPLEMENT_METADATA(Bracket);

  IMPLEMENT_RTBINDING((type, BracketType, VALUE, setType(value), &type),
                      (prodId, TiWord, VALUE, setProdId(value), &prodId),
                      (sourceLocation, SourceLocation, VALUE, setSourceLocation(value), &sourceLocation));

  IMPLEMENT_MAP_CONTAINER((TiObject, operand));

  IMPLEMENT_AST_MAP_PRINTABLE(Bracket, << (this->type == BracketType::ROUND ? STR("()") : STR("[]")));


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Bracket);

  IMPLEMENT_ATTR_CONSTRUCTOR(Bracket);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Bracket);

  public: virtual ~Bracket()
  {
    DISOWN_SHAREDPTR(this->operand);
  }


  //============================================================================
  // Member Functions

  public: void setType(BracketType const &t)
  {
    this->type = t;
  }
  public: void setType(BracketType const *t)
  {
    this->type = t == 0 ? BracketType::ROUND : t->get();
  }

  public: BracketType const& getType() const
  {
    return this->type;
  }

  public: void setOperand(TioSharedPtr const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }

  public: TioSharedPtr const& getOperand() const
  {
    return this->operand;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;

}; // class

} } } // namespace

#endif

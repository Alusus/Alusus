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
                public virtual Container, public virtual MetadataHolder,
                public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Bracket, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(Node, Container, MetadataHolder, Clonable, Printable);


  //============================================================================
  // Member Variables

  private: BracketType type;
  private: TioSharedPtr operand;


  //============================================================================
  // Constructors & Destructor

  public: Bracket()
  {
  }

  public: Bracket(Word pid, SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: Bracket(Word pid, BracketType t, TioSharedPtr const &o) :
    MetadataHolder(pid), type(t), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: Bracket(Word pid, SourceLocation const &sl, BracketType t, TioSharedPtr const &o) :
    MetadataHolder(pid, sl), type(t), operand(o)
  {
    OWN_SHAREDPTR(this->operand);
  }

  public: virtual ~Bracket()
  {
    DISOWN_SHAREDPTR(this->operand);
  }

  public: static SharedPtr<Bracket> create()
  {
    return std::make_shared<Bracket>();
  }

  public: static SharedPtr<Bracket> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<Bracket>(pid, sl);
  }

  public: static SharedPtr<Bracket> create(Word pid, BracketType t, TioSharedPtr const &o)
  {
    return std::make_shared<Bracket>(pid, t, o);
  }

  public: static SharedPtr<Bracket> create(Word pid, SourceLocation const &sl, BracketType t, TioSharedPtr const &o)
  {
    return std::make_shared<Bracket>(pid, sl, t, o);
  }


  //============================================================================
  // Member Functions

  public: void setType(BracketType t)
  {
    this->type = t;
  }

  public: BracketType getType() const
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
  // Container Implementation

  public: virtual void set(Int index, TiObject *val);

  public: virtual void remove(Int index);

  public: virtual Word getCount() const
  {
    return 1;
  }

  public: virtual TiObject* get(Int index) const;


  //============================================================================
  // MetadataHolder Overrides

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getSourceLocation on the children.
   */
  public: virtual SourceLocation const& getSourceLocation() const;

  public: virtual TiObject* getAttribute(Char const *name);


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } } // namespace

#endif

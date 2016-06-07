/**
 * @file Core/Data/Ast/ParamPass.h
 * Contains the header of class Core::Data::Ast::ParamPass.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_PARAMPASS_H
#define CORE_DATA_AST_PARAMPASS_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class ParamPass : public Node,
                  public virtual Container, public virtual MetadataHolder,
                  public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParamPass, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(Node, Container, MetadataHolder, Clonable, Printable);


  //============================================================================
  // Member Variables

  private: BracketType type;
  private: TioSharedPtr operand;
  private: TioSharedPtr param;


  //============================================================================
  // Constructors & Destructor

  public: ParamPass()
  {
  }

  public: ParamPass(Word pid, SourceLocation const &sl) :
    MetadataHolder(pid, sl)
  {
  }

  public: ParamPass(Word pid, BracketType t, TioSharedPtr const &o, TioSharedPtr const &p) :
    MetadataHolder(pid), type(t), operand(o), param(p)
  {
    OWN_SHAREDPTR(this->operand);
    OWN_SHAREDPTR(this->param);
  }

  public: ParamPass(Word pid, SourceLocation const &sl, BracketType t, TioSharedPtr const &o, TioSharedPtr const &p) :
    MetadataHolder(pid, sl), type(t), operand(o), param(p)
  {
    OWN_SHAREDPTR(this->operand);
    OWN_SHAREDPTR(this->param);
  }

  public: virtual ~ParamPass()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->param);
  }

  public: static SharedPtr<ParamPass> create()
  {
    return std::make_shared<ParamPass>();
  }

  public: static SharedPtr<ParamPass> create(Word pid, SourceLocation const &sl)
  {
    return std::make_shared<ParamPass>(pid, sl);
  }

  public: static SharedPtr<ParamPass> create(Word pid, BracketType t, TioSharedPtr const &o, TioSharedPtr const &p)
  {
    return std::make_shared<ParamPass>(pid, t, o, p);
  }

  public: static SharedPtr<ParamPass> create(Word pid, SourceLocation const &sl,
                                             BracketType t, TioSharedPtr const &o, TioSharedPtr const &p)
  {
    return std::make_shared<ParamPass>(pid, sl, t, o, p);
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

  public: void setParam(TioSharedPtr const &p)
  {
    UPDATE_OWNED_SHAREDPTR(this->param, p);
  }

  public: TioSharedPtr const& getParam() const
  {
    return this->param;
  }


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, TiObject *val);

  public: virtual void remove(Int index);

  public: virtual Word getCount() const
  {
    return 2;
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

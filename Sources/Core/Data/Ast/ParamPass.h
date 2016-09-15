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
                  public virtual MapContainer, public virtual MetadataHolder,
                  public virtual Clonable, public virtual Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParamPass, Node, "Core.Data.Ast", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_4(Node, MapContainer, MetadataHolder, Clonable, Printable);


  //============================================================================
  // Member Variables

  private: BracketType type;
  private: TioSharedPtr operand;
  private: TioSharedPtr param;

  IMPLEMENT_MAP_CONTAINER((TiObject, operand), (TiObject, param));

  IMPLEMENT_AST_MAP_PRINTABLE(ParamPass, << (this->type == BracketType::ROUND ? STR("()") : STR("[]")));


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
  // MetadataHolder Overrides

  public: virtual TiObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("type")) {
      return &this->type;
    }
    return MetadataHolder::getAttribute(name);
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;

}; // class

} } } // namespace

#endif

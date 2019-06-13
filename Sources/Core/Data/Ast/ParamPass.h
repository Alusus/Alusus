/**
 * @file Core/Data/Ast/ParamPass.h
 * Contains the header of class Core::Data::Ast::ParamPass.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_PARAMPASS_H
#define CORE_DATA_AST_PARAMPASS_H

namespace Core { namespace Data { namespace Ast
{

// TODO: DOC

class ParamPass : public Node,
                  public Binding, public MapContaining<TiObject>, public MetaHaving,
                  public Clonable, public Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ParamPass, Node, "Core.Data.Ast", "Core", "alusus.org");
  IMPLEMENT_INTERFACES(Node, Binding, MapContaining<TiObject>, MetaHaving, Clonable, Printable);
  OBJECT_FACTORY(ParamPass);


  //============================================================================
  // Member Variables

  private: BracketType type;
  private: TioSharedPtr operand;
  private: TioSharedPtr param;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(ParamPass);

  IMPLEMENT_BINDING(Binding,
    (type, BracketType, VALUE, setType(value), &type),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (operand, TiObject, SHARED_REF, setOperand(value), operand.get()),
    (param, TiObject, SHARED_REF, setParam(value), param.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(ParamPass, << (this->type == BracketType::ROUND ? S("()") : S("[]")));


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ParamPass);

  IMPLEMENT_ATTR_CONSTRUCTOR(ParamPass);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ParamPass);

  public: virtual ~ParamPass()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->param);
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
  private: void setOperand(TiObject *o)
  {
    this->setOperand(getSharedPtr(o));
  }

  public: TioSharedPtr const& getOperand() const
  {
    return this->operand;
  }

  public: void setParam(TioSharedPtr const &p)
  {
    UPDATE_OWNED_SHAREDPTR(this->param, p);
  }
  private: void setParam(TiObject *p)
  {
    this->setParam(getSharedPtr(p));
  }

  public: TioSharedPtr const& getParam() const
  {
    return this->param;
  }


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;

}; // class

} } } // namespace

#endif

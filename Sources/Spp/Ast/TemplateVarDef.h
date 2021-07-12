/**
 * @file Spp/Ast/TemplateVarDef.h
 * Contains the header of class Spp::Ast::TemplateVarDef.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_TEMPLATEVARDEF_H
#define SPP_AST_TEMPLATEVARDEF_H

namespace Spp::Ast
{

//============================================================================
// Other Defs

ti_s_enum(TemplateVarType, TiInt, "Spp.Ast", "Spp", "alusus.org", STRING, INTEGER, TYPE, FUNCTION, AST);


//============================================================================
// TemplateVarDef class

class TemplateVarDef: public Core::Data::Node, public Binding, public Core::Data::Ast::MetaHaving
{
  //============================================================================
  // Type Info

  TYPE_INFO(TemplateVarDef, Core::Data::Node, "Spp.Ast.Template", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(Binding, Core::Data::Ast::MetaHaving)
  ));


  //============================================================================
  // Member Variables

  private: TiStr name;
  private: TemplateVarType type;
  private: TioSharedPtr defaultVal;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(TemplateVarDef);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get()),
    (name, TiStr, VALUE, setName(value), &this->name),
    (type, TemplateVarType, VALUE, setType(value), &this->type),
    (defaultVal, TiObject, SHARED_REF, setDefaultVal(value), this->defaultVal.get())
  );


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(TemplateVarDef);

  IMPLEMENT_ATTR_CONSTRUCTOR(TemplateVarDef);

  public: TemplateVarDef(Char const *n, TemplateVarType t, TioSharedPtr const &v = TioSharedPtr())
    : name(n), type(t), defaultVal(v)
  {
    OWN_SHAREDPTR(this->defaultVal);
  }

  public: virtual ~TemplateVarDef()
  {
    DISOWN_SHAREDPTR(this->defaultVal);
  }


  //============================================================================
  // Member Functions

  public: void setName(Char const *n)
  {
    this->name = n;
  }
  public: void setName(TiStr const *n)
  {
    this->name = n == 0 ? "" : n->get();
  }

  public: TiStr const& getName() const
  {
    return this->name;
  }

  public: void setType(TemplateVarType const &t)
  {
    this->type = t;
  }
  public: void setType(TemplateVarType const *t)
  {
    this->type = t == 0 ? TemplateVarType::STRING : t->get();
  }

  public: TemplateVarType const& getType() const
  {
    return this->type;
  }

  public: void setDefaultVal(TioSharedPtr const &val)
  {
    UPDATE_OWNED_SHAREDPTR(this->defaultVal, val);
  }
  private: void setDefaultVal(TiObject *val)
  {
    this->setDefaultVal(getSharedPtr(val));
  }

  public: TioSharedPtr const& getDefaultVal() const
  {
    return this->defaultVal;
  }

}; // class

} // namespace

#endif

/**
 * @file Spp/Ast/TemplateVarDef.h
 * Contains the header of class Spp::Ast::TemplateVarDef.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

ti_s_enum(TemplateVarType, TiInt, "Spp.Ast", "Spp", "alusus.org",
  STRING,
  INTEGER,
  MODULE,
  TYPE,
  FUNCTION,
  AST,
  AST_REF
);


//============================================================================
// TemplateVarDef class

class TemplateVarDef: public Core::Ast::Node, public Binding
{
  //============================================================================
  // Type Info

  TYPE_INFO(TemplateVarDef, Core::Ast::Node, "Spp.Ast.Template", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(Binding)
  ));
  OBJECT_FACTORY(TemplateVarDef);


  //============================================================================
  // Member Variables

  private: TiStr name;
  private: TemplateVarType type;
  private: SharedPtr<Core::Ast::Node> defaultVal;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get()),
    (name, TiStr, VALUE, setName(value), &this->name),
    (type, TemplateVarType, VALUE, setType(value), &this->type),
    (defaultVal, Core::Ast::Node, SHARED_REF, setDefaultVal(value), this->defaultVal.get())
  );


  //============================================================================
  // Constructors

  IMPLEMENT_EMPTY_CONSTRUCTOR(TemplateVarDef);

  IMPLEMENT_ATTR_CONSTRUCTOR(TemplateVarDef);

  public: TemplateVarDef(Char const *n, TemplateVarType t, SharedPtr<Core::Ast::Node> const &v = SharedPtr<Core::Ast::Node>())
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

  public: void setDefaultVal(SharedPtr<Core::Ast::Node> const &val)
  {
    UPDATE_OWNED_SHAREDPTR(this->defaultVal, val);
  }
  private: void setDefaultVal(Core::Ast::Node *val)
  {
    this->setDefaultVal(getSharedPtr(val));
  }

  public: SharedPtr<Core::Ast::Node> const& getDefaultVal() const
  {
    return this->defaultVal;
  }

}; // class

} // namespace

#endif

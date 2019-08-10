/**
 * @file Spp/Ast/Template.h
 * Contains the header of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_TEMPLATE_H
#define SPP_AST_TEMPLATE_H

namespace Spp::Ast
{

class Template : public Core::Data::Node,
                 public Binding, public MapContaining<TiObject>,
                 public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Template, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Data::Ast::List> varDefs;

  private: TioSharedPtr body;

  private: SharedList<Core::Data::Ast::Scope> instances;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Template);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (varDefs, Core::Data::Ast::List, SHARED_REF, setVarDefs(value), varDefs.get()),
    (body, TiObject, SHARED_REF, setBody(value), body.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Template);

  IMPLEMENT_ATTR_CONSTRUCTOR(Template);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Template);

  public: virtual ~Template()
  {
    DISOWN_SHAREDPTR(this->varDefs);
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setVarDefs(SharedPtr<Core::Data::Ast::List> const &defs)
  {
    UPDATE_OWNED_SHAREDPTR(this->varDefs, defs);
  }
  private: void setVarDefs(Core::Data::Ast::List *defs)
  {
    this->setVarDefs(getSharedPtr(defs));
  }

  public: SharedPtr<Core::Data::Ast::List> const& getVarDefs() const
  {
    return this->varDefs;
  }

  public: Word getVarDefCount() const
  {
    return this->varDefs == 0 ? 0 : this->varDefs->getCount();
  }

  public: void setBody(TioSharedPtr const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(TiObject *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: TioSharedPtr const& getBody() const
  {
    return this->body;
  }

  public: virtual TioSharedPtr const& getDefaultInstance(Helper *helper);
  public: virtual Bool matchInstance(TiObject *templateInputs, Helper *helper, TioSharedPtr &result);

  private: Bool prepareTemplateVars(
    TiObject *templateInputs, Helper *helper, PlainList<TiObject> *vars, SharedPtr<Core::Notices::Notice> &notice
  );

  private: Bool matchTemplateVars(
    Containing<TiObject> *templateInputs, Core::Data::Ast::Scope *instance, Helper *helper,
    SharedPtr<Core::Notices::Notice> &notice
  );
  private: Bool matchTemplateVar(
    TiObject *templateInput, Core::Data::Ast::Scope *instance, TemplateVarDef *varDef, Helper *helper,
    SharedPtr<Core::Notices::Notice> &notice
  );

  private: Bool assignTemplateVars(
    Containing<TiObject> *templateInputs, Core::Data::Ast::Scope *instance, Helper *helper,
    SharedPtr<Core::Notices::Notice> &notice
  );

  public: static TiObject* getTemplateVar(Core::Data::Ast::Scope const *instance, Char const *name);

  private: static TiObject* traceObject(TiObject *ref, TemplateVarType varType, Helper *helper);

  public: Word getInstanceCount() const
  {
    return this->instances.getCount();
  }

  public: SharedPtr<Core::Data::Ast::Scope> const& getInstance(Int index)
  {
    return this->instances.get(index);
  }


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} // namespace

#endif

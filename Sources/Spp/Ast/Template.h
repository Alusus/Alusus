/**
 * @file Spp/Ast/Template.h
 * Contains the header of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_TEMPLATE_H
#define SPP_AST_TEMPLATE_H

namespace Spp { namespace Ast
{

class Template : public Core::Data::Node,
                 public virtual Binding, public virtual MapContaining<TiObject>,
                 public virtual Core::Data::Ast::MetaHaving, public virtual Core::Data::Clonable,
                 public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Template, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Binding, MapContaining<TiObject>,
                                         Core::Data::Ast::MetaHaving, Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Types

  public: s_enum(VarType, INTEGER, STRING, TYPE, FUNCTION);

  public: struct VarDef
  {
    Str name;
    VarType type;
    TioSharedPtr defaultVal;
    VarDef(Char const *n, VarType t, TioSharedPtr const &v = TioSharedPtr()) : name(n), type(t), defaultVal(v) {}
  };


  //============================================================================
  // Member Variables

  private: std::vector<VarDef> varDefs;

  private: std::vector<SharedPtr<Block>> instances;

  private: SharedPtr<Data::Clonable> templateBody;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Template);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (templateBody, Data::Clonable, SHARED_REF, setTemplateBody(value), templateBody.get())
  );


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Template);

  IMPLEMENT_ATTR_CONSTRUCTOR(Template);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Template);

  public: virtual ~Template()
  {
    DISOWN_SHAREDPTR(this->templateBody);
  }


  //============================================================================
  // Member Functions

  public: void setTemplateBody(SharedPtr<Data::Clonable> const &body)
  {
    UPDATE_OWNED_SHAREDPTR(this->templateBody, body);
  }
  private: void setTemplateBody(Data::Clonable *body)
  {
    this->setTemplateBody(getSharedPtr(body));
  }

  public: SharedPtr<Data::Clonable> const& getTemplateBody() const
  {
    return this->templateBody;
  }

  public: void setVarDefs(std::vector<VarDef> const *defs)
  {
    this->varDefs = *defs;
    for (Int i = 0; i < this->varDefs.size(); ++i) OWN_SHAREDPTR(this->varDefs[i].defaultVal);
  }

  public: void setVarDefs(std::initializer_list<VarDef> const &vars)
  {
    this->varDefs = vars;
    for (Int i = 0; i < this->varDefs.size(); ++i) OWN_SHAREDPTR(this->varDefs[i].defaultVal);
  }

  public: std::vector<VarDef> const* getVarDefs() const
  {
    return &this->varDefs;
  }

  public: virtual TioSharedPtr const& getDefaultInstance(Helper *helper);
  public: virtual Bool matchInstance(TiObject *templateInput, Helper *helper, TioSharedPtr &result);

  private: Bool matchTemplateVars(
    TiObject *templateInput, Block *instance, Helper *helper, SharedPtr<Core::Notices::Notice> &notice
  );
  private: Bool matchTemplateVar(
    TiObject *templateInput, Block *instance, Int varIndex, Helper *helper,
    SharedPtr<Core::Notices::Notice> &notice
  );

  private: Bool assignTemplateVars(
    TiObject *templateInput, Block *instance, Helper *helper, SharedPtr<Core::Notices::Notice> &notice
  );

  public: static TiObject* getTemplateVar(Block const *instance, Char const *name);

  private: static TiObject* traceObject(TiObject *ref, VarType varType, Helper *helper);


  //============================================================================
  // Clonable Implementation

  public: virtual SharedPtr<TiObject> clone() const;


  //============================================================================
  // Printable Implementation

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} } // namespace

#endif

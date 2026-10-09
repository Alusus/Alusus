/**
 * @file Spp/Ast/Template.h
 * Contains the header of class Spp::Ast::Template.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_TEMPLATE_H
#define SPP_AST_TEMPLATE_H

namespace Spp::Ast
{

class Template : public Core::Ast::Node,
                 public MapContaining<Core::Ast::Node>, public Core::Ast::Mergeable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Template, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, MapContaining<Core::Ast::Node>, Core::Ast::Mergeable
  );
  OBJECT_FACTORY(Template);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::List> varDefs;

  private: SharedPtr<Core::Ast::Node> body;

  private: SharedList<Core::Ast::Scope> instances;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (varDefs, Core::Ast::List, SHARED_REF, setVarDefs(value), varDefs.get()),
    (body, Core::Ast::Node, SHARED_REF, setBody(value), body.get())
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

  public: void setVarDefs(SharedPtr<Core::Ast::List> const &defs)
  {
    UPDATE_OWNED_SHAREDPTR(this->varDefs, defs);
  }
  private: void setVarDefs(Core::Ast::List *defs)
  {
    this->setVarDefs(getSharedPtr(defs));
  }

  public: SharedPtr<Core::Ast::List> const& getVarDefs() const
  {
    return this->varDefs;
  }

  public: Word getVarDefCount() const
  {
    return this->varDefs == 0 ? 0 : this->varDefs->getCount();
  }

  public: void setBody(SharedPtr<Core::Ast::Node> const &b)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, b);
  }
  private: void setBody(Core::Ast::Node *b)
  {
    this->setBody(getSharedPtr(b));
  }

  public: SharedPtr<Core::Ast::Node> const& getBody() const
  {
    return this->body;
  }

  public: virtual SharedPtr<Core::Ast::Node> const& getDefaultInstance(Helper *helper);
  /**
   * @brief Find or create the instance matching the given inputs.
   * On failure the notice will be set to the reason of the failure, if available.
   */
  public: virtual Bool matchInstance(
    Core::Ast::Node *templateInputs, Helper *helper, SharedPtr<Core::Ast::Node> &result,
    SharedPtr<Core::Notices::Notice> &notice
  );

  private: Bool prepareTemplateVars(
    Core::Ast::Node *templateInputs, Helper *helper, PlainList<Core::Ast::Node> *vars,
    SharedPtr<Core::Notices::Notice> &notice
  );

  private: Bool matchTemplateVars(
    Containing<Core::Ast::Node> *templateInputs, Core::Ast::Scope *instance, Helper *helper,
    SharedPtr<Core::Notices::Notice> &notice
  );
  private: Bool matchTemplateVar(
    Core::Ast::Node *templateInput, Core::Ast::Scope *instance, TemplateVarDef *varDef, Helper *helper,
    SharedPtr<Core::Notices::Notice> &notice
  );

  private: Bool assignTemplateVars(
    Containing<Core::Ast::Node> *templateInputs, Core::Ast::Scope *instance, Helper *helper,
    SharedPtr<Core::Notices::Notice> &notice
  );

  public: static Core::Ast::Node* getTemplateVar(Core::Ast::Scope const *instance, Char const *name);

  private: static Core::Ast::Node* traceObject(
    Core::Ast::Node *ref, TemplateVarType varType, Helper *helper
  );

  public: Word getInstanceCount() const
  {
    return this->instances.getCount();
  }

  public: SharedPtr<Core::Ast::Scope> const& getInstance(Int index)
  {
    return this->instances.get(index);
  }


  //============================================================================
  // Mergeable Implementation

  public: virtual Bool merge(Core::Ast::Node *src, Core::Ast::Seeker *seeker, Core::Notices::Store *noticeStore);


  //============================================================================
  // Printing Functions

  public: virtual void print(OutStream &stream, Int indents=0) const;

}; // class

} // namespace

#endif

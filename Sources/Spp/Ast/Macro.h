/**
 * @file Spp/Ast/Macro.h
 * Contains the header of class Spp::Ast::Macro.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_MACRO_H
#define SPP_AST_MACRO_H

namespace Spp::Ast
{

class Macro : public Core::Ast::Node,
              public Binding, public MapContaining<Core::Ast::Node>,
              public Core::Ast::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Macro, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(
      Binding,
      MapContaining<Core::Ast::Node>,
      Core::Ast::Printable
    )
  ));
  OBJECT_FACTORY(Macro);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Map> argTypes;
  private: SharedPtr<Core::Ast::Node> body;
  private: TiBool member;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (member, TiBool, VALUE, setMember(value), &member),
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (argTypes, Core::Ast::Map, SHARED_REF, setArgTypes(value), argTypes.get()),
    (body, Core::Ast::Node, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(Macro);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Macro);

  IMPLEMENT_ATTR_CONSTRUCTOR(Macro);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(Macro);

  public: virtual ~Macro()
  {
    DISOWN_SHAREDPTR(this->argTypes);
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setArgTypes(SharedPtr<Core::Ast::Map> const &args)
  {
    UPDATE_OWNED_SHAREDPTR(this->argTypes, args);
  }
  private: void setArgTypes(Core::Ast::Map *args)
  {
    this->setArgTypes(getSharedPtr(args));
  }

  public: SharedPtr<Core::Ast::Map> const& getArgTypes() const
  {
    return this->argTypes;
  }

  public: Word getArgCount() const
  {
    return this->argTypes == 0 ? 0 : this->argTypes->getCount();
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

  public: Bool matchCall(Containing<Core::Ast::Node> *args, Helper *helper)
  {
    VALIDATE_NOT_NULL(args);
    // TODO: Match arg types as well.
    return args->getElementCount() == this->getArgCount();
  }

  public: void setMember(Bool m)
  {
    this->member = m;
  }
  public: void setMember(TiBool const *m)
  {
    this->member = m == 0 ? false : m->get();
  }

  public: Bool isMember() const
  {
    return this->member.get();
  }

}; // class

} // namespace

#endif

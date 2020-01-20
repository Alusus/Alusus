/**
 * @file Spp/Ast/Macro.h
 * Contains the header of class Spp::Ast::Macro.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_MACRO_H
#define SPP_AST_MACRO_H

namespace Spp::Ast
{

class Macro : public Core::Data::Node,
              public Binding, public MapContaining<TiObject>,
              public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Macro, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(
      Binding,
      MapContaining<TiObject>,
      Core::Data::Ast::MetaHaving,
      Core::Data::Printable
    )
  ));
  OBJECT_FACTORY(Macro);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Data::Ast::Map> argTypes;
  private: TioSharedPtr body;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Macro);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (argTypes, Core::Data::Ast::Map, SHARED_REF, setArgTypes(value), argTypes.get()),
    (body, TiObject, SHARED_REF, setBody(value), body.get())
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

  public: void setArgTypes(SharedPtr<Core::Data::Ast::Map> const &args)
  {
    UPDATE_OWNED_SHAREDPTR(this->argTypes, args);
  }
  private: void setArgTypes(Core::Data::Ast::Map *args)
  {
    this->setArgTypes(getSharedPtr(args));
  }

  public: SharedPtr<Core::Data::Ast::Map> const& getArgTypes() const
  {
    return this->argTypes;
  }

  public: Word getArgCount() const
  {
    return this->argTypes == 0 ? 0 : this->argTypes->getCount();
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

  public: Bool matchCall(Containing<TiObject> *args, Helper *helper)
  {
    VALIDATE_NOT_NULL(args);
    // TODO: Match arg types as well.
    return args->getElementCount() == this->getArgCount();
  }

}; // class

} // namespace

#endif

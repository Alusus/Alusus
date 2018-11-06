/**
 * @file Spp/Ast/Macro.h
 * Contains the header of class Spp::Ast::Macro.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_MACRO_H
#define SPP_AST_MACRO_H

namespace Spp::Ast
{

class Macro : public Core::Data::Node,
              public virtual Binding, public virtual ExMapContaining<TiObject>,
              public virtual Core::Data::Ast::MetaHaving,
              public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Macro, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(
      Binding,
      ExMapContaining<TiObject>,
      Core::Data::Ast::MetaHaving,
      Core::Data::Clonable,
      Core::Data::Printable
    )
  ));


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

  IMPLEMENT_MAP_CONTAINING(ExMapContaining<TiObject>,
    (argTypes, Core::Data::Ast::Map, SHARED_REF, setArgTypes(value), argTypes.get()),
    (body, TiObject, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(Macro);

  IMPLEMENT_AST_MAP_CLONABLE(Macro);


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

}; // class

} // namespace

#endif

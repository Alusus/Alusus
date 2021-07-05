/**
 * @file Spp/Ast/AstLiteralCommand.h
 * Contains the header of class Spp::Ast::AstLiteralCommand.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_ASTLITERALCOMMAND_H
#define SPP_AST_ASTLITERALCOMMAND_H

namespace Spp::Ast
{

class AstLiteralCommand : public Core::Data::Node,
                          public Binding, public MapContaining<TiObject>,
                          public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(AstLiteralCommand, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(AstLiteralCommand);


  //============================================================================
  // Member Variables

  private: TioSharedPtr body;
  private: TiBool preprocessDisabled;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(AstLiteralCommand);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get()),
    (preprocessDisabled, TiBool, VALUE, setPreprocessDisabled(value), &preprocessDisabled)
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>, (body, TiObject, SHARED_REF, setBody(value), body.get()));

  IMPLEMENT_AST_MAP_PRINTABLE(AstLiteralCommand);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(AstLiteralCommand);

  IMPLEMENT_ATTR_CONSTRUCTOR(AstLiteralCommand);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(AstLiteralCommand);

  public: virtual ~AstLiteralCommand()
  {
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

  public: void setBody(TioSharedPtr const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, o);
  }
  private: void setBody(TiObject *o)
  {
    this->setBody(getSharedPtr(o));
  }

  public: TioSharedPtr const& getBody() const
  {
    return this->body;
  }

  public: void setPreprocessDisabled(Bool d)
  {
    this->preprocessDisabled = d;
  }
  public: void setPreprocessDisabled(TiBool const *d)
  {
    this->preprocessDisabled = d == 0 ? false : d->get();
  }

  public: Bool isPreprocessDisabled() const
  {
    return this->preprocessDisabled.get();
  }

}; // class

} // namespace

#endif

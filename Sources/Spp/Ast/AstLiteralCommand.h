/**
 * @file Spp/Ast/AstLiteralCommand.h
 * Contains the header of class Spp::Ast::AstLiteralCommand.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
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

class AstLiteralCommand : public Core::Ast::Node,
                          public Binding, public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(AstLiteralCommand, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, Binding, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(AstLiteralCommand);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::Node> body;
  private: TiBool preprocessDisabled;


  //============================================================================
  // Implementations

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Ast::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get()),
    (preprocessDisabled, TiBool, VALUE, setPreprocessDisabled(value), &preprocessDisabled)
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>, (body, Core::Ast::Node, SHARED_REF, setBody(value), body.get()));

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

  public: void setBody(SharedPtr<Core::Ast::Node> const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->body, o);
  }
  private: void setBody(Core::Ast::Node *o)
  {
    this->setBody(getSharedPtr(o));
  }

  public: SharedPtr<Core::Ast::Node> const& getBody() const
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

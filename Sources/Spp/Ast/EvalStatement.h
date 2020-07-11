/**
 * @file Spp/Ast/EvalStatement.h
 * Contains the header of class Spp::Ast::EvalStatement.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_EVALSTATEMENT_H
#define SPP_AST_EVALSTATEMENT_H

namespace Spp::Ast
{

class EvalStatement : public Core::Data::Node,
                      public Binding, public MapContaining<TiObject>,
                      public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(EvalStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(EvalStatement);


  //============================================================================
  // Member Variables

  private: TioSharedPtr body;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(EvalStatement);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (body, TiObject, SHARED_REF, setBody(value), body.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(EvalStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(EvalStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(EvalStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(EvalStatement);

  public: virtual ~EvalStatement()
  {
    DISOWN_SHAREDPTR(this->body);
  }


  //============================================================================
  // Member Functions

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

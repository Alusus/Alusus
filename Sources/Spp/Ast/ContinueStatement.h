/**
 * @file Spp/Ast/ContinueStatement.h
 * Contains the header of class Spp::Ast::ContinueStatement.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_CONTINUESTATEMENT_H
#define SPP_AST_CONTINUESTATEMENT_H

namespace Spp::Ast
{

class ContinueStatement : public Core::Data::Node,
                          public Binding, public MapContaining<TiObject>,
                          public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(ContinueStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Data::Node, Binding, MapContaining<TiObject>,
    Core::Data::Ast::MetaHaving, Core::Data::Printable
  );
  OBJECT_FACTORY(ContinueStatement);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Data::Ast::IntegerLiteral> steps;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(ContinueStatement);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (steps, Core::Data::Ast::IntegerLiteral, SHARED_REF, setSteps(value), steps.get())
  );

  IMPLEMENT_AST_MAP_PRINTABLE(ContinueStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ContinueStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(ContinueStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ContinueStatement);

  public: virtual ~ContinueStatement()
  {
    DISOWN_SHAREDPTR(this->steps);
  }


  //============================================================================
  // Member Functions

  public: void setSteps(SharedPtr<Core::Data::Ast::IntegerLiteral> const &s)
  {
    UPDATE_OWNED_SHAREDPTR(this->steps, s);
  }
  private: void setSteps(Core::Data::Ast::IntegerLiteral *s)
  {
    this->setSteps(getSharedPtr(s));
  }

  public: SharedPtr<Core::Data::Ast::IntegerLiteral> const& getSteps() const
  {
    return this->steps;
  }

}; // class

} // namespace

#endif

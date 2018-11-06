/**
 * @file Spp/Ast/BreakStatement.h
 * Contains the header of class Spp::Ast::BreakStatement.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_BREAKSTATEMENT_H
#define SPP_AST_BREAKSTATEMENT_H

namespace Spp::Ast
{

class BreakStatement : public Core::Data::Node,
                       public virtual Binding, public virtual ExMapContaining<TiObject>,
                       public virtual Core::Data::Ast::MetaHaving, public virtual Core::Data::Clonable,
                       public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(BreakStatement, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Binding, ExMapContaining<TiObject>,
                                         Core::Data::Ast::MetaHaving, Core::Data::Clonable,
                                         Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Data::Ast::IntegerLiteral> steps;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(BreakStatement);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(ExMapContaining<TiObject>,
    (steps, Core::Data::Ast::IntegerLiteral, SHARED_REF, setSteps(value), steps.get())
  );

  IMPLEMENT_AST_CLONABLE(BreakStatement);

  IMPLEMENT_AST_MAP_PRINTABLE(BreakStatement);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(BreakStatement);

  IMPLEMENT_ATTR_CONSTRUCTOR(BreakStatement);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(BreakStatement);

  public: virtual ~BreakStatement()
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

/**
 * @file Spp/Ast/BreakStatement.h
 * Contains the header of class Spp::Ast::BreakStatement.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_BREAKSTATEMENT_H
#define SPP_AST_BREAKSTATEMENT_H

namespace Spp::Ast
{

class BreakStatement : public Core::Ast::Node,
                       public MapContaining<Core::Ast::Node>
{
  //============================================================================
  // Type Info

  TYPE_INFO(BreakStatement, Core::Ast::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(
    Core::Ast::Node, MapContaining<Core::Ast::Node>
  );
  OBJECT_FACTORY(BreakStatement);


  //============================================================================
  // Member Variables

  private: SharedPtr<Core::Ast::IntegerLiteral> steps;


  //============================================================================
  // Implementations

  IMPLEMENT_MAP_CONTAINING(MapContaining<Core::Ast::Node>,
    (steps, Core::Ast::IntegerLiteral, SHARED_REF, setSteps(value), steps.get())
  );


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

  public: void setSteps(SharedPtr<Core::Ast::IntegerLiteral> const &s)
  {
    UPDATE_OWNED_SHAREDPTR(this->steps, s);
  }
  private: void setSteps(Core::Ast::IntegerLiteral *s)
  {
    this->setSteps(getSharedPtr(s));
  }

  public: SharedPtr<Core::Ast::IntegerLiteral> const& getSteps() const
  {
    return this->steps;
  }

}; // class

} // namespace

#endif

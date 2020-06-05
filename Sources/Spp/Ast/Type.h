/**
 * @file Spp/Ast/Type.h
 * Contains the header of class Spp::Ast::Type.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_TYPE_H
#define SPP_AST_TYPE_H

namespace Spp::Ast
{

using namespace Core;

class Type : public Core::Data::Node,
             public Binding, public Core::Data::Ast::MetaHaving, public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Type, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(
      Binding,
      Core::Data::Ast::MetaHaving,
      Core::Data::Printable
    )
  ));


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(Type);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );


  //============================================================================
  // Constructor / Destructor

  public: virtual ~Type()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual TypeMatchStatus matchTargetType(
    Type const *type, Helper *helper, ExecutionContext const *ec
  ) const = 0;

  public: virtual Bool isEqual(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    return this->matchTargetType(type, helper, ec) == TypeMatchStatus(TypeMatchStatus::EXACT);
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    return this->matchTargetType(type, helper, ec) >= TypeMatchStatus::IMPLICIT_CAST;
  }

  public: virtual Bool isExplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    return this->matchTargetType(type, helper, ec) >= TypeMatchStatus::EXPLICIT_CAST;
  }

  public: virtual Bool hasCustomInitialization(Helper *helper, ExecutionContext const *ec) const
  {
    return false;
  }

  public: virtual Bool hasCustomDestruction(Helper *helper, ExecutionContext const *ec) const
  {
    return false;
  }

}; // class

} // namespace

#endif

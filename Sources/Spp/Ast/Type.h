/**
 * @file Spp/Ast/Type.h
 * Contains the header of class Spp::Ast::Type.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_TYPE_H
#define SPP_AST_TYPE_H

namespace Spp::Ast
{

using namespace Core;

class Type : public Core::Data::Node,
             public virtual Binding, public virtual Core::Data::Ast::MetaHaving,
             public virtual Core::Data::Clonable, public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Type, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net", (
    INHERITANCE_INTERFACES(
      Binding,
      Core::Data::Ast::MetaHaving,
      Core::Data::Clonable,
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
    return this->matchTargetType(type, helper, ec) == TypeMatchStatus::EXACT;
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    return this->matchTargetType(type, helper, ec) >= TypeMatchStatus::IMPLICIT_CAST;
  }

  public: virtual Bool isExplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    return this->matchTargetType(type, helper, ec) >= TypeMatchStatus::EXPLICIT_CAST;
  }

}; // class

} // namespace

#endif

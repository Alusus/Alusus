/**
 * @file Spp/Ast/ValueType.h
 * Contains the header of class Spp::Ast::ValueType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_VALUETYPE_H
#define SPP_AST_VALUETYPE_H

namespace Spp { namespace Ast
{

using namespace Core;

class ValueType : public Core::Data::Node, public virtual Core::Data::IdHolder
{
  //============================================================================
  // Type Info

  TYPE_INFO(ValueType, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES_1(Core::Data::Node, Core::Data::IdHolder);


  //============================================================================
  // Constructors & Destructor

  protected: ValueType()
  {
  }

  public: virtual ~ValueType()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual Str const& getName() const
  {
    return ID_GENERATOR->getDesc(this->getId());
  }

  /**
   * Determines whether this type is equal to the given type.
   * @param[in] other The type.
   * @return @c true or @c false.
   */
  public: virtual bool isEqualTo(ValueType const *other, ExecutionContext const *context) const
  {
    return this->getId() == other->getId();
  }

  /**
   * Determines whether this type can be implicitly casted to the given type.
   * @param[in] type The type.
   * @return @c true or @c false.
   */
  public: virtual bool isImplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const = 0;

  /**
   * Determines whether this type can be explicitly casted to the given type.
   * @param[in] type The type.
   * @return @c true or @c false.
   */
  public: virtual bool isExplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const = 0;

  /**
   * Compares this type against the given type.
   * @param[in] other The type to compare against.
   * @return One of the results of Scg::TypeComparisonResult.
   */
  public: virtual TypeComparisonResult compare(ValueType const *other, ExecutionContext const *context)
  {
    if (isEqualTo(other, context)) {
      return TypeComparisonResult::Equivalent;
    } else if (isImplicitlyCastableTo(other, context)) {
      return TypeComparisonResult::ImplicitlyEquivalent;
    } else if (isExplicitlyCastableTo(other, context)) {
      return TypeComparisonResult::ExplicitlyEquivalent;
    } else {
      return TypeComparisonResult::NotEquivalent;
    }
  }

}; // class

} } // namespace

#endif

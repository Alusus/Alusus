/**
 * @file Spp/Ast/IntegerType.h
 * Contains the header of class Spp::Ast::IntegerType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_INTEGERTYPE_H
#define SPP_AST_INTEGERTYPE_H

namespace Spp { namespace Ast
{

//! Represent the integer type.
class IntegerType : public ValueType
{
  //============================================================================
  // Type Info

  TYPE_INFO(IntegerType, ValueType, "Spp.Ast", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: Int bitCount;


  //============================================================================
  // Construtors & Destructor

  //! Constructs an integer type.
  public: IntegerType(Int bits) : bitCount(bits)
  {
  }

  public: virtual ~IntegerType()
  {
  }


  //============================================================================
  // Member Functions

  public: Int getBitCount() const
  {
    return this->bitCount;
  }

  public: virtual bool isImplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const;

  public: virtual bool isExplicitlyCastableTo(ValueType const *type, ExecutionContext const *context) const;

}; // class

} } // namespace

#endif

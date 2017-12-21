/**
 * @file Spp/Ast/ArrayType.h
 * Contains the header of class Spp::Ast::ArrayType.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_ARRAYTYPE_H
#define SPP_AST_ARRAYTYPE_H

namespace Spp { namespace Ast
{

class ArrayType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArrayType, Type, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(ArrayType);

  IMPLEMENT_AST_MAP_PRINTABLE(ArrayType);


  //============================================================================
  // Member Variables

  private: mutable TioSharedPtr contentTypeRef;
  private: mutable TioSharedPtr sizeRef;


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ArrayType);

  IMPLEMENT_ATTR_CONSTRUCTOR(ArrayType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ArrayType);


  //============================================================================
  // Member Functions

  public: Type* getContentType(Helper *helper) const;

  public: Word getSize(Helper *helper) const;

  public: virtual Bool isImplicitlyCastableTo(Type const *type, Helper *helper) const
  {
    return this == type;
  }

  public: virtual Bool isExplicitlyCastableTo(Type const *type, Helper *helper) const
  {
    return this == type;
  }

}; // class

} } // namespace

#endif

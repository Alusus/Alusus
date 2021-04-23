/**
 * @file Spp/Ast/ArrayType.h
 * Contains the header of class Spp::Ast::ArrayType.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_ARRAYTYPE_H
#define SPP_AST_ARRAYTYPE_H

namespace Spp::Ast
{

class ArrayType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArrayType, DataType, "Spp.Ast", "Spp", "alusus.org");
  OBJECT_FACTORY(ArrayType);

  IMPLEMENT_AST_MAP_PRINTABLE(ArrayType);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ArrayType);

  IMPLEMENT_ATTR_CONSTRUCTOR(ArrayType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ArrayType);


  //============================================================================
  // Member Functions

  public: Type* getContentType(Helper *helper) const;

  public: Word getSize(Helper *helper) const;

  public: virtual TypeMatchStatus matchTargetType(
    Type const *type, Helper *helper, ExecutionContext const *ec, TypeMatchOptions opts = TypeMatchOptions::NONE
  ) const;

}; // class

} // namespace

#endif

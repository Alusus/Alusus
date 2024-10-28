/**
 * @file Spp/Ast/PointerType.h
 * Contains the header of class Spp::Ast::PointerType.
 *
 * @copyright Copyright (C) 2024 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_POINTERTYPE_H
#define SPP_AST_POINTERTYPE_H

namespace Spp::Ast
{

class PointerType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(PointerType, DataType, "Spp.Ast", "Spp", "alusus.org");
  OBJECT_FACTORY(PointerType);

  IMPLEMENT_AST_MAP_PRINTABLE(PointerType);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(PointerType);

  IMPLEMENT_ATTR_CONSTRUCTOR(PointerType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(PointerType);


  //============================================================================
  // Member Functions

  public: Type* getContentType(Helper *helper) const;

  public: virtual TypeMatchStatus matchTargetType(
    Type const *type, Helper *helper, TypeMatchOptions opts = TypeMatchOptions::NONE
  ) const;

  public: virtual Bool isIdentical(Type const *type, Helper *helper) const;

}; // class

} // namespace

#endif

/**
 * @file Spp/Ast/VoidType.h
 * Contains the header of class Spp::Ast::VoidType.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_VOIDTYPE_H
#define SPP_AST_VOIDTYPE_H

namespace Spp::Ast
{

class VoidType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(VoidType, DataType, "Spp.Ast", "Spp", "alusus.org");
  OBJECT_FACTORY(VoidType);

  IMPLEMENT_AST_MAP_PRINTABLE(VoidType);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(VoidType);

  IMPLEMENT_ATTR_CONSTRUCTOR(VoidType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(VoidType);


  //============================================================================
  // Member Functions

  public: virtual TypeMatchStatus matchTargetType(
    Type const *type, Helper *helper, ExecutionContext const *ec, TypeMatchOptions opts = TypeMatchOptions::NONE
  ) const
  {
    if (this == type || type->isDerivedFrom<VoidType>()) return TypeMatchStatus::EXACT;
    else return TypeMatchStatus::NONE;
  }

}; // class

} // namespace

#endif

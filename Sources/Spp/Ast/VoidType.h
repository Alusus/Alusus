/**
 * @file Spp/Ast/VoidType.h
 * Contains the header of class Spp::Ast::VoidType.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
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

  TYPE_INFO(VoidType, DataType, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(VoidType);

  IMPLEMENT_AST_MAP_PRINTABLE(VoidType);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(VoidType);

  IMPLEMENT_ATTR_CONSTRUCTOR(VoidType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(VoidType);


  //============================================================================
  // Member Functions

  public: virtual Bool isEqual(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    if (this == type || type->isDerivedFrom<VoidType>()) return true;
    else return false;
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    if (this == type || type->isDerivedFrom<VoidType>()) return true;
    else return false;
  }

  public: virtual Bool isExplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    if (this == type || type->isDerivedFrom<VoidType>()) return true;
    else return false;
  }

}; // class

} // namespace

#endif

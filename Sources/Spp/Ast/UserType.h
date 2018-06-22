/**
 * @file Spp/Ast/UserType.h
 * Contains the header of class Spp::Ast::UserType.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_USERTYPE_H
#define SPP_AST_USERTYPE_H

namespace Spp::Ast
{

class UserType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(UserType, DataType, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(UserType);

  IMPLEMENT_AST_MAP_PRINTABLE(UserType);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(UserType);

  IMPLEMENT_ATTR_CONSTRUCTOR(UserType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(UserType);


  //============================================================================
  // Member Functions

  public: virtual Bool isEqual(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    if (this == type) return true;
    else return false;
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    if (this == type) return true;
    else return false;
  }

  public: virtual Bool isExplicitlyCastableTo(Type const *type, Helper *helper, ExecutionContext const *ec) const
  {
    if (this == type) return true;
    else return false;
  }

}; // class

} // namespace

#endif

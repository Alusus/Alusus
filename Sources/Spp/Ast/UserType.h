/**
 * @file Spp/Ast/UserType.h
 * Contains the header of class Spp::Ast::UserType.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_USERTYPE_H
#define SPP_AST_USERTYPE_H

namespace Spp::Ast
{

class UserType : public DataType, public Core::Data::Ast::Mergeable
{
  //============================================================================
  // Type Info

  TYPE_INFO(UserType, DataType, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(Core::Data::Ast::Mergeable)
  ));
  OBJECT_FACTORY(UserType);

  IMPLEMENT_AST_MAP_PRINTABLE(UserType);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(UserType);

  IMPLEMENT_ATTR_CONSTRUCTOR(UserType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(UserType);


  //============================================================================
  // Member Functions

  public: virtual TypeMatchStatus matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const;

  public: virtual Bool hasCustomInitialization(Helper *helper, ExecutionContext const *ec) const;

  public: virtual Bool hasCustomDestruction(Helper *helper, ExecutionContext const *ec) const;


  //============================================================================
  // Mergeable Implementation

  public: virtual Bool merge(TiObject *src, Core::Notices::Store *noticeStore);

}; // class

} // namespace

#endif

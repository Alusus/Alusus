/**
 * @file Spp/Ast/PointerType.h
 * Contains the header of class Spp::Ast::PointerType.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_POINTERTYPE_H
#define SPP_AST_POINTERTYPE_H

namespace Spp { namespace Ast
{

class PointerType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(PointerType, Type, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(PointerType);

  IMPLEMENT_AST_MAP_PRINTABLE(PointerType);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(PointerType);

  IMPLEMENT_ATTR_CONSTRUCTOR(PointerType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(PointerType);


  //============================================================================
  // Member Functions

  public: Type* getContentType(Core::Data::Seeker *seeker) const;

  public: virtual Bool isImplicitlyCastableTo(
    Type const *type, ExecutionContext const *context, Core::Data::Seeker *seeker
  ) const;

  public: virtual Bool isExplicitlyCastableTo(
    Type const *type, ExecutionContext const *context, Core::Data::Seeker *seeker
  ) const;

}; // class

} } // namespace

#endif

/**
 * @file Spp/Ast/ReferenceType.h
 * Contains the header of class Spp::Ast::ReferenceType.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_REFERENCETYPE_H
#define SPP_AST_REFERENCETYPE_H

namespace Spp::Ast
{

class ReferenceType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(ReferenceType, DataType, "Spp.Ast", "Spp", "alusus.org");
  OBJECT_FACTORY(ReferenceType);

  IMPLEMENT_AST_MAP_PRINTABLE(ReferenceType);


  //============================================================================
  // Member Variables

  private: mutable TioSharedPtr contentTypeRef;


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ReferenceType);

  IMPLEMENT_ATTR_CONSTRUCTOR(ReferenceType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(ReferenceType);


  //============================================================================
  // Member Functions

  public: Type* getContentType(Helper *helper) const;

  public: virtual TypeMatchStatus matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const;

}; // class

} // namespace

#endif

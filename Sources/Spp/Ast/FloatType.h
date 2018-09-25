/**
 * @file Spp/Ast/FloatType.h
 * Contains the header of class Spp::Ast::FloatType.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_FLOATTYPE_H
#define SPP_AST_FLOATTYPE_H

namespace Spp::Ast
{

class FloatType : public DataType
{
  //============================================================================
  // Type Info

  TYPE_INFO(FloatType, DataType, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_MAP_CLONABLE(FloatType);

  IMPLEMENT_AST_MAP_PRINTABLE(FloatType);


  //============================================================================
  // Member Variables

  private: mutable TioSharedPtr bitCountRef;


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(FloatType);

  IMPLEMENT_ATTR_CONSTRUCTOR(FloatType);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(FloatType);


  //============================================================================
  // Member Functions

  public: Word getBitCount(Helper *helper) const;

  public: virtual TypeMatchStatus matchTargetType(Type const *type, Helper *helper, ExecutionContext const *ec) const;

}; // class

} // namespace

#endif

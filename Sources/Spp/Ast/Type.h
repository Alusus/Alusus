/**
 * @file Spp/Ast/Type.h
 * Contains the header of class Spp::Ast::Type.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_TYPE_H
#define SPP_AST_TYPE_H

namespace Spp { namespace Ast
{

using namespace Core;

class Type : public Block
{
  //============================================================================
  // Type Info

  TYPE_INFO(Type, Block, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(Type);

  IMPLEMENT_AST_LIST_PRINTABLE(Type);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Type);

  IMPLEMENT_ATTR_CONSTRUCTOR(Type);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(Type);

}; // class

} } // namespace

#endif

/**
 * @file Spp/Ast/Block.h
 * Contains the header of class Spp::Ast::Block.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_BLOCK_H
#define SPP_AST_BLOCK_H

namespace Spp { namespace Ast
{

using namespace Core;

class Block : public Core::Data::Ast::Scope
{
  //============================================================================
  // Type Info

  TYPE_INFO(Block, Core::Data::Ast::Scope, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(Block);

  IMPLEMENT_AST_LIST_PRINTABLE(Block);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Block);

  IMPLEMENT_ATTR_CONSTRUCTOR(Block);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(Block);

}; // class

} } // namespace

#endif

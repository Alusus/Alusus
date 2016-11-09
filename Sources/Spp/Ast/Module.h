/**
 * @file Spp/Ast/Module.h
 * Contains the header of class Spp::Ast::Module.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_MODULE_H
#define SPP_AST_MODULE_H

namespace Spp { namespace Ast
{

using namespace Core;

class Module : public Block
{
  //============================================================================
  // Type Info

  TYPE_INFO(Module, Block, "Spp.Ast", "Spp", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(Module);

  IMPLEMENT_AST_LIST_PRINTABLE(Module);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Module);

  IMPLEMENT_ATTR_CONSTRUCTOR(Module);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(Module);

}; // class

} } // namespace

#endif

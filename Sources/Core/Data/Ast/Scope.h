/**
 * @file Core/Data/Ast/Scope.h
 * Contains the header of class Core::Data::Ast::Scope.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_SCOPE_H
#define CORE_DATA_AST_SCOPE_H

namespace Core { namespace Data { namespace Ast
{

class Scope : public StatementList
{
  //============================================================================
  // Type Info

  TYPE_INFO(Scope, StatementList, "Core.Data.Ast", "Core", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(Scope);

  IMPLEMENT_AST_LIST_PRINTABLE(Scope);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Scope);

  IMPLEMENT_ATTR_CONSTRUCTOR(Scope);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(Scope);

}; // class

} } } // namespace

#endif

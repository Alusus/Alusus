/**
 * @file Core/Data/Ast/StatementList.h
 * Contains the header of class Core::Data::Ast::StatementList.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_STATEMENTLIST_H
#define CORE_DATA_AST_STATEMENTLIST_H

namespace Core { namespace Data { namespace Ast
{

class StatementList : public List
{
  //============================================================================
  // Type Info

  TYPE_INFO(StatementList, List, "Core.Data.Ast", "Core", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(StatementList);

  IMPLEMENT_AST_LIST_PRINTABLE(StatementList);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(StatementList);

  IMPLEMENT_ATTR_CONSTRUCTOR(StatementList);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(StatementList);

}; // class

} } } // namespace

#endif

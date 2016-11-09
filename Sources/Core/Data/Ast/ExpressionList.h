/**
 * @file Core/Data/Ast/ExpressionList.h
 * Contains the header of class Core::Data::Ast::ExpressionList.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_EXPRESSIONLIST_H
#define CORE_DATA_AST_EXPRESSIONLIST_H

namespace Core { namespace Data { namespace Ast
{

class ExpressionList : public List
{
  //============================================================================
  // Type Info

  TYPE_INFO(ExpressionList, List, "Core.Data.Ast", "Core", "alusus.net");

  IMPLEMENT_AST_LIST_CLONABLE(ExpressionList);

  IMPLEMENT_AST_LIST_PRINTABLE(ExpressionList);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(ExpressionList);

  IMPLEMENT_ATTR_CONSTRUCTOR(ExpressionList);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(ExpressionList);

}; // class

} } } // namespace

#endif

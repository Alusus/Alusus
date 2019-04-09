/**
 * @file Core/Data/Ast/MergeList.h
 * Contains the header of class Core::Data::Ast::MergeList.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_MERGELIST_H
#define CORE_DATA_AST_MERGELIST_H

namespace Core::Data::Ast
{

class MergeList : public List
{
  //============================================================================
  // Type Info

  TYPE_INFO(MergeList, List, "Core.Data.Ast", "Core", "alusus.net");


  //============================================================================
  // Implementations

  IMPLEMENT_AST_LIST_CLONABLE(MergeList);

  IMPLEMENT_AST_LIST_PRINTABLE(MergeList);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(MergeList);

  IMPLEMENT_ATTR_CONSTRUCTOR(MergeList);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(MergeList);

}; // class

} // namespace

#endif
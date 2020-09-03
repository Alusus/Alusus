/**
 * @file Core/Data/Ast/Mergeable.h
 * Contains the header of interface Data::Ast::Mergeable.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_DATA_AST_MERGEABLE_H
#define CORE_DATA_AST_MERGEABLE_H

namespace Core::Data::Ast
{

// TODO: DOC

class Mergeable : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Mergeable, TiInterface, "Core.Data.Ast", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual Bool merge(TiObject *src, Seeker *seeker, Notices::Store *noticeStore) = 0;

}; // class

} // namespace

#endif

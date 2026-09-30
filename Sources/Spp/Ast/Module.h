/**
 * @file Spp/Ast/Module.h
 * Contains the header of class Spp::Ast::Module.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_AST_MODULE_H
#define SPP_AST_MODULE_H

namespace Spp::Ast
{

using namespace Core;

class Module : public Core::Ast::Scope, public Core::Ast::Mergeable
{
  //============================================================================
  // Type Info

  TYPE_INFO(Module, Core::Ast::Scope, "Spp.Ast", "Spp", "alusus.org", (
    INHERITANCE_INTERFACES(Core::Ast::Mergeable)
  ));
  OBJECT_FACTORY(Module);


  //============================================================================
  // Constructor / Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(Module);

  IMPLEMENT_ATTR_CONSTRUCTOR(Module);

  IMPLEMENT_ATTR_LIST_CONSTRUCTOR(Module);


  //============================================================================
  // Mergeable Implementation

  public: virtual Bool merge(Core::Ast::Node *src, Core::Ast::Seeker *seeker, Core::Notices::Store *noticeStore);

}; // class

} // namespace

#endif

/**
 * @file Spp/Grammar/tools.h
 * Contains declarations of grammar utility functions.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_GRAMMAR_TOOLS_H
#define SPP_GRAMMAR_TOOLS_H

namespace Spp::Grammar
{

/// @ingroup spp_ast
Bool parseCommandSection(
  Core::Ast::Node *ast, Core::Grammar::CommandSection &section, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseCommandKeywords(
  Core::Ast::Node *ast, SharedPtr<Core::Grammar::Map> &keywords, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseCommandArg(
  Core::Ast::Node *ast, Core::Grammar::CommandArg &arg, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseMinMax(
  Core::Ast::Node *ast, Core::Ast::Node *&resultAst, SharedPtr<TiInt> &min, SharedPtr<TiInt> &max, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseQualifier(
  Core::Ast::Node *ast, Str &qualifier, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
void convertInfixOpIntoList(Core::Ast::Node *ast, Char const *op, Array<Core::Ast::Node*> &list);

/// @ingroup spp_ast
Bool overrideTree(
  TiObject *target, Str baseRefQualifier, Core::Ast::Node *qualifierAst, Core::Ast::Node *valueAst,
  Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool cloneChain(
  TiObject *target, Core::Ast::Node *qualifier, Str &baseRefQualifier, Core::Notices::Store *noticeStore, TiObject *&result
);

/// @ingroup spp_ast
Bool parseValueAst(Core::Ast::Node *valueAst, Core::Notices::Store *noticeStore, TioSharedPtr &result);

} // namespace

#endif

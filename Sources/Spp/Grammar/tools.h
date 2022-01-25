/**
 * @file Spp/Grammar/tools.h
 * Contains declarations of grammar utility functions.
 *
 * @copyright Copyright (C) 2022 Sarmad Khalid Abdullah
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
  TiObject *ast, Core::Data::Grammar::CommandSection &section, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseCommandKeywords(
  TiObject *ast, SharedPtr<Core::Data::Grammar::Map> &keywords, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseCommandArg(
  TiObject *ast, Core::Data::Grammar::CommandArg &arg, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseMinMax(
  TiObject *ast, TiObject *&resultAst, SharedPtr<TiInt> &min, SharedPtr<TiInt> &max, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool parseQualifier(
  TiObject *ast, Str &qualifier, Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
void convertInfixOpIntoList(TiObject *ast, Char const *op, Array<TiObject*> &list);

/// @ingroup spp_ast
Bool overrideTree(
  TiObject *target, Str baseRefQualifier, TiObject *qualifierAst, TiObject *valueAst,
  Core::Notices::Store *noticeStore
);

/// @ingroup spp_ast
Bool cloneChain(
  TiObject *target, TiObject *qualifier, Str &baseRefQualifier, Core::Notices::Store *noticeStore, TiObject *&result
);

/// @ingroup spp_ast
Bool parseValueAst(TiObject *valueAst, Core::Notices::Store *noticeStore, TioSharedPtr &result);

} // namespace

#endif

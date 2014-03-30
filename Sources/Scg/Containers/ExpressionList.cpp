/**
 * @file Scg/Containers/ExpressionList.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

#include <stdarg.h>

// Boost header files
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

// Scg files
#include <Containers/ExpressionList.h>

using namespace llvm;

namespace Scg
{
  ExpressionList::ExpressionList(Expression *firstExpr, ...)
  {
    this->children.push_back(firstExpr);

    va_list exprs;
    va_start(exprs, firstExpr);
    while (auto expr = va_arg(exprs, Expression*))
      this->children.push_back(expr);
    va_end(exprs);
  }

  //----------------------------------------------------------------------------

  ExpressionList::ExpressionList(const ExpressionArray &body)
  {
    this->children = body;
  }

  //----------------------------------------------------------------------------

  std::string ExpressionList::ToString()
  {
    std::string str;
    // TODO: This will definitely not be enough if the block statement is big,
    // yet in many cases it might be over allocation.
    str.reserve(10*1024);
    str.append("{\n");
    for (auto expr : this->children)
    {
      auto exprStr = expr->ToString();
      str.append("  ");
      str.append(boost::replace_all_copy(exprStr, "\n", "  \n"));
    }
    str.append("}\n");
    return "";
  }
}

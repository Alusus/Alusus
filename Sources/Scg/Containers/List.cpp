/**
 * @file Scg/Containers/List.cpp
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
#include <boost/algorithm/string/join.hpp>

// LLVM header files
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/List.h>

using namespace llvm;

namespace Scg
{
  Expression::CodeGenerationStage List::GenerateCode()
  {
    // List doesn't evaluate to a value.
    // TODO: Later, a list built-in type should be added to the language making list
    // evaluate to a value!
    return Expression::GenerateCode();
  }

  //----------------------------------------------------------------------------

  std::string List::ToString()
  {
    std::vector<std::string> strs;
    strs.reserve(this->children.size());
    for (auto expr : this->children)
      strs.push_back(expr->ToString());
    return "(" + boost::algorithm::join(strs, ", ") + ")";
  }
}

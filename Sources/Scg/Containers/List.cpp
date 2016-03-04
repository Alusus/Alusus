/**
 * @file Scg/Containers/List.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Boost header files
#include <boost/algorithm/string/join.hpp>

// LLVM header files
#include <llvm/IR/IRBuilder.h>

// Scg files
#include <Containers/List.h>

using namespace llvm;

namespace Scg
{

std::string List::toString()
{
  std::vector<std::string> strs;
  strs.reserve(this->children.getCount());

  for (Int i = 0; i < this->children.getCount(); ++i) {
    auto node = io_cast<AstNode>(this->children.get(i));
    strs.push_back(node->toString());
  }

  return "(" + boost::algorithm::join(strs, ", ") + ")";
}

} // namespace

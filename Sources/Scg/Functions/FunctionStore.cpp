/**
 * @file Scg/Functions/FunctionStore.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// STL header files
#include <algorithm>

// Scg files
#include <Functions/FunctionSignature.h>
#include <Functions/FunctionStore.h>
#include <Functions/UserDefinedFunction.h>

using namespace llvm;

namespace Scg
{

void FunctionStore::add(SharedPtr<Function> const &function)
{
  this->functions.push_back(function);
}


SharedPtr<Function> const& FunctionStore::get(const std::string &name,
                                              const ValueTypeSpecArray &arguments) const
{
  for (Int i = 0; i < this->functions.size(); ++i) {
    SharedPtr<Function> const &func = this->functions[i];
    if (func->getName().compare(name) == 0) {
      if (func->isVarArgs()) {
        // The function has a variable number of arguments, so we only check
        // the fixed ones.
        if (func->getArgumentTypeSpecs().isEqualTo(&arguments,
            func->getArgumentTypeSpecs().size() /* limit the comparison */)) {
          return func;
        }
      } else {
        if (func->getArgumentTypeSpecs().isEqualTo(&arguments)) {
          return func;
        }
      }
    }
  }

  return SharedPtr<Function>::null;
}


Function* FunctionStore::match(const Module &module,
                               const std::string &name,
                               const ValueTypeSpecArray &argTypes) const
{
  // Find all the matching functions and sort them by the number of required
  // implicit casting.
  typedef std::pair<int, Function*> FunctionMatch;
  std::vector<FunctionMatch> matches;

  for (Int i = 0; i < this->functions.size(); ++i) {
    Function *function = this->functions[i].get();
    auto matchability = function->getSignature().match(module, name, argTypes);

    if (matchability != -1) {
      matches.push_back(std::make_pair(matchability, function));
    }
  }

  auto compFunc = [](const FunctionMatch &a, const FunctionMatch &b) {
    return a.first < b.first;
  };
  std::sort(matches.begin(), matches.end(), compFunc);

  switch (matches.size()) {
  case 0:
    // No match found.
    return nullptr;

  case 1:
    // Single match found.
    return matches[0].second;

  default:

    // More than one match.
    if (matches[0].first == matches[1].first) {
      // More than one match with the same number of implicit castings and
      // there is no preference of one over the other.
      // TODO: Should raise an error that there are multiple matches.
      return nullptr;
    } else {
      return matches[0].second;
    }
  }
}

} // namespace

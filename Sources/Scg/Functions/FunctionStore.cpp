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
void FunctionStore::Add(UserDefinedFunction *function)
{
  this->functions.push_back(function);
}

//------------------------------------------------------------------------------

const UserDefinedFunction *FunctionStore::Get(const std::string &name,
      const ValueTypeSpecArray &arguments) const
{
  for (auto func : this->functions) {
    if (func->GetFunctionName().compare(name) == 0) {
      if (func->IsVarArgs()) {
        // The function has a variable number of arguments, so we only check
        // the fixed ones.
        if (func->GetArgumentTypes().IsEqualTo(&arguments,
            func->GetArgumentTypes().size() /* limit the comparison */)) {
          return func;
        }
      } else {
        if (func->GetArgumentTypes().IsEqualTo(&arguments)) {
          return func;
        }
      }
    }
  }
  return nullptr;
}

//------------------------------------------------------------------------------

const UserDefinedFunction *FunctionStore::Get(const FunctionSignature &signature) const
{
	return Get(signature.name, signature.arguments);
}

//------------------------------------------------------------------------------

const UserDefinedFunction *FunctionStore::Match(const std::string &name, const ValueTypeSpecArray &argTypes) const
{
  // Find all the matching functions and sort them by the number of required
  // implicit casting.
	typedef std::pair<int, UserDefinedFunction *> FunctionMatch;
  std::vector<FunctionMatch> matches;
  for (auto function : this->functions) {
    auto matchability = function->GetSignature().Match(module, name, argTypes);
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
}

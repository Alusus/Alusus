/**
 * @file Scg/Instructions/FunctionMatcher.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg header files
#include <Functions/FunctionSignature.h>
#include <Containers/Module.h>

namespace Scg
{
int FunctionSignature::match(const Module &module, const std::string &name,
                             const ValueTypeSpecArray &argTypes) const
{
  if (this->name.compare(name) != 0) {
    // Name doesn't match.
    return -1;
  }

  if (varArgs) {
    if (this->arguments.size() > argTypes.size()) {
      // Argument count doesn't match.
      return -1;
    }
  } else {
    if (this->arguments.size() != argTypes.size()) {
      // Argument count doesn't match.
      return -1;
    }
  }

  auto implCastCount = 0;

  for (auto i = 0; i < this->arguments.size(); i++) {
    //auto compRes = this->arguments[i]->compare(module, argTypes[i]);
    auto compRes = argTypes[i]->compare(module, this->arguments[i]);

    if (compRes == TypeComparisonResult::NotEquivalent) {
      // One of the arguments is not equal or cannot be implicitly casted
      // to the corresponding argument in the other signature.
      return -1;
    }

    implCastCount += (int)compRes;
  }

  return implCastCount;
}
}

/**
 * @file Scg/Functions/FunctionSignature.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __FunctionMatcher_h__
#define __FunctionMatcher_h__

#include <Types/ValueTypeSpec.h>

namespace Scg
{
class Module;

/**
 * A structure containing the signature of a function.
 *
 * The signature of a function is the smallest set of information that
 * enables the identification of a function. This is the name of the function
 * and the arguments it accepts.
 */
class FunctionSignature
{
public:
  //! The name of the function.
  std::string name;
  //! The arguments of the functions.
  ValueTypeSpecArray arguments;

  /**
   * Constructs a signature with the given name and arguments.
   */
  FunctionSignature(const std::string &name, const ValueTypeSpecArray &args) :
    name(name), arguments(args) {}

  // TODO: Replace the return type with an enum.
  /**
   * Compares this function signature with the given signature.
   * @param[in] signature Another signature to compare against.
   * @return Returns -1 if the two signatures don't match, otherwise return
   * the number of implicit conversion required to match the signatures.
   */
  int Compare(const Module &module, const FunctionSignature &signature);
};
}

#endif // __FunctionMatcher_h__

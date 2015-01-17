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

#ifndef __FunctionSignature_h__
#define __FunctionSignature_h__

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
  const std::string &name;
  //! The arguments of the functions.
  const ValueTypeSpecArray &arguments;
  bool isVarArgs;

  /**
   * Constructs a signature with the given name and arguments.
   */
  FunctionSignature(const std::string &name, const ValueTypeSpecArray &args,
  		bool isVarArgs) : name(name), arguments(args), isVarArgs(isVarArgs) {}

  // TODO: Replace the return type with an enum.
  /**
   * Tries to match the given function name and argument types against this signature.
   * @param[in] name The name of the function to match against.
   * @param[in] argTypes The types of the arguments.
   * @return Returns -1 if there is no match, otherwise return the number of
   * implicit conversion required to match the signatures.
   */
  int Match(const Module &module, const std::string &name,
      const ValueTypeSpecArray &argTypes) const;
};
}

#endif // __FunctionSignature_h__

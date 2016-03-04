/**
 * @file Scg/Functions/FunctionStore.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_FUNCTIONSTORE_H
#define SCG_FUNCTIONSTORE_H

#include "core.h"
#include <AstNode.h>
#include <Functions/Function.h>
#include <typedefs.h>

namespace Scg
{

class FunctionSignature;

class FunctionStore : public AstNode
{
  //============================================================================
  // Type Info

  TYPE_INFO(FunctionStore, AstNode, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  // TODO: To quickly implement this function, I just used an array and
  // implemented a function that iterates through it to find the required
  // function. This is really inefficient, but should be sufficient for now.
  //! An array containing the functions of this store.
  private: std::vector<SharedPtr<Scg::Function> > functions;


  //============================================================================
  // Constructors & Destructor

  public: FunctionStore() {}


  //============================================================================
  // Member Functions

  /**
   * Adds the given function to the store.
   * @param[in] function  A pointer to the function to be added.
   */
  public: void add(SharedPtr<Function> const &function);

  /**
   * Retrieves the function having the given name and argument types.
   * @param[in] name  The name of the function.
   * @param[in] arguments An array containing the arguments of the function.
   * @return The required function if found, or @c nullptr.
   */
  public: SharedPtr<Function> const& get(const std::string &name,
                                         const ValueTypeSpecArray &arguments) const;

  /**
   * Retrieves the function having the given signature.
   * @param[in] signature The signature of the function to retrieve.
   * @return The requested function if found, or @c nullptr.
   */
  public: SharedPtr<Function> const& get(const FunctionSignature &signature) const
  {
    return this->get(signature.name, signature.arguments);
  }

  /**
   * Finds a function matching the given signature. The difference between
   * this and get() is that this method consider implicit casting.
   * @param[in] signature The signature of the function to match.
   * @return The matching function if found, or @c nullptr.
   */
  public: Function* match(const Module &module,
                          const std::string &name,
                          const ValueTypeSpecArray &argTypes) const;

}; // class

} // namespace

#endif

#ifndef __FunctionStore_h__
#define __FunctionStore_h__

#include <typedefs.h>

namespace Scg
{
class FunctionSignature;

/**
 * Represent a block of expressions.
 */
class FunctionStore: public Expression
{
  const Module &module;
  // TODO: To quickly implement this function, I just used an array and
  // implemented a function that iterates through it to find the required
  // function. This is really inefficient, but should be sufficient for now.
  //! An array containing the functions of this store.
  FunctionArray functions;

public:
  /**
   * Constructs a function store for the given module.
   */
  FunctionStore(const Module &module) : module(module) {}
  ~FunctionStore() {}

public:
  /**
   * Adds the given function to the store.
   * @param[in] function  A pointer to the function to be added.
   */
  void Add(UserDefinedFunction *function);

  // @{
  /**
   * Retrieves the function having the given name and argument types.
   * @param[in] name  The name of the function.
   * @param[in] arguments An array containing the arguments of the function.
   * @return The required function if found, or @c nullptr.
   */
  const UserDefinedFunction *Get(const std::string &name,
      const ValueTypeSpecArray &arguments) const;
  UserDefinedFunction *Get(const std::string &name,
      const ValueTypeSpecArray &arguments)
  {
    return const_cast<UserDefinedFunction *>(
        static_cast<const FunctionStore*>(this)->Get(name, arguments));
  }
  // @}

  // @{
  /**
   * Retrieves the function having the given signature.
   * @param[in] signature The signature of the function to retrieve.
   * @return The requested function if found, or @c nullptr.
   */
  const UserDefinedFunction *Get(const FunctionSignature &signature) const;
  UserDefinedFunction *Get(const FunctionSignature &signature)
  {
    return const_cast<UserDefinedFunction *>(
        static_cast<const FunctionStore*>(this)->Get(signature));
  }
  // @}

  // @{
  /**
   * Finds a function matching the given signature. The difference between
   * this and Get() is that this method consider implicit casting.
   * @param[in] signature The signature of the function to match.
   * @return The matching function if found, or @c nullptr.
   */
  const UserDefinedFunction *Match(const std::string &name, const ValueTypeSpecArray &argTypes) const;
  UserDefinedFunction *Match(const std::string &name, const ValueTypeSpecArray &argTypes)
  {
    return const_cast<UserDefinedFunction *>(
        static_cast<const FunctionStore*>(this)->Match(name, argTypes));
  }
  // @}
};
}

#endif // __FunctionStore_h__

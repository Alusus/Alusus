#ifndef __FunctionStore_h__
#define __FunctionStore_h__

// SCG header files
#include <BuiltInFunctions/Function.h>
#include <typedefs.h>

namespace Scg
{
class FunctionSignature;

/**
 * Represent a block of expressions.
 */
class FunctionStore : public Expression
{
  // TODO: To quickly implement this function, I just used an array and
  // implemented a function that iterates through it to find the required
  // function. This is really inefficient, but should be sufficient for now.
  //! An array containing the functions of this store.
  FunctionArray functions;

public:
  /**
   * Constructs a function store for the given module.
   */
  FunctionStore() {}
  ~FunctionStore() {}

public:
  /**
   * Adds the given function to the store.
   * @param[in] function  A pointer to the function to be added.
   */
  void add(Function *function);

  // @{
  /**
   * Retrieves the function having the given name and argument types.
   * @param[in] name  The name of the function.
   * @param[in] arguments An array containing the arguments of the function.
   * @return The required function if found, or @c nullptr.
   */
  const Function *get(const std::string &name,
                      const ValueTypeSpecArray &arguments) const;
  Function *get(const std::string &name,
                const ValueTypeSpecArray &arguments)
  {
    return const_cast<Function *>(
             static_cast<const FunctionStore*>(this)->get(name, arguments));
  }
  // @}

  // @{
  /**
   * Retrieves the function having the given signature.
   * @param[in] signature The signature of the function to retrieve.
   * @return The requested function if found, or @c nullptr.
   */
  const Function *get(const FunctionSignature &signature) const;
  Function *get(const FunctionSignature &signature)
  {
    return const_cast<Function *>(
             static_cast<const FunctionStore*>(this)->get(signature));
  }
  // @}

  // @{
  /**
   * Finds a function matching the given signature. The difference between
   * this and get() is that this method consider implicit casting.
   * @param[in] signature The signature of the function to match.
   * @return The matching function if found, or @c nullptr.
   */
  const Function *match(const Module &module,
                        const std::string &name, const ValueTypeSpecArray &argTypes) const;
  Function *match(const Module &module, const std::string &name, const ValueTypeSpecArray &argTypes)
  {
    return const_cast<Function*>(static_cast<const FunctionStore*>(this)->match(
                                   module, name, argTypes));
  }
  // @}
};
}

#endif // __FunctionStore_h__

#ifndef __FunctionStore_h__
#define __FunctionStore_h__

#include <typedefs.h>

namespace Scg
{
/**
 * Represent a block of expressions.
 */
class FunctionStore: public Expression
{
  // TODO: To quickly implement this function, I just used an array and
  // implemented a function that iterates through it to find the required
  // function. This is really inefficient, but should be sufficient for now.
  //! An array containing the functions of this store.
  FunctionArray functions;

public:
  FunctionStore() {}
  ~FunctionStore() {}

public:
  /**
   * Adds the given function to the store.
   * @param[in] function  A pointer to the function to be added.
   */
  void Add(Function *function);

  // @{
  /**
   * Retrieves the function having the given name and argument types.
   * @param[in] name  The name of the function.
   * @param[in] arguments An array containing the arguments of the function.
   * @return The required function if found, or @c nullptr.
   */
  const Function *Get(const std::string &name,
      const ValueTypeSpecArray &arguments) const;
  Function *Get(const std::string &name,
      const ValueTypeSpecArray &arguments)
  {
    return const_cast<Function*>(
        static_cast<const FunctionStore*>(this)->Get(name, arguments));
  }
  // @}
};
}

#endif // __FunctionStore_h__

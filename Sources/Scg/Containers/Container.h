/**
 * @file Scg/Containers/Container.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Scg_Containers_Container_h__
#define __Scg_Containers_Container_h__

#include <unordered_map>
#include <vector>
#include <Functions/FunctionStore.h>

namespace Scg
{
class Function;
class Variable;

class Container : public Expression
{
	//! A function store containing the functions defined in this container.
	//FunctionStore callables;
	//! A map containing the variables defined in this container.
	std::unordered_map<std::string, Variable*> variables;
	//! A pointer to the parent container or @c nullptr if this is the root.
	Container *parent;

public:
  Container(Container *parent = nullptr) : parent(parent) {}
  virtual ~Container() {}

public:
  //@{
  /**
   * Retrieves a pointer to the parent container.
   * @return A pointer to the parent or @c nullptr if this is the root.
   */
  const Container *GetParent() const { return parent; }
  Container *GetParent() { return parent; }
  //@}

  void AddCallable(Function *callable);
  void AddVariable(Variable *variable);

  //@{
  /**
   * Retrieves the callables matching the given names and arguments.
   *
   * Notice that this function also consider implicit casting during the matching.
   * This function starts by trying the current container. If it doesn't find a match
   * it tries to find matches in the parent container, and so on.
   *
   * @param[in] name The name of the callable to call.
   * @param[in] argTypes The types of the arguments to pass to the callable.
   *
   * @return A vector of all the matches.
   */
  const Function * MatchCallable(const std::string &name,
  		const ValueTypeSpecArray &argTyps) const;
  Function * MatchCallable(const std::string &name,
  		const ValueTypeSpecArray &argTypes)
  {
    return const_cast<Function *>(
        static_cast<const Container*>(this)->MatchCallable(name, argTypes));
  }
  //@}

  //@{
  /**
   * Retrieves the variable having the given name.
   */
  const Variable *GetVariable(const std::string &name) const;
  Variable *GetVariable(const std::string &name)
  {
    return const_cast<Variable*>(
        static_cast<const Container*>(this)->GetVariable(name));
  }
  //@}
};
}

#endif /* __Scg_Containers_Container_h__ */

/**
 * @file Scg/Containers/Block.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>
#include <Containers/Container.h>
#include <Values/Variable.h>

namespace Scg
{
void Container::AddCallable(Function *callable)
{
  //this->callables.Add(callable);
}

//------------------------------------------------------------------------------

void Container::AddVariable(Variable *variable)
{
  //this->variables.insert(variable->GetName(), variable);
}

//------------------------------------------------------------------------------

const Function *Container::MatchCallable(const std::string &name,
    const ValueTypeSpecArray &argTypes) const
{
  return nullptr;
  /*
  // Tries to match a function defined in the current container.
  auto match = this->callables.Match(name, argTypes);
  if (match != nullptr) {
    return match;
  }
  // Tries to match a function defined in the parent container.
  if (GetParent() != nullptr) {
    return GetParent()->MatchCallable(name, argTypes);
  }
  // Couldn't find a match.
  return nullptr;
  */
}

//------------------------------------------------------------------------------

const Variable* Container::GetVariable(const std::string &name) const
{
  // Tries to find the variable in the current container.
  auto it = this->variables.find(name);
  if (it != this->variables.end()) {
    return it->second;
  }
  // Tries to find the variable in the parent container.
  if (GetParent() != nullptr) {
    return GetParent()->GetVariable(name);
  }
  // Couldn't find a match.
  return nullptr;
}
}

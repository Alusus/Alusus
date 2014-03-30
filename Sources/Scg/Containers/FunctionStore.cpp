#include <prerequisites.h>

// Scg files
#include <Containers/FunctionStore.h>
#include <Values/Function.h>
#include <Types/ValueTypeSpec.h>

using namespace llvm;

namespace Scg
{
void FunctionStore::Add(Function *function)
{
  this->functions.push_back(function);
}

const Function *FunctionStore::Get(const std::string &name,
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
}

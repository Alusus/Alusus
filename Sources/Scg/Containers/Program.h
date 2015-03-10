/**
 * @file Scg/Containers/Program.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Program_h__
#define __Program_h__

// STL header files
#include <list>

// Alusus header files
#include <typedefs.h>
#include <llvm_fwd.h>

// Scg forward declarations
// TODO: Maybe this should be moved to an fwd file?
namespace Scg
{

class ArrayType;
class BinaryOperator;
class Block;
class FloatConst;
class CallFunction;
class DeclareExtFunction;
class DefineFunction;
class PointerType;
class Return;
}

namespace Scg
{
class Program
{
  //! A list of the modules making up this program.
  ModuleArray modules;
  //! The LLVM module representing this program.
  llvm::Module *llvmModule = nullptr;
  //! A store containing built-in functions.
  FunctionStore builtInFunctions;

  Core::Processing::BuildMsgStore *buildMsgStore = nullptr;

public:
  /**
   * Class constructor.
   */
  Program()
  {
    InitialiseBuiltInFunctions();
  }

  /**
   * Class destructor.
   */
  ~Program()
  {
    for (auto module : this->modules) {
      delete module;
    }
    this->modules.clear();
  }

private:
  void InitialiseBuiltInFunctions();

public:
  const ModuleArray &GetModules() const { return modules; }
  ModuleArray &GetModules() { return modules; }

  void SetBuildMsgStore(Core::Processing::BuildMsgStore *store) { this->buildMsgStore = store; }
  Core::Processing::BuildMsgStore* GetBuildMsgStore() { return this->buildMsgStore; }

  //@{
  /**
   * Retrieves the LLVM module this program generates. This only gets created
   * during compilation.
   * @return The LLVM Module.
   */
  const llvm::Module *GetLlvmModule() const
  {
  	return llvmModule;
  }
  llvm::Module *GetLlvmModule()
  {
  	return llvmModule;
  }
  //@}

  /**
   * Adds the given module to the program.
   * @param[in] module  A pointer to the module to be added. Notice that this
   *                    should be allocated in the heap and is going to be
   *                    freed by the program so the user shouldn't free it.
   */
  void AddModule(Module *module)
  {
    this->modules.push_back(module);
  }

  /**
   * Determine whether this program has a function with the given signature,
   * whether it is a function defined in the program or just a declaration.
   *
   * @note While this function might return true indicating that there is a
   * DefineFunction or DeclareExtFunction instructions, the function
   * GetFunction() might still return @c nullptr until the function has
   * actually been generated during code generation.
   *
   * @param[in] name      The name of the function.
   * @param[in] arguments The types of the arguments of the required function.
   *
   * @return True or false.
   */
  bool HasFunction(const std::string &name,
  		const ValueTypeSpecArray &arguments) const;

  /**
   * Gets all function matches for the given name and argument types.
   * @param[in] name      The name of the function.
   * @param[in] arguments The types of the arguments of the required function.
   * @return A pointer to the function, or nullptr if there is no matching
   * function.
   */
  std::vector<Function *> GetFunction(const std::string &funcName,
  		const ValueTypeSpecArray &arguments);

  std::vector<Function *> MatchFunction(const std::string &funcName,
  		const ValueTypeSpecArray &arguments);

  /**
   * Finds the DefineFunction instructions that generates functions matching
   * the given name.
   * @return An array containing all the matching DefineFunction instructions.
   */
  std::vector<DefineFunction*> FindDefineFunction(const std::string &funcName,
      const ValueTypeSpecArray &arguments);

  /**
   * Finds the DeclareExtFunction instructions that generates functions matching
   * the given name.
   * @return An array containing all the matching DeclareExtFunction instructions.
   */
  std::vector<DeclareExtFunction*> FindDeclareFunction(const std::string &funcName,
      const ValueTypeSpecArray &arguments);

  /**
   * Compiles the program into LLVM IR and return the result.
   * @return A string containing the generated LLVM IR code.
   */
  std::string Compile();

  /**
   * Executes the program given the name of the main function.
   * @param[in] functionName  The name of the entry point of the program.
   */
  void Execute(const char *functionName);
};
}

#endif // __Program_h__

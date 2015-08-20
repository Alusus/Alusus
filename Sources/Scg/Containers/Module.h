/**
 * @file Scg/Containers/Module.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Module_h__
#define __Module_h__

// STL header files
#include <list>
#include <unordered_map>

// Alusus header files
#include <Functions/FunctionSignature.h>
#include <Functions/FunctionStore.h>
#include <Functions/UserDefinedFunction.h>
#include <llvm_fwd.h>
#include <typedefs.h>

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
class Program;
class Return;
}

namespace Scg
{
class Module : public Expression
{
  friend class Program;

  //llvm::Module *llvmModule;
  //ExpressionArray expressions;
  ExpressionArray headerComplExprs;
  ExpressionArray footerComplExprs;
  int tempIndex = 0;
  //! A store of all the functions defined or declared in the module.
  FunctionStore functionStore;
  //! A map containing the variables defined in this block.
  VariableMap variableMap;
  //! A pointer to the program containing this module, or null.
  Program *program = nullptr;
  //! The name of the module.
  std::string name;
  //! A map of the user defined types.
  mutable ValueTypeMap typeMap;

public:
  Module(const std::string &name);
  ~Module();

  //@{
  /**
   * Retrieves the name of the module.
   * @return The name of the module.
   */
  const std::string &GetName() const { return this->name; }
  std::string &GetName() { return this->name; }
  //@}

  //@{
  /**
   * Get a pointer to the LLVM module.
   *
   * @return A pointer to the LLVM module.
   */
  const llvm::Module *GetLlvmModule() const;
  llvm::Module *GetLlvmModule();
  //@}

  // TODO: Do we need a constant version of this member function?
  /**
   * Retrieves the value type having the given name.
   * @param[in] typeName  The name of the value type to retrieve.
   */
  ValueType *GetValueTypeByName(const std::string &typeName) const;

  /**
   * Retrieves the value type having the given name.
   * @param[in] typeName  The name of the value type to retrieve.
   */
  ValueType *tryGetValueTypeByName(const std::string &typeName) const;

  /**
   * Adds the given function to the function store of the module.
   * @param[in] function  A pointer to the function to be added.
   */
  void AddFunction(Function *function) {
    this->functionStore.Add(function);
  }

  /**
   * Determine whether this module has a function with the given signature,
   * whether it is a function defined in the module or just a declaration.
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

  //@{
  /**
   * Get the function having the given name and argument types.
   *
   * @note This function might return null even if HasFunction() returns false,
   * as this function only retrieves the UserDefinedFunction object if it has been
   * generated.
   *
   * @param[in] name      The name of the function.
   * @param[in] arguments The types of the arguments of the required function.
   * @return A pointer to the function, or nullptr if there is no matching
   * function.
   */
  const Function *GetFunction(const std::string &name,
      const ValueTypeSpecArray &arguments) const
  {
    return this->functionStore.Get(name, arguments);
  }
  Function *GetFunction(const std::string &name,
      const ValueTypeSpecArray &arguments)
  {
    return this->functionStore.Get(name, arguments);
  }
  //@}

  Function *MatchFunction(const std::string &name,
      const ValueTypeSpecArray &arguments)
  {
    return this->functionStore.Match(*this, name, arguments);
  }

  //@{
  /**
   * Retrieves a pointer to the program containing this module.
   * @return A pointer to the program, or @c null if this program is not
   * in a module.
   */
  const Program *GetProgram() const
  {
    return program;
  }

  Program *GetProgram()
  {
    return program;
  }
  //@}

  /**
   * Finds the DefineFunction instruction that generates a function with the
   * given name.
   * @param[in] name  The name of the function.
   * @return A pointer to the DefineFunction instruction, or @c null.
   */
  DefineFunction *FindDefineFunction(const std::string &name,
      const ValueTypeSpecArray &arguments);

  /**
   * Finds the DeclareExtFunction instruction that declares a function with the
   * given name.
   * @param[in] name  The name of the function.
   * @return A pointer to the DeclareExtFunction instruction, or @c null.
   */
  DeclareExtFunction *FindDeclareFunction(const std::string &name,
      const ValueTypeSpecArray &arguments);

  /**
   * Returns a reference to the variable map.
   * @return A reference to the variable map.
   */
  const VariableMap &GetVariableMap() const { return variableMap; }
  VariableMap &GetVariableMap() { return variableMap; }

  /**
   * Get a reference to the type map of this module.
   *
   * @return A reference to the type map.
   */
  const ValueTypeMap &GetTypeMap() const { return typeMap; }
  ValueTypeMap &GetTypeMap() { return typeMap; }

  std::string GetTempVarName();

  //@{
  /**
   * Prepends the given expression at the beginning of the module.
   * @param[in] expr  A pointer to the expression.
   */
  void PrependExpression(Expression *expr);
  void PrependExpressions(ExpressionArray &expr);
  //@}

  //@{
  /**
   * Appends the given expression to the end of the module.
   * @param[in] expr  A pointer to the expression.
   */
  void AppendExpression(Expression *expr);
  void AppendExpressions(ExpressionArray &expr);
  //@}

private:
  /**
   * Sets a pointer to the program containing this module.
   * @param program A pointer to the program.
   */
  void SetProgram(Program *prog) { this->program = prog; }
};
}

#endif // __Module_h__

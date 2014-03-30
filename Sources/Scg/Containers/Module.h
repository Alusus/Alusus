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

// Alusus header files
#include <Containers/FunctionStore.h>
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
class Module
{
  friend class Program;

  llvm::Module *llvmModule;
  ExpressionArray expressions;
  ExpressionArray headerComplExprs;
  ExpressionArray footerComplExprs;
  int tempIndex = 0;
  //! A store of all the functions defined or declared in the module.
  FunctionStore functionStore;
  //! A map containing the variables defined in this block.
  VariableMap variableMap;
  /*! A set containing the names of the functions automatically linked through
      code generation of CallFunction instructions. */
  StringSet autoLinkFuncSet;
  //! A pointer to the program containing this module, or null.
  Program *program = nullptr;
  //! A map of the user defined types.
  mutable ValueTypeMap typeMap;
  /*! A list of the types allocated by this module to be deallocated at
      destruction time. */
  mutable std::list<ValueType*> allocatedTypes;

public:
  Module(const std::string &name);
  ~Module();

  /**
   * Get a pointer to the LLVM module.
   *
   * @return A pointer to the LLVM module.
   */
  const llvm::Module *GetLlvmModule() const { return llvmModule; }
  llvm::Module *GetLlvmModule() { return llvmModule; }

  // TODO: Do we need a constant version of this member function?
  /**
   * Retrieves the value type having the given name.
   * @param[in] typeName  The name of the value type to retrieve.
   */
  ValueType *GetValueTypeByName(const std::string &typeName) const;

  /**
   * Creates a new array type.
   *
   * @param[in] elementsType  The type of the elements of the array.
   * @param[in] arraySize     The number of elements in the array.
   *
   * @return A pointer to the newly created array type. This shouldn't be
   * deallocated by the caller as it gets automatically deallocated by the
   * module.
   */
  ArrayType *GetArrayValueType(ValueType &elementsType, int arraySize) const;

  /**
   * Creates a new pointer type.
   *
   * @param[in] contentType   The type of the content of this pointer.
   *
   * @return A pointer to the newly created pointer type. This shouldn't be
   * deallocated by the caller as it gets automatically deallocated by the
   * module.
   */
  PointerType *GetPointerValueType(ValueType &contentType) const;

  /**
   * Adds the given function to the function store of the module.
   * @param[in] function  A pointer to the function to be added.
   */
  void AddFunction(Function *function) {
    this->functionStore.Add(function);
  }

  //@{
  /**
   * Get the function having the given name and argument types.
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
   * Returns a reference to the auto-link function set.
   *
   * The auto-link link function set contains the names of the functions for
   * which a link was automatically generated during the pre-code generation
   * step of the CallFunction instruction. It is useful to avoid creating
   * a link more than once if a call to the same function is made more than
   * once.
   *
   * @return A reference to the auto-link function set.
   */
  const StringSet &GetAutoDeclFuncSet() const { return autoLinkFuncSet; }
  StringSet &GetAutoDeclFuncSet() { return autoLinkFuncSet; }

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

  /**
   * Prepends a new complementary expression to the module.
   * @note Complementary expressions are expressions generated by other
   * expressions during the pre-code generation stage to be able to continue
   * the code generation. An example of this is the CallFunction instruction
   * which automatically adds a DeclareExtFunction instruction if the function
   * being called is defined in a different module. Complementary expressions
   * get added to the main list of expressions during the beginning of each
   * pre-code generation cycle.
   * @param[in] expr  A pointer to the expression to be added.
   */
  void PrependComplementaryExpression(Expression *expr)
  {
    headerComplExprs.push_back(expr);
  }

  /**
   * Appends a new complementary expression to the module.
   * @note Complementary expressions are expressions generated by other
   * expressions during the pre-code generation stage to be able to continue
   * the code generation. An example of this is the CallFunction instruction
   * which automatically adds a DeclareExtFunction instruction if the function
   * being called is defined in a different module. Complementary expressions
   * get added to the main list of expressions during the beginning of each
   * pre-code generation cycle.
   * @param[in] expr  A pointer to the expression to be added.
   */
  void AppendComplementaryExpression(Expression *expr)
  {
    footerComplExprs.push_back(expr);
  }

public:
  void Compile();
  void Compile(std::string &out);
  void Execute(const char *functionName);

  const ExpressionArray& GetExpressions() const
  {
    return expressions;
  }

private:
  void PreGenerateCode();
  void GenerateCode();
  void PostGenerateCode();
  void GenerateRequiredDeclExtFunctions();

  /**
   * Sets a pointer to the program containing this module.
   * @param program A pointer to the program.
   */
  void SetProgram(Program *prog) { this->program = prog; }

};
}

#endif // __Module_h__

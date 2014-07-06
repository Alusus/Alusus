/**
 * @file Scg/Instructions/DeclareExtFunction.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __DeclareExtFunction_h__
#define __DeclareExtFunction_h__

// Scg header files
#include <Instructions/Instruction.h>
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <Functions/Function.h>

#include <llvm_fwd.h>

namespace Scg
{
class DefineFunction;

/**
 * Represents a function definition, i.e. a prototype and body.
 */
class DeclareExtFunction: public Instruction
{
  //! A string containing the name of the function.
  std::string name;
  //! The return value of the function.
  ValueTypeSpec *returnType;
  //! An array containing the types of the function's arguments.
  ValueTypeSpecArray argTypes;
  //! Whether the function has a variable number of arguments.
  bool isVarArgs;

public:
  // TODO: The return type and arguments should support pointers.
  /**
   * Constructs a function declaration with the given name and signature.
   * @param[in] name        The name of the function.
   * @param[in] returnType  The return value type of the function.
   * @param[in] argTypes    The types of the function arguments.
   * @param[in] isVarArgs   Whether the function has a variable number of
   * arguments.
   */
  DeclareExtFunction(const std::string &name, ValueTypeSpec *returnType,
      const ValueTypeSpecArray &argTypes, bool isVarArgs = false);

  /**
   * Constructs a copy of the given DeclareExtFunction instruction.
   * @param[in] declFunc  The DeclareExtFunction instruction to be copied.
   */
  DeclareExtFunction(DeclareExtFunction *declFunc);

  /**
   * Constructs a function declaration for the given function definition.
   * @param definition  A pointer to the function definition to construct a
   * declaration for.
   */
  DeclareExtFunction(DefineFunction *definition);

  /**
   * Class destructor.
   */
  ~DeclareExtFunction();

  /**
   * Retrieves the name of the function being declared.
   * @return The name of the function being declared.
   */
  const std::string &GetName() const { return name; }
  std::string &GetName() { return name; }

  /**
   * Retrieves the return type of the function being declared.
   * @return A pointer to the @c ValueTypeSpec specifying the return type.
   */
  const ValueTypeSpec *GetReturnType() const { return returnType; }
  ValueTypeSpec *GetReturnType() { return returnType; }

  /**
   * Retrieves an array containing the argument types of the function being declared.
   * @return An array containing the argument types of the function being declared.
   */
  const ValueTypeSpecArray &GetArgumentTypes() const { return argTypes; }
  ValueTypeSpecArray &GetArgumentTypes() { return argTypes; }

  /**
   * Returns whether the function has a variable number of arguments.
   * @return Whether the function has a variable number of arguments.
   */
  bool IsVarArgs() const { return isVarArgs; }

  /**
   * Retrieves a pointer to the Alusus function declared by this instruction.
   *
   * @return A pointer to the Alusus function declared by this instruction.
   */
  const Function *GetDeclaredFunction() const { return (Function*)(children[0]); }
  Function *GetDeclaredFunction() { return (Function*)(children[0]); }

  //! @copydoc Expression::PreGenerateCode()
  virtual CodeGenerationStage PreGenerateCode();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __DeclareExtFunction_h__

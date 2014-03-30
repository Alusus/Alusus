/**
 * @file Scg/Values/Function.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Function_h__
#define __Function_h__

// Scg header files
#include <Values/Value.h>
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>

#include <llvm_fwd.h>

namespace Scg
{
class Block;
class ValueTypeSpec;
}

namespace Scg
{
/**
 * Represents a function definition, i.e. a prototype and body.
 */
class Function: public Value
{
  /**
   * Specifies what an instance of the Function class represents: a function
   * or a link to an external function.
   */
  enum FunctionDefinitionSource
  {
    Internal,   //!< The function is defined internally, i.e. in the source code.
    External    //!< The function is defined externally and this is a link only.
  };

  //! Whether this is a function or a link to an external function.
  FunctionDefinitionSource funcType;
  //! A string containing the name of the function being defined or linked to.
  std::string name;
  //! The return value type of the function being defined or linked to.
  ValueTypeSpec *returnType;
  //! An array containing the types of the arguments of the function being linked to.
  ValueTypeSpecArray argTypes;
  /*! An array containing the definitions (types and names) of the arguments of
      the function being defined. */
  VariableDefinitionArray argDefs;
  //! Whether the function being linked to has a variable number of arguments.
  bool isVarArgs = false;
  //! A pointer to the LLVM function representing this function.
  llvm::Function *llvmFunction = nullptr;
  /*! An array containing the Alusus variables representing the arguments of the
      function. */
  VariableArray args;

public:
  /**
   * Construct a function with the given name, arguments, and body.
   * @param[in] name        The name of the function.
   * @param[in] returnType  The return value type of the function.
   * @param[in] arguments   The arguments of the function.
   * @param[in] body        The body of the function.
   */
  Function(const std::string &name, ValueTypeSpec *returnType,
      const VariableDefinitionArray &argDefs, Block *body);

  /**
   * Construct a link to an external function with the given name, return type,
   * and arguments.
   * @param[in] name        The name of the function.
   * @param[in] returnType  The return value type of the function.
   * @param[in] argTypes    The types of the function arguments.
   */
  Function(const std::string &name, ValueTypeSpec *returnType,
      const ValueTypeSpecArray &argTypes, bool isVarArgs = false);

  //! Class destructor.
  ~Function();

  // TODO: Document these functions.

  FunctionDefinitionSource GetFunctionType() const { return funcType; }
  const std::string &GetFunctionName() const { return name; }
  const ValueTypeSpec *GetReturnType() const { return returnType; }
  const ValueTypeSpecArray &GetArgumentTypes() const { return argTypes; }
  const VariableDefinitionArray &GetArgumentDefinitions() const { return argDefs; }
  int GetArgumentCount() const
  {
    if (funcType == FunctionDefinitionSource::Internal)
      return GetArgumentDefinitions().size();
    else
      return GetArgumentTypes().size();
  }
  bool IsVarArgs() const { return isVarArgs; }
  const llvm::Function *GetLlvmFunction() const { return llvmFunction; }
  llvm::Function *GetLlvmFunction() { return llvmFunction; }

  /**
   * Get the expression representing the left-hand side of the binary operator.
   *
   * @return A pointer to the left-hand side expression.
   */
  const Block *GetBody() const
  {
    return (Block*) children[0];
  }
  Block *GetBody()
  {
    return (Block*) children[0];
  }

private:
  /**
   * Called by PreGenerateCode() if this object represents a function.
   */
  void CreateFunction();

  /**
   * Called by PreGenerateCode() if this object represents a link to an
   * external function.
   */
  void CreateLinkToExternalFunction();

public:
  //! @copydoc Expression::PreGenerateCode()
  virtual CodeGenerationStage PreGenerateCode();

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationResult GenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();

private:
  virtual void SetFunction(Function *function);
};
}

#endif // __Function_h__

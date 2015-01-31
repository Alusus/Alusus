/**
 * @file Scg/Instructions/CallFunction.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __CallFunction_h__
#define __CallFunction_h__

// Scg header files
#include <Instructions/Instruction.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>
#include <typedefs.h>

namespace Scg
{
class List;
class Function;

// TODO: Modify CallFunction class such that it accepts an expression in
// addition to a name, where the expression should be a pointer to a function.
// One idea might be to change the PointerToVariable operator such that it also
// checks for functions in addition to variables.

/**
 * Represents a call to a function.
 */
class CallFunction : public Instruction
{
  //! The name of the function to call.
  std::string funcName;
  //! A list containing the arguments of the function.
  List *args;
  /*! A pointer to the function this CallFunction instruction is calling.
      This is found during the code generation process. */
  Function *function = nullptr;
  /*! An array containing the types of the arguments. This is filled in
      during the code generation process. */
  ValueTypeSpecArray argTypes;
  //! The LLVM call instruction representing this function call.
  llvm::CallInst *callInst;

public:
  /**
   * Constructs a CallFunction instruction.
   * @param[in] functionName  The name of the function to be called.
   * @param[in] args          An list of the arguments to be passed to the
   *                          function.
   */
  CallFunction(const std::string &functionName, List *args)
    : funcName(functionName), callInst(0), args(args)
  {
    // Add the arguments to the children so that they are called during
    // the code generation process.
    // TODO: I am using reinterpret_cast because I don't want to include
    // List class. Is this is the correct approach?
    children.push_back(reinterpret_cast<Expression*>(args));
  }

  /**
   * Retrieves the name of the function to be called.
   * @return A string containing the name of the function to be called.
   */
  const std::string &GetFunctionName() const { return funcName; }

  //@{
  /**
   * Retrieve an array containing the types of the arguments of the function
   * being called. Note this gets generated during the pre-code generation
   * stage.
   */
  const ValueTypeSpecArray &GetArgumentTypes() const { return argTypes; }
  ValueTypeSpecArray &GetArgumentTypes() { return argTypes; }
  //@}

  /**
   * Retrieves the expression containing the arguments passed to the function.
   * @return A pointers to the expression.
   */
  const List *GetArguments() const { return reinterpret_cast<List*>(children[0]); }
  List *GetArguments() { return reinterpret_cast<List*>(children[0]); }

  //! @copydoc Expression::GetValueType()
  virtual const ValueType *GetValueType() const;

public:
  //! @copydoc Expression::PreGenerateCode()
  virtual CodeGenerationStage PreGenerateCode();

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationStage GenerateCode();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __CallFunction_h__

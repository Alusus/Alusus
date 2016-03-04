/**
 * @file Scg/Instructions/CallFunction.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CALLFUNCTION_H
#define SCG_CALLFUNCTION_H

#include "core.h"
#include <Instructions/Instruction.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>
#include <typedefs.h>

namespace Scg
{

class CodeGenUnit;
class List;

// TODO: Modify CallFunction class such that it accepts an expression in
// addition to a name, where the expression should be a pointer to a function.
// One idea might be to change the IdentifierReference operator such that it also
// checks for functions in addition to variables.

/**
 * Represents a call to a function.
 */
class CallFunction : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(CallFunction, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! The name of the function to call.
  private: std::string funcName;
  //! A list containing the arguments of the function.
  private: SharedPtr<List> args;
  /*! A pointer to the function this CallFunction instruction is calling.
    This is found during the code generation process. */
  private: Function *function = nullptr;
  /*! An array containing the types of the arguments. This is filled in
    during the code generation process. */
  private: ValueTypeSpecArray argTypes;
  //! The LLVM call instruction representing this function call.
  private: llvm::Value *callInst;


  //============================================================================
  // Constructors & Destructor

  /**
   * Constructs a CallFunction instruction.
   * @param[in] functionName  The name of the function to be called.
   * @param[in] args          An list of the arguments to be passed to the
   *                          function.
   */
  public: CallFunction(Char const *functionName, SharedPtr<List> const &args)
    : funcName(functionName), callInst(nullptr), args(args)
  {
    OWN_SHAREDPTR(this->args);
  }

  public: virtual ~CallFunction()
  {
    DISOWN_SHAREDPTR(this->args);
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the name of the function to be called.
   * @return A string containing the name of the function to be called.
   */
  public: const std::string &getFunctionName() const
  {
    return funcName;
  }

  /**
   * Retrieve an array containing the types of the arguments of the function
   * being called. Note this gets generated during the pre-code generation
   * stage.
   */
  public: const ValueTypeSpecArray &getArgumentTypes() const
  {
    return argTypes;
  }
  public: ValueTypeSpecArray &getArgumentTypes()
  {
    return argTypes;
  }

  /**
   * Retrieves the expression containing the arguments passed to the function.
   * @return A pointers to the expression.
   */
  public: SharedPtr<List> const& getArguments() const
  {
    return this->args;
  }

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::preGenerateCode()
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, IdentifiableObject *val)
  {
  }

  public: virtual void remove(Int index)
  {
  }

  public: virtual Word getCount() const
  {
    return 1;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    return this->args.get();
  }

}; // class

} // namespace

#endif

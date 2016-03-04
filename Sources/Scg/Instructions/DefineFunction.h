/**
 * @file Scg/Instructions/DefineFunction.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_DEFINEFUNCTION_H
#define SCG_DEFINEFUNCTION_H

#include "core.h"
#include <Functions/UserDefinedFunction.h>
#include <Instructions/Instruction.h>
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>

using namespace Core;

namespace Scg
{

class Block;
class ValueTypeSpec;
class CodeGenUnit;

/**
 * Represents a function definition, i.e. a prototype and body.
 */
class DefineFunction : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(DefineFunction, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! A string containing the name of the function.
  private: std::string name;
  //! The return value type of the function.
  private: SharedPtr<ValueTypeSpec> returnType;
  //! An array containing the types and names of the function's arguments.
  private: VariableDefinitionArray arguments;
  //! A pointer to the block containing the body of the function.
  private: SharedPtr<Block> body;
  //! The function created during code generation.
  private: SharedPtr<UserDefinedFunction> function;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a function with the given name, arguments, and body.
   * @param[in] name        The name of the function.
   * @param[in] returnType  The return value type of the function.
   * @param[in] arguments   The arguments of the function.
   * @param[in] body        The body of the function.
   */
  public: DefineFunction(Core::Basic::Char const *name, SharedPtr<ValueTypeSpec> const &returnType,
                         const VariableDefinitionArray &arguments, SharedPtr<Block> const &body) :
    name(name),
    returnType(returnType),
    arguments(arguments),
    body(body)
  {
  }

  public: static SharedPtr<DefineFunction> create(Core::Basic::Char const *name,
                                                  SharedPtr<ValueTypeSpec> const &returnType,
                                                  const VariableDefinitionArray &arguments,
                                                  SharedPtr<Block> const &body)
  {
    return std::make_shared<DefineFunction>(name, returnType, arguments, body);
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the name of the function to be defined by this instruction.
   * @return The name of the function to be defined.
   */
  public: std::string const& getName() const
  {
    return this->name;
  }

  /**
   * Retrieves the return type of the function.
   * @return The return type of the function.
   */
  public: SharedPtr<ValueTypeSpec> const& getReturnType() const
  {
    return this->returnType;
  }

  /**
   * Retrieves the arguments of the function to be defined by this instruction.
   * This is an array type-name tuple, one array item for each argument.
   * @return The arguments of the function.
   */
  public: VariableDefinitionArray const& getArguments() const
  {
    return this->arguments;
  }

  /**
   * Retrieves a pointer to the Alusus function defined by this instruction.
   *
   * @return A pointer to the Alusus function defined by this instruction.
   */
  public: SharedPtr<UserDefinedFunction> const& getDefinedFunction() const
  {
    return this->function;
  }

  //! @copydoc AstNode::preGenerateCode()
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit);

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
    return this->function.get();
  }

}; // class

} // namespace

#endif

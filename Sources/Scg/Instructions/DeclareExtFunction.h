/**
 * @file Scg/Instructions/DeclareExtFunction.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_DECLAREEXTFUNCTION_H
#define SCG_DECLAREEXTFUNCTION_H

#include "core.h"
#include <Instructions/Instruction.h>
#include <Types/ValueType.h>
#include <Types/ValueTypeSpec.h>
#include <Functions/ExternalFunction.h>
#include <llvm_fwd.h>

using namespace Core;

namespace Scg
{

class DefineFunction;
class CodeGenUnit;

/**
 * Represents a function definition, i.e. a prototype and body.
 */
class DeclareExtFunction : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(DeclareExtFunction, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! A string containing the name of the function.
  private: std::string name;
  //! The return value of the function.
  private: SharedPtr<ValueTypeSpec> returnType;
  //! An array containing the types of the function's arguments.
  private: ValueTypeSpecArray argTypes;
  //! Whether the function has a variable number of arguments.
  private: bool varArgs;
  //! The function object created during code generation.
  private: SharedPtr<ExternalFunction> function;


  //============================================================================
  // Constructors & Destructor

  // TODO: The return type and arguments should support pointers.
  /**
   * Constructs a function declaration with the given name and signature.
   * @param[in] name        The name of the function.
   * @param[in] returnType  The return value type of the function.
   * @param[in] argTypes    The types of the function arguments.
       * @param[in] varArgs   Whether the function has a variable number of
   * arguments.
   */
  public: DeclareExtFunction(Core::Basic::Char const *name, SharedPtr<ValueTypeSpec> const &returnType,
                             ValueTypeSpecArray const &argTypes, bool varArgs = false);

  /**
   * Constructs a copy of the given DeclareExtFunction instruction.
   * @param[in] declFunc  The DeclareExtFunction instruction to be copied.
   */
  public: DeclareExtFunction(DeclareExtFunction *declFunc);

  /**
   * Constructs a function declaration for the given function definition.
   * @param definition  A pointer to the function definition to construct a
   * declaration for.
   */
  public: DeclareExtFunction(DefineFunction *definition);

  public: static SharedPtr<DeclareExtFunction> create(Core::Basic::Char const *name, SharedPtr<ValueTypeSpec> const &returnType,
                                                      ValueTypeSpecArray const &argTypes, bool varArgs = false)
  {
    return std::make_shared<DeclareExtFunction>(name, returnType, argTypes, varArgs);
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the name of the function being declared.
   * @return The name of the function being declared.
   */
  public: std::string const& getName() const
  {
    return this->name;
  }

  /**
   * Retrieves the return type of the function being declared.
   * @return A pointer to the @c ValueTypeSpec specifying the return type.
   */
  public: SharedPtr<ValueTypeSpec> const& getReturnType() const
  {
    return this->returnType;
  }

  /**
   * Retrieves an array containing the argument types of the function being declared.
   * @return An array containing the argument types of the function being declared.
   */
  public: ValueTypeSpecArray const& getArgumentTypes() const
  {
    return argTypes;
  }
  public: ValueTypeSpecArray& getArgumentTypes()
  {
    return argTypes;
  }

  /**
   * Returns whether the function has a variable number of arguments.
   * @return Whether the function has a variable number of arguments.
   */
  public: bool isVarArgs() const
  {
    return varArgs;
  }

  /**
   * Retrieves a pointer to the Alusus function declared by this instruction.
   *
   * @return A pointer to the Alusus function declared by this instruction.
   */
  public: SharedPtr<ExternalFunction> const& getDeclaredFunction() const
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

  public: virtual void set(Int index, TiObject *val)
  {
  }

  public: virtual void remove(Int index)
  {
  }

  public: virtual Word getCount() const
  {
    return 1;
  }

  public: virtual TiObject* get(Int index) const
  {
    return this->function.get();
  }

}; // class

} // namespace

#endif

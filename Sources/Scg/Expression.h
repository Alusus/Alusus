/**
 * @file Scg/Expression.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Expression_h__
#define __Expression_h__

// TODO: Consider moving the file to a different folder.

// Scg header files.
#include <typedefs.h>
#include <exceptions.h>

// LLVM forward declarations
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{
class BinaryOperator;
class Block;
class CallFunction;
class UserDefinedFunction;
class FunctionAstBlock;
class FunctionExpression;
class DefineStruct;
class DefineVariable;
class FloatConst;
class FunctionLinkExpression;
class Module;
class Program;
class Return;
class Struct;
}

namespace Scg
{
/**
 * Base class for all expression classes.
 */
class Expression
{
  // TODO: Befriending other expression classes is necessary to be able to call
  // setModule() and setBlock() methods from them. Is there a better way to
  // do this?
  friend class Array;
  friend class BinaryOperator;
  friend class Block;
  friend class CallFunction;
  friend class CodeGenerator;
  friend class DeclareExtFunction;
  friend class DefineFunction;
  friend class DefineStruct;
  friend class DefineVariable;
  friend class FloatConst;
  friend class UserDefinedFunction;
  friend class FunctionAstBlock;
  friend class FunctionalExpression;
  friend class FunctionLinkExpression;
  friend class IfStatement;
  friend class Module;
  friend class Return;
  friend class Struct;

public:
  /**
   * An enumeration that specifies the code generation stage an expression is
   * currently at.
   */
  enum CodeGenerationStage {
    PreCodeGeneration,  //!< The expression is still in pre-code generation.
    CodeGeneration,     //!< The expression is generating code.
    PostCodeGeneration, //!< The expression is doing post-code generation.
    None                /*!< The expression either finished code generation
                   or hasn't started. */
  };

private:
  //! A pointer to the module containing this expression.
  Module *module = nullptr;
  //! A pointer to the block containing this expression.
  UserDefinedFunction *function = nullptr;
  //! A pointer to the block containing this expression.
  Block *block = nullptr;
  //! The code generation stage of this expression.
  CodeGenerationStage codeGenStage = CodeGenerationStage::None;
  //! The code generation stage of the children of this expression.
  CodeGenerationStage childrenCodeGenStage = CodeGenerationStage::None;
  //! The source location at which this expression was produced.
  Core::Data::SourceLocation sourceLocation;
  bool preserveChildrenCodeGenerationOrder = false;

protected:
  //! The LLVM Value object generated during code generation, or @c nullptr.
  llvm::Value *generatedLlvmValue = nullptr;
  //! Whether this expression generated a terminate instruction, e.g. Return instruction.
  bool termInstGenerated = false;

protected:
  //! An array containing the children expressions.
  ExpressionArray children;

public:
  Expression() {}

  /**
   * Destructor.
   */
  virtual ~Expression() {};

  Core::Data::SourceLocation const& getSourceLocation() const
  {
    return this->sourceLocation;
  }

  //@{
  /**
   * Returns a pointer to the program containing this expression.
   * @return A pointer to the module or @c nullptr.
   */
  const Program *getProgram() const;
  Program *getProgram()
  {
    return const_cast<Program*>(
             const_cast<const Expression*>(this)->getProgram());
  }
  //@}

  /**
   * Returns a pointer to the module containing this expression.
   *
   * @return A pointer to the module.
   */
  const Module *getModule() const
  {
    return module;
  }
  Module *getModule()
  {
    return module;
  }

  /**
   * Get a pointer to the function whose body is this block.
   *
   * @return A pointer to the function, or 0.
   */
  const UserDefinedFunction *getFunction() const
  {
    return function;
  }
  UserDefinedFunction *getFunction()
  {
    return function;
  }

  /**
   * Returns a pointer to the module containing this expression, or 0 if this is the top expression.
   *
   * A pointer to the module or 0.
   */
  const Block *getBlock() const
  {
    return block;
  }
  Block *getBlock()
  {
    return block;
  }

  // TODO: Should we change the return type of this function to
  // const ValueType &?
  /**
   * Retrieves the type of the value this expression evaluates to. If the
   * expression doesn't evaluate to a value, an expression is thrown.
   * @return The type of the value this expression evaluates to.
   * @throw InvalidValueException If the expression doesn't evaluate to a
   * value.
   */
  virtual const ValueTypeSpec *getValueTypeSpec() const
  {
    // TODO: This exception is being frequently used, with a similar statement
    // each time. A macro should be created for it to avoid duplication.
    throw EXCEPTION(InvalidValueException,
                    "Expression doesn't evaluate to a value.");
  }
  virtual ValueTypeSpec *getValueTypeSpec()
  {
    return const_cast<ValueTypeSpec*>(
             static_cast<const Expression *>(this)->getValueTypeSpec());
  }

  /**
   * Returns a reference to the children.
   */
  const ExpressionArray& getChildren() const
  {
    return children;
  }

  /**
   * Retrieves the code generation stage of this expression and its children.
   * @note An expression will not pass a generation stage until all its
   * children pass that stage too. For example, if an expression itself is
   * under CodeGeneration stage but one or more of its children are under
   * still under PreCodeGeneration stage, then the expression as a whole is
   * considered to be in PreCodeGeneration stage.
   * @return The code generation stage of this expression and its children.
   */
  CodeGenerationStage getCodeGenerationStage()
  {
    return std::min(codeGenStage, childrenCodeGenStage);
  }

  virtual CodeGenerationStage callPreGenerateCode();
  virtual CodeGenerationStage callGenerateCode();
  virtual CodeGenerationStage callPostGenerateCode();

  /**
   * Called as a preparatory step before generating IR code.
   *
   * @return @c true if the function succeeds, otherwise @c false.
   */
  virtual CodeGenerationStage preGenerateCode()
  {
    // By default there is nothing to do, so we just move the expression
    // to the next stage of code generation.
    return CodeGenerationStage::CodeGeneration;
  }

  /**
   * Generate LLVM IR code out of this expression.
   *
   * @return The value of this expression, or 0 if it doesn't evaluate to a value.
   */
  virtual CodeGenerationStage generateCode()
  {
    return CodeGenerationStage::PostCodeGeneration;
  }

  // TODO: This is not implemented yet, but I just added the declaration so that
  // I can use it in the build-in functions. The idea is to change the generateCode()
  // member function such that it doesn't return the value (or at least stores it
  // somewhere) and leave it to this function to generate it. The reason is that
  // I want to move the logic of creating LLVM instructions for built-in and
  // user-defined functions to their classes, rather than the CallFunction instruction.
  // This way the CallFunction can be used in a generic way whether it is calling a
  // built-in function or a user-defined one. So I added a function called
  // CreateLLVMInstruction() in the Function class. But this function shouldn't be
  // intervening in the process of generating the code for the arguments to be passed,
  // thus it needs to call the method
  /**
   * Retrieves the value that was generated by this expression in generateCode()
   * member function.
   * @return The generated LLVM Value.
   */
  virtual llvm::Value *getGeneratedLlvmValue() const
  {
    // TODO: Probably we should throw an exception if this method is being called
    // before the code-generation stage.
    return generatedLlvmValue;
  }

  /**
   * Retrieves a value determining whether a terminate instruction was generated
   * during the course of the code generation of this expression.
   *
   * @note This function should only be called after generateCode() member function
   * finishes execution successfully, i.e. with
   * CodeGenerationStage::PostCodeGeneration value.
   *
   * @return True or false.
   */
  virtual bool isTermInstGenerated() const
  {
    // TODO: Probably we should throw an exception if this method is being called
    // before the code-generation stage.
    return termInstGenerated;
  }

  /**
   * Called as a finalising step after generating IR code.
   *
   * @return @c true if finalisation code executed successfully, otherwise @c false.
   * This is useful because this function might need to be called multiple times to
   * be executed multiple times in case of dependencies preventing finalisation.
   */
  virtual CodeGenerationStage postGenerateCode()
  {
    // By default there is nothing to do, so we just move the expression
    // to the next stage of code generation.
    return CodeGenerationStage::None;
  }

  // TODO: Convert to abstract function and implement in deriving classes.
  /**
   * Returns a string representation of the expression.
   *
   * @return The string representation.
   */
  virtual std::string toString() const
  {
    return "";
  }

private:
  virtual void setModule(Module *module);
  virtual void setFunction(UserDefinedFunction *function);
  virtual void setBlock(Block *block);

  /// Sets the source location at which this expression was produced.
  void setSourceLocation(Core::Data::SourceLocation const &sl)
  {
    this->sourceLocation = sl;
  }

public:
  static void *operator new[](size_t size) = delete;

  static void operator delete[](void *ptr) = delete;

  static void *operator new(size_t size);

  static void operator delete(void *ptr);
};
}

#endif // __Expression_h__

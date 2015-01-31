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

namespace Scg
{
  class BinaryOperator;
  class Block;
  class CallFunction;
  class Function;
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
  // TODO: Document this structure.
  struct CodeGenerationResult
  {
    llvm::Value *exprValue;
    bool termInstGenerated;

    CodeGenerationResult()
    {
      exprValue = 0;
      termInstGenerated = false;
    }

    CodeGenerationResult(llvm::Value *_exprValue)
      : exprValue(_exprValue)
      , termInstGenerated(false)
    {
    }

    CodeGenerationResult(llvm::Value *_exprValue, bool _termInstGenerated)
      : exprValue(_exprValue)
      , termInstGenerated(_termInstGenerated)
    {
    }
  };

  /**
   * Base class for all expression classes.
   */
  class Expression
  {
    // TODO: Befriending other expression classes is necessary to be able to call
    // SetModule() and SetBlock() methods from them. Is there a better way to
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
    friend class Function;
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
    enum CodeGenerationStage
    {
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
    Function *function = nullptr;
    //! A pointer to the block containing this expression.
    Block *block = nullptr;
    //! The code generation stage of this expression.
    CodeGenerationStage codeGenStage = CodeGenerationStage::None;
    //! The code generation stage of the children of this expression.
    CodeGenerationStage childrenCodeGenStage = CodeGenerationStage::None;
    //! The number of the line in code that produced this expression.
    int lineInCode = 0;
    //! The number of the column in code that produced this expression.
    int columnInCode = 0;

  protected:
    //! The LLVM Value object generated during code generation, or @c nullptr.
    llvm::Value *generatedLlvmValue = nullptr;
    //! Whether this expression generated a terminate instruction, e.g. Return instruction.
    bool termInstGenerated = false;

  protected:
    //! An array containing the children expressions.
    ExpressionArray children;
    //! Whether children should automatically be deleted at destruction time.
    bool autoDeleteChildren = true;

  public:
    Expression() { }

    /**
     * Destructor.
     */
    virtual ~Expression();

    //@{
    /**
     * Returns a pointer to the program containing this expression.
     * @return A pointer to the module or @c nullptr.
     */
    const Program *GetProgram() const;
    Program *GetProgram()
    {
    	return const_cast<Program*>(
    			const_cast<const Expression*>(this)->GetProgram());
    }
    //@}

    /**
     * Returns a pointer to the module containing this expression.
     *
     * @return A pointer to the module.
     */
    const Module *GetModule() const { return module; }
    Module *GetModule() { return module; }

    /**
     * Get a pointer to the function whose body is this block.
     *
     * @return A pointer to the function, or 0.
     */
    const Function *GetFunction() const { return function; }
    Function *GetFunction() { return function; }

    /**
     * Returns a pointer to the module containing this expression, or 0 if this is the top expression.
     *
     * A pointer to the module or 0.
     */
    const Block *GetBlock() const { return block; }
    Block *GetBlock() { return block; }

    // TODO: Should we change the return type of this function to
    // const ValueType &?
    /**
     * Retrieves the type of the value this expression evaluates to. If the
     * expression doesn't evaluate to a value, an expression is thrown.
     * @return The type of the value this expression evaluates to.
     * @throw InvalidValueException If the expression doesn't evaluate to a
     * value.
     */
    virtual const ValueType *GetValueType() const
    {
      // TODO: This exception is being frequently used, with a similar statement
      // each time. A macro should be created for it to avoid duplication.
      THROW_EXCEPTION(InvalidValueException,
          "Expression doesn't evaluate to a value.");
    }
    virtual ValueType *GetValueType()
    {
      return const_cast<ValueType*>(
          static_cast<const Expression*>(this)->GetValueType());
    }

    /**
     * Returns a reference to the children.
     */
    const ExpressionArray& GetChildren() const
    {
      return children;
    }

    /**
     * Returns the number of the line in code that produced this expression.
     * @return The number of the line in code that produced this expression.
     */
    int GetLineInCode() const { return lineInCode; }

    /**
     * Returns the number of the column in code that produced this expression.
     * @return The number of the column in code that produced this expression.
     */
    int GetColumnInCode() const { return columnInCode; }

    /**
     * Retrieves the code generation stage of this expression and its children.
     * @note An expression will not pass a generation stage until all its
     * children pass that stage too. For example, if an expression itself is
     * under CodeGeneration stage but one or more of its children are under
     * still under PreCodeGeneration stage, then the expression as a whole is
     * considered to be in PreCodeGeneration stage.
     * @return The code generation stage of this expression and its children.
     */
    CodeGenerationStage GetCodeGenerationStage()
    {
      return std::min(codeGenStage, childrenCodeGenStage);
    }

    virtual CodeGenerationStage CallPreGenerateCode();
    virtual CodeGenerationStage CallGenerateCode();
    virtual CodeGenerationStage CallPostGenerateCode();

    /**
     * Called as a preparatory step before generating IR code.
     *
     * @return @c true if the function succeeds, otherwise @c false.
     */
    virtual CodeGenerationStage PreGenerateCode()
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
    virtual CodeGenerationStage GenerateCode()
    {
    	return CodeGenerationStage::PostCodeGeneration;
    }

    // TODO: This is not implemented yet, but I just added the declaration so that
    // I can use it in the build-in functions. The idea is to change the GenerateCode()
    // member function such that it doesn't return the value (or at least stores it
    // somewhere) and leave it to this function to generate it. The reason is that
    // I want to move the logic of creating LLVM instructions for built-in and
    // user-defined functions to their classes, rather than the CallFunction instruction.
    // This way the CallFunction can be used in a generic way whether it is calling a
    // built-in function or a user-defined one. So I added a function called
    // CreateLLVMInstruction() in the Callable class. But this function shouldn't be
    // intervening in the process of generating the code for the arguments to be passed,
    // thus it needs to call the method
    /**
     * Retrieves the value that was generated by this expression in GenerateCode()
     * member function.
     * @return The generated LLVM Value.
     */
    virtual llvm::Value *GetGeneratedLlvmValue() const
    {
    	// TODO: Probably we should throw an exception if this method is being called
    	// before the code-generation stage.
    	return generatedLlvmValue;
    }

    /**
     * Retrieves a value determining whether a terminate instruction was generated
     * during the course of the code generation of this expression.
     *
     * @note This function should only be called after GenerateCode() member function
     * finishes execution successfully, i.e. with
     * CodeGenerationStage::PostCodeGeneration value.
     *
     * @return True or false.
     */
    virtual bool IsTermInstGenerated() const
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
    virtual CodeGenerationStage PostGenerateCode()
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
    virtual std::string ToString() const { return ""; }

  private:
    virtual void SetModule(Module *module);
    virtual void SetFunction(Function *function);
    virtual void SetBlock(Block *block);

    /**
     * Sets the number of the line in code that produced this expression.
     * @param[in] lineInCode  The line in code.
     */
    void SetLineInCode(int line) { this->lineInCode = line; }

    /**
     * Sets the number of the column in code that produced this expression.
     * @param[in] columnInCode  The column in code.
     */
    void SetColumnInCode(int column) { this->columnInCode = column; }
  };
}

#endif // __Expression_h__

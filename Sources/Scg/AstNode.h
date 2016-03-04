/**
 * @file Scg/AstNode.h
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_ASTNODE_H
#define SCG_ASTNODE_H

// LLVM forward declarations
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

// Core
#include "core.h"

// Scg header files.
#include "typedefs.h"
#include "exceptions.h"

namespace Scg
{

class CodeGenUnit;

/**
 * Base class for all AST classes.
 */
class AstNode : public Core::Data::Node
{
  //============================================================================
  // Type Info

  TYPE_INFO(AstNode, Core::Data::Node, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Data Types

  /**
   * An enumeration that specifies the code generation stage an expression is
   * currently at.
   */
  public: enum CodeGenerationStage {
    PreCodeGeneration,  //!< The expression is still in pre-code generation.
    CodeGeneration,     //!< The expression is generating code.
    PostCodeGeneration, //!< The expression is doing post-code generation.
    None                /*!< The expression either finished code generation
                             or hasn't started. */
  };


  //============================================================================
  // Member Variables

  //! The code generation stage of this object.
  private: CodeGenerationStage codeGenStage = CodeGenerationStage::None;

  //! The code generation stage of the children of this expression.
  protected: CodeGenerationStage childrenCodeGenStage = CodeGenerationStage::None;

  protected: bool preserveChildrenCodeGenerationOrder = false;

  //! The LLVM Value object generated during code generation, or @c nullptr.
  protected: llvm::Value *generatedLlvmValue = nullptr;

  //! Whether this expression generated a terminate instruction, e.g. Return instruction.
  protected: bool termInstGenerated = false;

  //! The source location at which this expression was produced.
  private: Core::Data::SourceLocation sourceLocation;


  //============================================================================
  // Constructors & Destructor

  protected: AstNode()
  {
  }

  public: virtual ~AstNode()
  {
  }


  //============================================================================
  // Member Functions

  /// Sets the source location at which this expression was produced.
  public: void setSourceLocation(Core::Data::SourceLocation const &sl)
  {
    this->sourceLocation = sl;
  }

  public: Core::Data::SourceLocation const& getSourceLocation() const
  {
    return this->sourceLocation;
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
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const
  {
    // TODO: This exception is being frequently used, with a similar statement
    // each time. A macro should be created for it to avoid duplication.
    throw EXCEPTION(InvalidValueException,
                    "Expression doesn't evaluate to a value.");
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
  public: CodeGenerationStage getCodeGenerationStage()
  {
    return std::min(codeGenStage, childrenCodeGenStage);
  }

  public: virtual CodeGenerationStage callPreGenerateCode(CodeGenUnit *codeGenUnit);
  public: virtual CodeGenerationStage callGenerateCode(CodeGenUnit *codeGenUnit);
  public: virtual CodeGenerationStage callPostGenerateCode(CodeGenUnit *codeGenUnit);

  /**
   * Called as a preparatory step before generating IR code.
   *
   * @return @c true if the function succeeds, otherwise @c false.
   */
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit)
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
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit)
  {
    return CodeGenerationStage::PostCodeGeneration;
  }

  /**
   * Called as a finalising step after generating IR code.
   *
   * @return @c true if finalisation code executed successfully, otherwise @c false.
   * This is useful because this function might need to be called multiple times to
   * be executed multiple times in case of dependencies preventing finalisation.
   */
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit)
  {
    // By default there is nothing to do, so we just move the expression
    // to the next stage of code generation.
    return CodeGenerationStage::None;
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
  public: virtual llvm::Value *getGeneratedLlvmValue() const
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
  public: virtual bool isTermInstGenerated() const
  {
    // TODO: Probably we should throw an exception if this method is being called
    // before the code-generation stage.
    return this->termInstGenerated;
  }

  // TODO: Convert to abstract function and implement in deriving classes.
  /**
   * Returns a string representation of the node.
   *
   * @return The string representation.
   */
  public: virtual std::string toString() const
  {
    return "";
  }

}; // class

} // namespace

#endif

/**
 * @file Scg/Values/Variable.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_VARIABLE_H
#define SCG_VARIABLE_H

#include <exceptions.h>
#include <typedefs.h>
#include <Values/Value.h>

// LLVM header files and forward declaration
#include <llvm_fwd.h>

namespace Scg
{

class CodeGenUnit;

class Variable : public Value
{
  //============================================================================
  // Type Info

  TYPE_INFO(Variable, Value, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The name of the variable.
  protected: std::string name;
  // TODO: We should change this variable type to const ValueType & after we
  // change the return type of the function Value::getValueTypeSpec().
  //! The type of the variable.
  protected: ValueType *variableType;
  //! The LLVM Alloca instruction used to define the variable, if it is local.
  protected: llvm::AllocaInst *llvmAllocaInst = nullptr;
  //! The LLVM GlobalVariable object used to define the variable, if it is global.
  protected: llvm::GlobalVariable *llvmGlobalVariable = nullptr;
  /*! If this variable represents a function argument, this points to the
    LLVM StoreInst used to set the value of the variable to the value
    passed by the caller of the function. */
  protected: llvm::StoreInst *llvmStoreInst = nullptr;
  /*! If this variable represents a function argument, this points to the
    LLVM Argument instance representing the function argument. */
  protected: llvm::Argument *llvmArgument;


  //============================================================================
  // Constructors & Destructor

  /**
   * Constructs a variable.
   * @param[in] name          The name of the variable.
   * @param[in] variableType  The type of the variable.
   * @param[in] argument      If this variable represents a function argument,
   *                          pass in the LLVM Argument representing the
   *                          argument here.
   */
  public: Variable(const std::string &name, ValueType *variableType,
                   llvm::Argument *llvmArgument = nullptr)
    : name(name), variableType(variableType), llvmArgument(llvmArgument)
  {
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the name of the variable.
   * @return The name of the variable.
   */
  public: std::string const& getName()
  {
    return this->name;
  }

  /**
   * Returns an LLVM Value object representing this variable. This is either
   * the LLVM AllocaInst object if this is a local variable, or the LLVM
   * GlobalVariable object if this is a global variable.
   *
   * @note Note that this method shouldn't be called outside the code
   * generation block, because it needs access to the containing block to
   * be able to fetch the variable.
   *
   * @return The LLVM Value object representing this variable.
   */
  public: llvm::Value const* getLlvmValue() const
  {
    if (llvmAllocaInst != nullptr) {
      return llvmAllocaInst;
    } else {
      return llvmGlobalVariable;
    }
  }

  public: llvm::Value* getLlvmValue()
  {
    return const_cast<llvm::Value *>(static_cast<const Variable *>(this)->getLlvmValue());
  }

  /**
   * Retrieves a pointer to the LLVM Alloca instruction which is used to
   * define this variable in LLVM, if it is a local variable.
   *
   * @note Note that this method shouldn't be called outside the code
   * generation block, because it needs access to the containing block to
   * be able to fetch the variable.
   *
   * @return A pointer to the LLVM Alloca instruction.
   */
  public: llvm::AllocaInst const* getLlvmAllocaInst() const
  {
    return this->llvmAllocaInst;
  }

  public: llvm::AllocaInst* getLlvmAllocaInst()
  {
    return const_cast<llvm::AllocaInst *>(static_cast<const Variable *>(this)->getLlvmAllocaInst());
  }

  /**
   * Retrieves a pointer to the LLVM GlobalVariable object which is used to
   * define this variable in LLVM, if it is a global variable.
   *
   * @note Note that this method shouldn't be called outside the code
   * generation block, because it needs access to the containing block to
   * be able to fetch the variable.
   *
   * @return A pointer to the LLVM GlobalVariable instruction.
   */
  public: llvm::GlobalVariable const* getLlvmGlobalVariable() const
  {
    return this->llvmGlobalVariable;
  }

  public: llvm::GlobalVariable* getLlvmGlobalVariable()
  {
    return const_cast<llvm::GlobalVariable *>(static_cast<const Variable *>(this)->getLlvmGlobalVariable());
  }

  // TODO: We should change the return type of this function to
  // const ValueType &.

  //! @copydoc Value::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return variableType->getValueTypeSpec();
  }

  //! @copydoc AstNode::preGenerateCode()
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();

}; // class

} // namespace

#endif

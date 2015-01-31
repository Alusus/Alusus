/**
 * @file Scg/Values/Variable.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Variable_h__
#define __Variable_h__

#include <exceptions.h>
#include <typedefs.h>
#include <Values/Value.h>

// LLVM header files and forward declaration
#include <llvm_fwd.h>

namespace Scg
{
class Variable : public Value
{
protected:
  //! The name of the variable.
  std::string name;
  // TODO: We should change this variable type to const ValueType & after we
  // change the return type of the function Value::GetValueType().
  //! The type of the variable.
  ValueType *variableType;
  //! The LLVM Alloca instruction used to define the variable, if it is local.
  llvm::AllocaInst *llvmAllocaInst = nullptr;
  //! The LLVM GlobalVariable object used to define the variable, if it is global.
  llvm::GlobalVariable *llvmGlobalVariable = nullptr;
  /*! If this variable represents a function argument, this points to the
      LLVM StoreInst used to set the value of the variable to the value
      passed by the caller of the function. */
  llvm::StoreInst *llvmStoreInst = nullptr;
  /*! If this variable represents a function argument, this points to the
      LLVM Argument instance representing the function argument. */
  llvm::Argument *llvmArgument;

public:
  /**
   * Constructs a variable.
   * @param[in] name          The name of the variable.
   * @param[in] variableType  The type of the variable.
   * @param[in] argument      If this variable represents a function argument,
   *                          pass in the LLVM Argument representing the
   *                          argument here.
   */
  Variable(const std::string &name, ValueType *variableType,
      llvm::Argument *llvmArgument = nullptr)
    : name(name)
    , variableType(variableType)
    , llvmArgument(llvmArgument)
  {}

  /**
   * Retrieves the name of the variable.
   * @return The name of the variable.
   */
  const std::string &GetName() { return name; }

  //@{
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
  const llvm::Value *GetLlvmValue() const
  {
    if (llvmAllocaInst != nullptr) {
      return llvmAllocaInst;
    } else {
      return llvmGlobalVariable;
    }
  }
  llvm::Value *GetLlvmValue()
  {
    return const_cast<llvm::Value*>(
        static_cast<const Variable*>(this)->GetLlvmValue());
  }
  //@}

  //@{
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
  const llvm::AllocaInst *GetLlvmAllocaInst() const
  {
    return llvmAllocaInst;
  }
  llvm::AllocaInst *GetLlvmAllocaInst()
  {
    return const_cast<llvm::AllocaInst*>(
        static_cast<const Variable*>(this)->GetLlvmAllocaInst());
  }
  //@}

  //@{
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
  const llvm::GlobalVariable *GetLlvmGlobalVariable() const
  {
    return llvmGlobalVariable;
  }
  llvm::GlobalVariable *GetLlvmGlobalVariable()
  {
    return const_cast<llvm::GlobalVariable*>(
        static_cast<const Variable*>(this)->GetLlvmGlobalVariable());
  }
  //@}

  // TODO: We should change the return type of this function to
  // const ValueType &.

  //@{
  //! @copydoc Value::GetValueType()
  virtual const ValueType *GetValueType() const { return variableType; }
  virtual ValueType *GetValueType() { return variableType; }
  //@}

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

#endif // __Variable_h__

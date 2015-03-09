/**
 * @file Scg/Containers/Block.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Block_h__
#define __Block_h__

// Scg header files
#include <typedefs.h>
#include <Expression.h>

// LLVM header files
#pragma warning(disable: 4146 4800 4355 4996)
#include <llvm/IR/IRBuilder.h>
#pragma warning(default: 4146 4800 4355 4996)
//#include <llvm_fwd.h>

namespace Scg
{
  /**
   * Represent a block of expressions.
   */
  class Block : public Expression
  {
    //! A map containing the variables defined in this block.
    VariableMap variableMap;
    //! The LLVM BasicBlock object representing the block.
    llvm::BasicBlock *llvmBasicBlock;
    //! The IR builder used to generate IR code when GenerateCode() is called.
    llvm::IRBuilder<> *irBuilder;

  public:
    /**
     * Constructs an empty block.
     */
    Block() : llvmBasicBlock(0) , irBuilder(0)
    {
      this->preserveChildrenCodeGenerationOrder = true;

    }

    /**
     * Constructs an empty block.
     */
    Block(const ExpressionArray &body);

    /**
     * Get a pointer to the LLVM IR builder.
     *
     * @return A pointer to the LLVM IR builder.
     */
    const llvm::IRBuilder<> *GetIRBuilder() const { return irBuilder; }
    llvm::IRBuilder<> *GetIRBuilder() { return irBuilder; }

    /**
     * Get a pointer to the LLVM Basic Block representing this block.
     *
     * @return A pointer to the LLVM Basic Block.
     */
    const llvm::BasicBlock *GetLlvmBB() const { return llvmBasicBlock; }
    llvm::BasicBlock *GetLlvmBB() { return llvmBasicBlock; }

    /**
     * Prepends the given expression at the beginning of the block.
     *
     * @param[in] expr  A pointer to the expression.
     */
    void PrependExpression(Expression *expr)
    {
      children.insert(children.begin(), expr);
    }

    /**
     * Append the given expression to the end of the block.
     *
     * @param[in] expr  A pointer to the expression.
     */
    void AppendExpression(Expression *expr)
    {
      children.push_back(expr);
    }

    /**
     * Get the variable having the given name.
     * @param[in] name  The name of the variable.
     * @return A pointer to the variable, or 0 if there is variable with the
     * the given name.
     */
    const Variable *GetVariable(const std::string &name) const;

    Variable *GetVariable(const std::string &name)
    {
      return const_cast<Variable*>(
          static_cast<const Block*>(this)->GetVariable(name));
    }

    /**
     * Returns a reference to the variable map.
     * @return A reference to the variable map.
     */
    const VariableMap &GetVariableMap() const { return variableMap; }
    VariableMap &GetVariableMap() { return variableMap; }

    //! @copydoc Expression::PreGenerateCode()
    virtual CodeGenerationStage PreGenerateCode();

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationStage GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();

  private:
    //! @copydoc Expression::SetBlock()
    virtual void SetBlock(Block *block);

  private:
    static int GetNewIndex()
    {
      static int s_blockIndex = 0;
      return s_blockIndex++;
    }

    static std::string GetNewBlockName();
  };
}

#endif // __Block_h__

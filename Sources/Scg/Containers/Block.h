/**
 * @file Scg/Containers/Block.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_BLOCK_H
#define SCG_BLOCK_H

// Scg header files
#include <typedefs.h>
#include <AstNode.h>

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
class Block : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(Block, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! A map containing the variables defined in this block.
  private: VariableMap variableMap;
  //! The LLVM BasicBlock object representing the block.
  private: llvm::BasicBlock *llvmBasicBlock;
  //! The IR builder used to generate IR code when generateCode() is called.
  private: llvm::IRBuilder<> *irBuilder;
  //! An array containing the children nodes.
  private: Core::Data::SharedList children;


  //============================================================================
  // Constructors

  public: Block() : llvmBasicBlock(0), irBuilder(0)
  {
    this->preserveChildrenCodeGenerationOrder = true;
  }

  public: Block(AstNodeSharedArray const &body);

  public: virtual ~Block();

  public: static SharedPtr<Block> create(AstNodeSharedArray const &body)
  {
    return std::make_shared<Block>(body);
  }


  //============================================================================
  // Member Functions

  /**
   * Get a pointer to the LLVM IR builder.
   *
   * @return A pointer to the LLVM IR builder.
   */
  public: const llvm::IRBuilder<> *getIRBuilder() const
  {
    return irBuilder;
  }
  public: llvm::IRBuilder<> *getIRBuilder()
  {
    return irBuilder;
  }

  /**
   * Get a pointer to the LLVM Basic Block representing this block.
   *
   * @return A pointer to the LLVM Basic Block.
   */
  public: const llvm::BasicBlock *getLlvmBB() const
  {
    return llvmBasicBlock;
  }
  public: llvm::BasicBlock *getLlvmBB()
  {
    return llvmBasicBlock;
  }

  /// Prepends the given node at the beginning of the block.
  public: void prependNode(SharedPtr<AstNode> const &node)
  {
    this->children.insert(0, node);
    OWN_SHAREDPTR(node);
  }

  /// Append the given node to the end of the block.
  public: void appendNode(SharedPtr<AstNode> const &node)
  {
    this->children.add(node);
    OWN_SHAREDPTR(node);
  }

  /**
   * Get the variable having the given name.
   * @param[in] name  The name of the variable.
   * @return A pointer to the variable, or 0 if there is variable with the
   * the given name.
   */
  public: const Variable *getVariable(const std::string &name) const;

  public: Variable *getVariable(const std::string &name)
  {
    return const_cast<Variable*>(
             static_cast<const Block*>(this)->getVariable(name));
  }

  /**
   * Returns a reference to the variable map.
   * @return A reference to the variable map.
   */
  public: VariableMap const& getVariableMap() const
  {
    return variableMap;
  }
  public: VariableMap& getVariableMap()
  {
    return variableMap;
  }

  //! @copydoc AstNode::preGenerateCode()
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();

  private: static int getNewIndex()
  {
    static int s_blockIndex = 0;
    return s_blockIndex++;
  }

  private: static std::string getNewBlockName();


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
    return this->children.getCount();
  }

  public: virtual TiObject* get(Int index) const
  {
    return this->children.get(index);
  }

}; // class

} // namespace

#endif

/**
 * @file Scg/Instructions/IfStatement.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_IFSTATEMENT_H
#define SCG_IFSTATEMENT_H

#include "core.h"
#include <AstNode.h>
#include <llvm_fwd.h>

namespace Scg
{

class Block;
class CodeGenUnit;

/**
 * Represents a function definition, i.e. a prototype and body.
 */
class IfStatement : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfStatement, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  private: SharedPtr<AstNode> condition;
  private: SharedPtr<Block> thenBlock;
  private: SharedPtr<Block> elseBlock;
  private: SharedPtr<Block> mergeBlock;
  private: llvm::ICmpInst *cmpInst;
  private: llvm::BranchInst *ifBranch;
  private: llvm::BranchInst *thenBranch;
  private: llvm::BranchInst *elseBranch;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a function with the given name, arguments, and body.
   *
   * @param[in] condition The condition of the if statement.
   * @param[in] thenBlock The block to be executed if the condition evaluates to true.
   * @param[in] elseBlock The block to be executed if the condition evaluates to false.
   */
  public: IfStatement(SharedPtr<AstNode> const &condition,
                      SharedPtr<Block> const &thenBlock,
                      SharedPtr<Block> const &elseBlock);

  public: virtual ~IfStatement();


  //============================================================================
  // Member Functions

  /**
   * Get the block to be executed if the condition evaluates to true.
   *
   * @return A pointer to the 'then' block.
   */
  public: SharedPtr<AstNode> const& getCondition() const
  {
    return this->condition;
  }

  /**
   * Get the block to be executed if the condition evaluates to true.
   *
   * @return A pointer to the 'then' block.
   */
  public: SharedPtr<Block> const& getThenBlock() const
  {
    return this->thenBlock;
  }

  /**
   * Get the block to be executed if the condition evaluates to true.
   *
   * @return A pointer to the 'else' block.
   */
  public: SharedPtr<Block> const& getElseBlock() const
  {
    return this->elseBlock;
  }

  public: SharedPtr<Block> const& getMergeBlock() const
  {
    return this->mergeBlock;
  }

  //! @copydoc AstNode::callGenerateCode()
  public: virtual CodeGenerationStage callGenerateCode(CodeGenUnit *codeGenUnit)
  {
    // We want to manually call the generateCode() member function of children
    // so we override the default behaviour of callGenerateCode();
    return generateCode(codeGenUnit);
  }

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
    return 4;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    switch (index) {
      case 0: return this->condition.get();
      case 1: return this->thenBlock.get();
      case 2: return this->elseBlock.get();
      case 3: return this->mergeBlock.get();
      default: return 0;
    }
  }

}; // class

} // namespace

#endif

/**
 * @file Scg/Instructions/WhileStatement.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_WHILESTATEMENT_H
#define SCG_WHILESTATEMENT_H

#include "core.h"
#include <Instructions/Instruction.h>
#include <llvm_fwd.h>

namespace Scg
{

class Block;
class CodeGenUnit;

/**
 * Represents a function definition, i.e. a prototype and body.
 */
class WhileStatement : public Instruction, Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(WhileStatement, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  private: AstNode *cond;
  private: SharedPtr<Block> condBlock;
  private: SharedPtr<Block> loopBlock;
  private: SharedPtr<Block> exitBlock;
  private: llvm::Instruction *brInst;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a function with the given name, arguments, and body.
   *
   * @param[in] cond  The condition expression of the for statement.
   * @param[in] body  The body of the for loop.
   */
  public: WhileStatement(SharedPtr<AstNode> const &cond, SharedPtr<Block> const &body);

  public: virtual ~WhileStatement();


  //============================================================================
  // Member Functions

  /**
   * Get the condition expression of the for loop.
   *
   * @return A pointer to the condition expression of the for loop.
   */
  public: AstNode* getCondExpr() const
  {
    return this->cond;
  }

  /**
   * Get the body of the for loop.
   *
   * @return A pointer to the body of the for loop.
   */
  public: SharedPtr<Block> const& getBody() const
  {
    return this->loopBlock;
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
    return 3;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    switch (index) {
      case 0: return this->condBlock.get();
      case 1: return this->loopBlock.get();
      case 2: return this->exitBlock.get();
      default: return 0;
    }
  }

}; // class

} // namespace

#endif

/**
 * @file Scg/Instructions/ForStatement.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_FORSTATEMENT_H
#define SCG_FORSTATEMENT_H

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
class ForStatement : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(ForStatement, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  private: AstNode *init;
  private: AstNode *cond;
  private: AstNode *loop;
  private: SharedPtr<Block> initBlock;
  private: SharedPtr<Block> condBlock;
  private: SharedPtr<Block> loopBlock;
  private: SharedPtr<Block> exitBlock;
  private: llvm::Instruction *brInst;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a function with the given name, arguments, and body.
   *
   * @param[in] init  The initialization expression of the for statement.
   * @param[in] cond  The condition expression of the for statement.
   * @param[in] loop  The loop expression of the for statement.
   * @param[in] body  The body of the for loop.
   */
  public: ForStatement(SharedPtr<AstNode> const &init,
                       SharedPtr<AstNode> const &cond,
                       SharedPtr<AstNode> const &loop,
                       SharedPtr<Block> const &body);

  public: virtual ~ForStatement();


  //============================================================================
  // Member Functions

  /**
   * Get the initialization expression of the for loop.
   *
   * @return A pointer to the initialization expression of the for loop.
   */
  public: AstNode* getInitExpr() const
  {
    return this->init;
  }

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
   * Get the loop expression of the for loop.
   *
   * @return A pointer to the loop expression of the for loop.
   */
  public: AstNode* getLoopExpr() const
  {
    return this->loop;
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
    return 4;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    switch (index) {
      case 0: return this->initBlock.get();
      case 1: return this->condBlock.get();
      case 2: return this->loopBlock.get();
      case 3: return this->exitBlock.get();
      default: return 0;
    }
  }

}; // class

} // namespace

#endif

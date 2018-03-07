/**
 * @file Spp/LlvmCodeGen/Block.h
 * Contains the header of class Spp::LlvmCodeGen::Block.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_BLOCK_H
#define SPP_LLVMCODEGEN_BLOCK_H

namespace Spp { namespace LlvmCodeGen
{

class Block : public Core::Basic::TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Block, Core::Basic::TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::BasicBlock *llvmEntryBlock;
  private: llvm::BasicBlock *llvmBlock;

  private: llvm::IRBuilder<> *irBuilder;

  private: llvm::Function *llvmFunc;

  private: Bool terminated;


  //============================================================================
  // Constructor & Destructor

  public: Block() : llvmEntryBlock(0), llvmBlock(0), irBuilder(0), llvmFunc(0), terminated(false)
  {
  }


  //============================================================================
  // Member Functions

  public: void setLlvmBlock(llvm::BasicBlock *block)
  {
    if (this->llvmEntryBlock == 0) this->llvmEntryBlock = block;
    this->llvmBlock = block;
  }
  public: llvm::BasicBlock* getLlvmBlock() const
  {
    return this->llvmBlock;
  }
  public: llvm::BasicBlock* getLlvmEntryBlock() const
  {
    return this->llvmEntryBlock;
  }

  public: void setIrBuilder(llvm::IRBuilder<> *builder)
  {
    this->irBuilder = builder;
  }
  public: llvm::IRBuilder<>* getIrBuilder() const
  {
    return this->irBuilder;
  }

  public: void setLlvmFunction(llvm::Function *f)
  {
    this->llvmFunc = f;
  }
  public: llvm::Function* getLlvmFunction() const
  {
    return this->llvmFunc;
  }

  public: void setTerminated(Bool t = true)
  {
    this->terminated = t;
  }
  public: Bool isTerminated() const
  {
    return this->terminated;
  }

}; // class

} } // namespace

#endif

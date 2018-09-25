/**
 * @file Spp/LlvmCodeGen/LoopContext.h
 * Contains the header of class Spp::LlvmCodeGen::LoopContext.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_LOOPCONTEXT_H
#define SPP_CODEGEN_LOOPCONTEXT_H

namespace Spp { namespace LlvmCodeGen
{

class LoopContext : public CodeGen::LoopTgContext
{
  //============================================================================
  // Type Info

  TYPE_INFO(LoopContext, CodeGen::LoopTgContext, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: SharedPtr<Block> conditionBlock;
  private: SharedPtr<Block> updaterBlock;
  private: SharedPtr<Block> bodyBlock;
  private: SharedPtr<Block> exitBlock;


  //============================================================================
  // Constructor & Destructor

  public: LoopContext()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual TiObject* getConditionContext() const
  {
    return this->conditionBlock.get();
  }

  public: virtual TiObject* getUpdaterContext() const
  {
    return this->updaterBlock.get();
  }

  public: virtual TiObject* getBodyContext() const
  {
    return this->bodyBlock.get();
  }

  public: void setConditionBlock(SharedPtr<Block> const &cb)
  {
    this->conditionBlock = cb;
  }

  public: SharedPtr<Block> const& getConditionBlock() const
  {
    return this->conditionBlock;
  }

  public: void setUpdaterBlock(SharedPtr<Block> const &ub)
  {
    this->updaterBlock = ub;
  }

  public: SharedPtr<Block> const& getUpdaterBlock() const
  {
    return this->updaterBlock;
  }

  public: void setBodyBlock(SharedPtr<Block> const &bb)
  {
    this->bodyBlock = bb;
  }

  public: SharedPtr<Block> const& getBodyBlock() const
  {
    return this->bodyBlock;
  }

  public: void setExitBlock(SharedPtr<Block> const &eb)
  {
    this->exitBlock = eb;
  }

  public: SharedPtr<Block> const& getExitBlock() const
  {
    return this->exitBlock;
  }

}; // class

} } // namespace

#endif

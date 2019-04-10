/**
 * @file Spp/LlvmCodeGen/IfContext.h
 * Contains the header of class Spp::LlvmCodeGen::IfContext.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_IFCONTEXT_H
#define SPP_CODEGEN_IFCONTEXT_H

namespace Spp { namespace LlvmCodeGen
{

class IfContext : public CodeGen::IfTgContext
{
  //============================================================================
  // Type Info

  TYPE_INFO(IfContext, CodeGen::IfTgContext, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: SharedPtr<Block> conditionBlock;
  private: SharedPtr<Block> bodyBlock;
  private: SharedPtr<Block> elseBlock;


  //============================================================================
  // Constructor & Destructor

  public: IfContext()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual TiObject* getConditionContext() const
  {
    return this->conditionBlock.get();
  }

  public: virtual TiObject* getBodyContext() const
  {
    return this->bodyBlock.get();
  }

  public: virtual TiObject* getElseContext() const
  {
    return this->elseBlock.get();
  }

  public: void setConditionBlock(SharedPtr<Block> const &cb)
  {
    this->conditionBlock = cb;
  }

  public: SharedPtr<Block> const& getConditionBlock() const
  {
    return this->conditionBlock;
  }

  public: void setBodyBlock(SharedPtr<Block> const &bb)
  {
    this->bodyBlock = bb;
  }

  public: SharedPtr<Block> const& getBodyBlock() const
  {
    return this->bodyBlock;
  }

  public: void setElseBlock(SharedPtr<Block> const &eb)
  {
    this->elseBlock = eb;
  }

  public: SharedPtr<Block> const& getElseBlock() const
  {
    return this->elseBlock;
  }

}; // class

} } // namespace

#endif

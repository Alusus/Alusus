/**
 * @file Spp/LlvmCodeGen/LazyJitBuildTarget.h
 * Contains the header of class Spp::LlvmCodeGen::LazyJitBuildTarget.
 *
 * @copyright Copyright (C) 2021 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_LAZYJITBUILDTARGET_H
#define SPP_LLVMCODEGEN_LAZYJITBUILDTARGET_H

namespace Spp::LlvmCodeGen
{

class LazyJitBuildTarget : public BuildTarget
{
  //============================================================================
  // Type Info

  TYPE_INFO(LazyJitBuildTarget, BuildTarget, "Spp.LlvmCodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: std::unique_ptr<LazyJitEngine> llvmJitEngine;
  private: std::unique_ptr<llvm::orc::ThreadSafeContext> llvmTsContext;
  private: llvm::LLVMContext *llvmContext = 0;
  private: llvm::DataLayout *llvmDataLayout = 0;
  private: std::unique_ptr<llvm::Module> llvmModule;

  private: CodeGen::GlobalItemRepo *globalItemRepo = 0;


  //============================================================================
  // Constructors & Destructor

  public: LazyJitBuildTarget(CodeGen::GlobalItemRepo *gir) : globalItemRepo(gir)
  {
  }

  public: virtual ~LazyJitBuildTarget()
  {
    this->llvmJitEngine.reset();
    this->llvmModule.reset();
    this->llvmTsContext.reset();
  }


  //============================================================================
  // Member Functions

  public: virtual void setupBuild();

  public: virtual llvm::DataLayout* getLlvmDataLayout()
  {
    return this->llvmDataLayout;
  }

  public: virtual llvm::LLVMContext* getLlvmContext()
  {
    return this->llvmContext;
  }

  public: virtual llvm::Module* getGlobalLlvmModule();

  public: Word getPointerBitCount()
  {
    return this->llvmDataLayout->getPointerSizeInBits();
  }

  public: virtual void addLlvmModule(std::unique_ptr<llvm::Module> module);

  public: void execute(Char const *entry);

}; // class

} // namespace

#endif

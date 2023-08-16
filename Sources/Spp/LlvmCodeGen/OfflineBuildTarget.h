/**
 * @file Spp/LlvmCodeGen/OfflineBuildTarget.h
 * Contains the header of class Spp::LlvmCodeGen::OfflineBuildTarget.
 *
 * @copyright Copyright (C) 2023 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_OFFLINEBUILDTARGET_H
#define SPP_LLVMCODEGEN_OFFLINEBUILDTARGET_H

namespace Spp::LlvmCodeGen
{

class OfflineBuildTarget : public BuildTarget
{
  //============================================================================
  // Type Info

  TYPE_INFO(OfflineBuildTarget, BuildTarget, "Spp.LlvmCodeGen", "Spp", "alusus.org");


  //============================================================================
  // Types

  private: struct LlvmGlobalCtorDtorEntryTypes
  {
    llvm::PointerType *llvmFuncPtrType = 0;
    llvm::PointerType *llvmIntPtrType = 0;
    llvm::StructType *llvmStructType = 0;
  };


  //============================================================================
  // Member Variables

  private: std::string targetTriple;
  private: std::unique_ptr<llvm::TargetMachine> targetMachine;
  private: std::unique_ptr<llvm::DataLayout> llvmDataLayout;
  private: std::unique_ptr<llvm::LLVMContext> llvmContext;
  private: std::unique_ptr<llvm::Module> llvmModule;
  private: LlvmGlobalCtorDtorEntryTypes llvmGlobalCtorDtorEntryTypes;


  //============================================================================
  // Constructors & Destructor

  public: OfflineBuildTarget()
  {
    this->setTargetTriple(0);
  }

  public: virtual ~OfflineBuildTarget()
  {
    this->llvmModule.reset();
    this->llvmContext.reset();
  }


  //============================================================================
  // Member Functions

  public: void setTargetTriple(Char const *tt)
  {
    if (tt == 0) this->targetTriple = llvm::sys::getDefaultTargetTriple();
    else this->targetTriple = tt;
  }

  public: std::string const& getTargetTriple() const
  {
    return this->targetTriple;
  }

  public: virtual void setupBuild();

  public: virtual llvm::DataLayout* getLlvmDataLayout()
  {
    return this->llvmDataLayout.get();
  }

  public: virtual llvm::LLVMContext* getLlvmContext()
  {
    return this->llvmContext.get();
  }

  public: virtual llvm::Module* getGlobalLlvmModule();

  public: Word getPointerBitCount()
  {
    return this->llvmDataLayout->getPointerSizeInBits();
  }

  public: virtual void addLlvmModule(std::unique_ptr<llvm::Module> module);

  public: Str generateLlvmIr(Array<Str> const *ctorNames, Array<Str> const *dtorNames);

  public: void generateObjectFile(
    Char const *filename, Array<Str> const *ctorNames, Array<Str> const *dtorNames
  );

  private: void buildCtorOrDtorArray(Array<Str> const *funcNames, Char const *globalVarName);

}; // class

} // namespace

#endif

/**
 * @file Spp/LlvmCodeGen/BuildTarget.h
 * Contains the header of class Spp::LlvmCodeGen::BuildTarget.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_BUILDTARGET_H
#define SPP_LLVMCODEGEN_BUILDTARGET_H

namespace Spp::LlvmCodeGen
{

class BuildTarget : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(BuildTarget, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.org");

  //============================================================================
  // Member Variables

  private: llvm::StructType *vaListType = 0;
  private: Bool debugInfo = false;


  //============================================================================
  // Member Functions

  /// Whether to generate debug info (DWARF) for the generated code.
  public: void setDebugInfo(Bool d)
  {
    this->debugInfo = d;
  }

  public: Bool isDebugInfoEnabled() const
  {
    return this->debugInfo;
  }

  public: virtual void setupBuild()
  {
    this->vaListType = 0;
  }

  /// Returns the debug info state of the global module, or null if debug info is disabled.
  public: virtual ModuleDebugInfo* getDebugInfo()
  {
    return 0;
  }

  /// The triple of the target the generated code is for. The JIT targets always build for the host.
  public: virtual std::string getTargetTripleName()
  {
    return llvm::sys::getDefaultTargetTriple();
  }

  /// Whether the generated code is optimized, which is recorded in the debug info.
  public: virtual Bool isOptimized() const
  {
    return false;
  }

  public: virtual llvm::DataLayout* getLlvmDataLayout() = 0;

  public: virtual llvm::LLVMContext* getLlvmContext() = 0;

  public: virtual llvm::Module* getGlobalLlvmModule() = 0;

  public: virtual Word getPointerBitCount()
  {
    return this->getLlvmDataLayout()->getPointerSizeInBits();
  }

  public: virtual void addLlvmModule(std::unique_ptr<llvm::Module> module) = 0;

  /// Whether the target follows the x86-64 System V calling convention. The JIT targets always build for the host.
  public: virtual Bool isSysVX86_64Abi()
  {
    llvm::Triple tt(llvm::sys::getDefaultTargetTriple());
    return tt.getArch() == llvm::Triple::x86_64 && !tt.isOSWindows();
  }

  public: virtual llvm::Type* getVaListType();

}; // class

} // namespace

#endif

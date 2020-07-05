/**
 * @file Spp/LlvmCodeGen/BuildTarget.h
 * Contains the header of class Spp::LlvmCodeGen::BuildTarget.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
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


  //============================================================================
  // Member Functions

  public: virtual void setupBuild()
  {
    this->vaListType = 0;
  }

  public: virtual llvm::DataLayout* getLlvmDataLayout() = 0;

  public: virtual llvm::LLVMContext* getLlvmContext() = 0;

  public: virtual llvm::Module* getGlobalLlvmModule() = 0;

  public: virtual Word getPointerBitCount()
  {
    return this->getLlvmDataLayout()->getPointerSizeInBits();
  }

  public: virtual void addLlvmModule(std::unique_ptr<llvm::Module> module) = 0;

  public: virtual llvm::Type* getVaListType();

}; // class

} // namespace

#endif

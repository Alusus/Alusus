/**
 * @file Scg/LlvmContainer.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __LlvmContainer_h__
#define __LlvmContainer_h__

namespace llvm
{
  class LLVMContext;
  class DataLayout;
}

namespace Scg
{
  class LlvmContainer
  {
    public: static llvm::DataLayout *dataLayout;

  public:
    static void Initialize();
    static void Finalize();
    static llvm::LLVMContext &GetContext();
    static llvm::DataLayout* getDataLayout();
  };
}

#endif // __LlvmContainer_h__

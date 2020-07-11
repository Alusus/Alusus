/**
 * @file Spp/LlvmCodeGen/JitBuildTarget.cpp
 * Contains the implementation of class Spp::LlvmCodeGen::JitBuildTarget.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::LlvmCodeGen
{

void JitBuildTarget::setupBuild()
{
  this->llvmJitEngine.reset();

  this->llvmJitEngine = llvm::cantFail(JitEngineBuilder().create(this->globalItemRepo));
  this->llvmDataLayout = const_cast<llvm::DataLayout*>(&this->llvmJitEngine->getDataLayout());

  this->llvmModule.reset();

  this->llvmTsContext = std::make_unique<llvm::orc::ThreadSafeContext>(std::make_unique<llvm::LLVMContext>());
  this->llvmContext = this->llvmTsContext->getContext();

  #ifdef USE_LOGS
    if (Core::Basic::Logger::getFilter() & Spp::LogLevel::LLVMCODEGEN_DIAGNOSTIC) {
      this->llvmContext->setDiagnosticHandlerCallBack(&llvmDiagnosticCallback);
    }
  #endif
}


llvm::Module* JitBuildTarget::getGlobalLlvmModule()
{
  if (this->llvmModule == 0) {
    this->llvmModule = std::make_unique<llvm::Module>("AlususProgram", *this->llvmContext);
    this->llvmModule->setDataLayout(*this->llvmDataLayout);
  }
  return this->llvmModule.get();
}


void JitBuildTarget::addLlvmModule(std::unique_ptr<llvm::Module> module)
{
  #ifdef USE_LOGS
    if (Core::Basic::Logger::getFilter() & Spp::LogLevel::LLVMCODEGEN_IR) {
      // Dump the module to be compiled.
      outStream << S(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
      llvm::raw_os_ostream ostream(outStream);
      llvm::createPrintModulePass(ostream)->runOnModule(*(module));
      outStream << S("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    }
  #endif

  // Compile the module.
  this->llvmJitEngine->addIRModule(
    llvm::orc::ThreadSafeModule(std::move(module), *this->llvmTsContext)
  );
}


void JitBuildTarget::execute(Char const *entry)
{
  if (this->llvmModule != 0) this->addLlvmModule(std::move(this->llvmModule));

  typedef void (*FuncType)();
  auto llvmEntry = llvm::cantFail(this->llvmJitEngine->lookup(entry));
  auto funcPtr = (FuncType)llvmEntry.getAddress();

  funcPtr();
}

} // namespace

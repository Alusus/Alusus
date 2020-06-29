/**
 * @file Spp/LlvmCodeGen/llvm_dependencies.h
 * Contains include statement for LLVM dependencies.
 *
 * @copyright Copyright (C) 2020 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef SCG_CODEGENUNIT_H
#define SCG_CODEGENUNIT_H

#undef C
#undef S

#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/Mangler.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/DiagnosticPrinter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/ThreadPool.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/IRTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h>
#include <llvm/ExecutionEngine/Orc/ObjectTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>

#define C(x)	u8##x
#define S(x)	u8##x


DEFINE_TYPE_NAME(llvm::Module, "llvm.org/LLVM/llvm.Module");
DEFINE_TYPE_NAME(llvm::Function, "llvm.org/LLVM/llvm.Function");
DEFINE_TYPE_NAME(llvm::Type, "llvm.org/LLVM/llvm.Type");
DEFINE_TYPE_NAME(llvm::Value, "llvm.org/LLVM/llvm.Value");
DEFINE_TYPE_NAME(llvm::IRBuilder<>, "llvm.org/LLVM/llvm.IRBuilder<>");
DEFINE_TYPE_NAME(llvm::Constant, "llvm.org/LLVM/llvm.Constant");

#endif

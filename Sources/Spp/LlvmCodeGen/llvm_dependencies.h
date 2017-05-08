/**
 * @file Spp/LlvmCodeGen/llvm_dependencies.h
 * Contains include statement for LLVM dependencies.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CODEGENUNIT_H
#define SCG_CODEGENUNIT_H

#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/Linker.h>
#include <llvm/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

DEFINE_TYPE_NAME(llvm::Module, "llvm.org/LLVM/llvm.Module");
DEFINE_TYPE_NAME(llvm::Function, "llvm.org/LLVM/llvm.Function");
DEFINE_TYPE_NAME(llvm::Type, "llvm.org/LLVM/llvm.Type");

#endif

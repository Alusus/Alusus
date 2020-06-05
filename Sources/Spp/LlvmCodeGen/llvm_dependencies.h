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

#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Linker/Linker.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Transforms/Utils/Cloning.h>

#undef C
#define C(x)	u8##x


DEFINE_TYPE_NAME(llvm::Module, "llvm.org/LLVM/llvm.Module");
DEFINE_TYPE_NAME(llvm::Function, "llvm.org/LLVM/llvm.Function");
DEFINE_TYPE_NAME(llvm::Type, "llvm.org/LLVM/llvm.Type");
DEFINE_TYPE_NAME(llvm::Value, "llvm.org/LLVM/llvm.Value");
DEFINE_TYPE_NAME(llvm::IRBuilder<>, "llvm.org/LLVM/llvm.IRBuilder<>");
DEFINE_TYPE_NAME(llvm::Constant, "llvm.org/LLVM/llvm.Constant");

#endif

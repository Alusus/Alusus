/**
 * @file Scg/DebuggingHelpers/llvm_helper.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __DebuggingHelpers_llvm_helpers_h__
#define __DebuggingHelpers_llvm_helpers_h__

#include <llvm_fwd.h>

namespace Scg
{
void printLlvmValue(llvm::Value *value);
void printLlvmValueType(llvm::Value *value);
}

#endif // __DebuggingHelpers_llvm_helpers_h__

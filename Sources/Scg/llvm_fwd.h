/**
 * @file Scg/llvm_fwd.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __llvm_fwd_h__
#define __llvm_fwd_h__

namespace llvm
{
  class AllocaInst;
  class Argument;
  class BasicBlock;
  class BranchInst;
  class CallInst;
  class CastInst;
  class ConstantFolder;
  class Function;
  class GetElementPtrInst;
  class GlobalVariable;
  class ICmpInst;
  class Instruction;
  class LoadInst;
  class Module;
  class ReturnInst;
  class StoreInst;
  class Type;
  class Value;
}

#endif // __llvm_fwd_h__

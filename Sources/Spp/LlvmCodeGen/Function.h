/**
 * @file Spp/LlvmCodeGen/Function.h
 * Contains the header of class Spp::LlvmCodeGen::Function.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_FUNCTION_H
#define SPP_LLVMCODEGEN_FUNCTION_H

namespace Spp { namespace LlvmCodeGen
{

class Function : public Core::Basic::TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Function, Core::Basic::TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Functions

  public: virtual llvm::Value* createCallInstruction(llvm::IRBuilder<> *irb, std::vector<llvm::Value*> const &args) = 0;

}; // class

} } // namespace

#endif

/**
 * @file Spp/LlvmCodeGen/Value.h
 * Contains the header of class Spp::LlvmCodeGen::Value.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_VALUE_H
#define SPP_LLVMCODEGEN_VALUE_H

namespace Spp { namespace LlvmCodeGen
{

class Value : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Value, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: llvm::Value *llvmValue;

  private: Bool constant;


  //============================================================================
  // Constructor & Destructor

  public: Value(llvm::Value *val = 0, Bool c = false) : llvmValue(val), constant(c)
  {
  }


  //============================================================================
  // Member Functions

  public: void setLlvmValue(llvm::Value *val)
  {
    this->llvmValue = val;
  }

  public: llvm::Value* getLlvmValue() const
  {
    return this->llvmValue;
  }

  public: llvm::Constant* getLlvmConstant() const
  {
    if (this->constant) {
      return static_cast<llvm::Constant*>(this->llvmValue);
    } else {
      throw EXCEPTION(GenericException, S("Value is not a constant."));
    }
  }

  public: void setConstant(Bool c)
  {
    this->constant = c;
  }

  public: Bool isConstant() const
  {
    return this->constant;
  }

}; // class

} } // namespace

#endif

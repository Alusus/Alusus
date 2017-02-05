/**
 * @file Spp/LlvmCodeGen/FloatType.h
 * Contains the header of class Spp::LlvmCodeGen::FloatType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_FLOATTYPE_H
#define SPP_LLVMCODEGEN_FLOATTYPE_H

namespace Spp { namespace LlvmCodeGen
{

//! Represent the integer type.
class FloatType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(FloatType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: Int bitCount;


  //============================================================================
  // Construtors & Destructor

  public: FloatType(Int bits);


  //============================================================================
  // Member Functions

  public: Int getBitCount() const
  {
    return this->bitCount;
  }

  public: llvm::Constant* getLlvmConstant(double value) const
  {
    return llvm::ConstantFP::get(this->llvmType, value);
  }

  public: virtual llvm::Constant* getDefaultLlvmValue() const
  {
    return this->getLlvmConstant(0);
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, ExecutionContext const *context) const;

  public: virtual Bool isExplicitlyCastableTo(Type const *type, ExecutionContext const *context) const;

}; // class

} } // namespace

#endif

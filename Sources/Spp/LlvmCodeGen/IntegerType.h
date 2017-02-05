/**
 * @file Spp/LlvmCodeGen/IntegerType.h
 * Contains the header of class Spp::LlvmCodeGen::IntegerType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_INTEGERTYPE_H
#define SPP_LLVMCODEGEN_INTEGERTYPE_H

namespace Spp { namespace LlvmCodeGen
{

//! Represent the integer type.
class IntegerType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(IntegerType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: Int bitCount;


  //============================================================================
  // Construtors & Destructor

  public: IntegerType(Int bits) : bitCount(bits)
  {
    this->llvmType = llvm::Type::getIntNTy(llvm::getGlobalContext(), bits);
  }


  //============================================================================
  // Member Functions

  public: Int getBitCount() const
  {
    return this->bitCount;
  }

  public: llvm::Constant* getLlvmConstant(int value) const
  {
    return llvm::ConstantInt::get(static_cast<llvm::IntegerType*>(this->llvmType), value);
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

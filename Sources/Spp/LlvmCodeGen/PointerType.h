/**
 * @file Spp/LlvmCodeGen/PointerType.h
 * Contains the header of class Spp::LlvmCodeGen::PointerType.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_POINTERTYPE_H
#define SPP_LLVMCODEGEN_POINTERTYPE_H

namespace Spp { namespace LlvmCodeGen
{

class PointerType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(PointerType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: Type const *contentType;


  //============================================================================
  // Constructors & Destructor

  public: PointerType(Type const *ct) : contentType(ct)
  {
    this->llvmType = this->contentType->getLlvmType()->getPointerTo();
  }


  //============================================================================
  // Member Functions

  public: Type const* getContentType() const
  {
    return this->contentType;
  }

  public: virtual llvm::Constant* getDefaultLlvmValue() const
  {
    return llvm::ConstantPointerNull::get(static_cast<llvm::PointerType*>(this->llvmType));
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, ExecutionContext const *context) const
  {
    auto pointerType = ti_cast<PointerType>(type);
    if (pointerType != 0 && pointerType->getContentType() == 0) return true;
    else return false;
  }

  public: virtual Bool isExplicitlyCastableTo(Type const *type, ExecutionContext const *context) const
  {
    return type->isDerivedFrom<PointerType>();
  }

}; // class

} } // namespace

#endif

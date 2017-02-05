/**
 * @file Spp/LlvmCodeGen/Type.h
 * Contains the header of class Spp::LlvmCodeGen::Type.
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_TYPE_H
#define SPP_LLVMCODEGEN_TYPE_H

namespace Spp { namespace LlvmCodeGen
{

class Type : public Core::Basic::TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Type, Core::Basic::TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  protected: llvm::Type *llvmType;


  //============================================================================
  // Constructors & Destructor

  protected: Type() : llvmType(0)
  {
  }

  public: virtual ~Type()
  {
  }


  //============================================================================
  // Member Functions

  public: llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: virtual llvm::Constant* getDefaultLlvmValue() const = 0;

  public: virtual llvm::Value* createCastInst(llvm::IRBuilder<> *irb, llvm::Value *value, Type const *targetType) const
  {
    throw EXCEPTION(GenericException, STR("Not implemented."));
  }

  public: virtual Bool isImplicitlyCastableTo(Type const *type, ExecutionContext const *context) const = 0;

  public: virtual Bool isExplicitlyCastableTo(Type const *type, ExecutionContext const *context) const = 0;

}; // class

} } // namespace

#endif

/**
 * @file Spp/LlvmCodeGen/types.h
 * Contains definitions for type classes.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_LLVMCODEGEN_TYPES_H
#define SPP_LLVMCODEGEN_TYPES_H

namespace Spp::LlvmCodeGen
{

//==============================================================================
// Type

class Type : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Type, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const = 0;

}; // class


//==============================================================================
// VoidType

class VoidType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(VoidType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::Type *llvmType;


  //============================================================================
  // Constructor & Destructor

  public: VoidType(llvm::Type *t) : llvmType(t)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

}; // class


//==============================================================================
// IntegerType

class IntegerType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(IntegerType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::IntegerType *llvmType;
  private: Word size;
  private: Bool withSign;


  //============================================================================
  // Constructor & Destructor

  public: IntegerType(llvm::IntegerType *t, Word s, Bool ws) : llvmType(t), size(s), withSign(ws)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: Word getSize() const
  {
    return this->size;
  }

  public: Bool isSigned() const
  {
    return this->withSign;
  }

}; // class


//==============================================================================
// FloatType

class FloatType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(FloatType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::Type *llvmType;
  private: Word size;


  //============================================================================
  // Constructor & Destructor

  public: FloatType(llvm::Type *t, Word s) : llvmType(t), size(s)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: Word getSize() const
  {
    return this->size;
  }

}; // class


//==============================================================================
// PointerType

class PointerType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(PointerType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::PointerType *llvmType;
  private: SharedPtr<Type> contentType;


  //============================================================================
  // Constructor & Destructor

  public: PointerType(llvm::PointerType *t, SharedPtr<Type> ct) : llvmType(t), contentType(ct)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: SharedPtr<Type> const& getContentType() const
  {
    return this->contentType;
  }

}; // class


//==============================================================================
// ArrayType

class ArrayType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(ArrayType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::ArrayType *llvmType;
  private: SharedPtr<Type> contentType;
  private: Word size;


  //============================================================================
  // Constructor & Destructor

  public: ArrayType(llvm::ArrayType *t, SharedPtr<Type> const &ct, Word s) : llvmType(t), contentType(ct), size(s)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: SharedPtr<Type> const& getContentType() const
  {
    return this->contentType;
  }

  public: Word getSize() const
  {
    return this->size;
  }

}; // class


//==============================================================================
// StructType

class StructType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(StructType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::StructType *llvmType;
  private: Str name;


  //============================================================================
  // Constructor & Destructor

  public: StructType(llvm::StructType *t, Char const *n) : llvmType(t), name(n)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: Str const& getName() const
  {
    return this->name;
  }

}; // class


//==============================================================================
// FunctionType

class FunctionType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(FunctionType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.net");


  //============================================================================
  // Member Variables

  private: llvm::FunctionType *llvmType;
  private: SharedPtr<SharedMap<Type, TiObject>> args;
  private: SharedPtr<Type> retType;
  private: Bool variadic;


  //============================================================================
  // Constructor & Destructor

  public: FunctionType(
    llvm::FunctionType *ft, SharedPtr<SharedMap<Type, TiObject>> const &args, SharedPtr<Type> const &rt, Bool v
  ) : llvmType(ft), args(args), retType(rt), variadic(v)
  {
  }


  //============================================================================
  // Member Functions

  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: llvm::FunctionType* getLlvmFunctionType() const
  {
    return this->llvmType;
  }

  public: SharedPtr<SharedMap<Type, TiObject>> const& getArgs() const
  {
    return this->args;
  }

  public: SharedPtr<Type> const& getRetType() const
  {
    return this->retType;
  }

  public: Bool isVariadic() const
  {
    return this->variadic;
  }

}; // class

} // namespace

#endif

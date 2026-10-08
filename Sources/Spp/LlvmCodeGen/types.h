/**
 * @file Spp/LlvmCodeGen/types.h
 * Contains definitions for type classes.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
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

  TYPE_INFO(Type, TiObject, "Spp.LlvmCodeGen", "Spp", "alusus.org");


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

  TYPE_INFO(VoidType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.org");


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

  TYPE_INFO(IntegerType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.org");


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

  TYPE_INFO(FloatType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.org");


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

  TYPE_INFO(PointerType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.org");


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

  TYPE_INFO(ArrayType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.org");


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

  TYPE_INFO(StructType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.org");


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
// AbiInfo

/// Describes how a function argument or return value is passed at the LLVM level in order to follow the C ABI.
struct AbiInfo
{
  enum class Kind
  {
    /// Passed as is (non-struct types).
    DIRECT,
    /// Passed in memory: by pointer with the byval attribute for arguments, or by sret pointer for return values.
    INDIRECT,
    /// A small struct passed in registers, split into the types listed in `parts`.
    COERCED
  };

  Kind kind = Kind::DIRECT;
  /// For COERCED: the type of each eightbyte of the struct.
  std::vector<llvm::Type*> parts;
  /// Index of the first LLVM parameter that belongs to this argument.
  Int firstParam = 0;
  /// Number of LLVM parameters this argument occupies.
  Int paramCount = 1;
};


//==============================================================================
// FunctionType

class FunctionType : public Type
{
  //============================================================================
  // Type Info

  TYPE_INFO(FunctionType, Type, "Spp.LlvmCodeGen", "Spp", "alusus.org");


  //============================================================================
  // Member Variables

  private: llvm::FunctionType *llvmType;
  private: SharedPtr<SharedMap<Type>> args;
  private: SharedPtr<Type> retType;
  private: Bool variadic;
  private: AbiInfo retAbi;
  private: std::vector<AbiInfo> argAbis;


  //============================================================================
  // Constructor & Destructor

  public: FunctionType(
    llvm::FunctionType *ft, SharedPtr<SharedMap<Type>> const &args, SharedPtr<Type> const &rt, Bool v,
    AbiInfo const &retAbi, std::vector<AbiInfo> const &argAbis
  ) : llvmType(ft), args(args), retType(rt), variadic(v), retAbi(retAbi), argAbis(argAbis)
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

  public: SharedPtr<SharedMap<Type>> const& getArgs() const
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

  public: AbiInfo const& getRetAbi() const
  {
    return this->retAbi;
  }

  public: AbiInfo const& getArgAbi(Int index) const
  {
    return this->argAbis[index];
  }

  /// Number of leading LLVM parameters that are not Alusus arguments (the sret pointer).
  public: Int getHiddenParamCount() const
  {
    return this->retAbi.kind == AbiInfo::Kind::INDIRECT ? 1 : 0;
  }

}; // class

} // namespace

#endif

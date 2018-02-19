/**
 * @file Spp/LlvmCodeGen/Type.h
 * Contains the header of class Spp::LlvmCodeGen::Type.
 *
 * @copyright Copyright (C) 2018 Sarmad Khalid Abdullah
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
  // Types

  public: s_enum(Category, INT, FLOAT, POINTER, ARRAY, STRUCT, VOID);


  //============================================================================
  // Member Variables

  private: Category category;
  private: llvm::Type *llvmType;
  private: Word size;
  private: Type *relatedType;
  private: Str name;


  //============================================================================
  // Constructor & Destructor

  public: Type(Category c, llvm::Type *t, Word s = 0) : category(c), llvmType(t), size(s), relatedType(0)
  {
  }

  public: Type(Category c, llvm::Type *t, Type *rt, Word s = 0) : category(c), llvmType(t), relatedType(rt), size(s)
  {
  }

  public: Type(Category c, llvm::Type *t, Type *rt, Char const *n)
    : category(c), llvmType(t), relatedType(rt), size(0), name(n)
  {
  }


  //============================================================================
  // Member Functions

  public: Category getCategory() const
  {
    return this->category;
  }

  public: llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  public: Word getSize() const
  {
    return this->size;
  }

  public: Type* getRelatedType() const
  {
    return this->relatedType;
  }

  public: Str const& getName() const
  {
    return this->name;
  }

}; // class

} } // namespace

#endif

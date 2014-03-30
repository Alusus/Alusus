/**
 * @file Scg/Types/ValueType.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __ValueType_h__
#define __ValueType_h__

// Scg header files
#include <Expression.h>
#include <typedefs.h>

// LLVM forward declarations
#include <llvm_fwd.h>

namespace Scg
{
// TODO: Should ValueType be derived from Expression? I can't think of any
// advantage in that, but I don't remember I decided to derive it from
// Expression.
/**
 * Represent a value type, e.g. IntegerType.
 */
class ValueType : public Expression
{
protected:
  //! The LLVM type representing this type.
  // TODO: Rename to llvmType for consistency.
  // TODO: Can we change the type to llvm::Type& without having to include
  // LLVM header file for the Type class?
  llvm::Type *llvmType;
  //! The number of variables of this type defined.
  mutable int varCount;

protected:
  //! Default constructor
  ValueType() : llvmType(0), varCount(0) {}

public:
  //! Class destructor
  virtual ~ValueType() {}

  // Disables copy constructor and assignment.
  ValueType & operator = (const ValueType &source) = delete;
  ValueType(const ValueType &source) = delete;

public:
  /**
   * Allocates a new variable of this type.
   * @param[in] name  The name of the variable.
   * @return A pointer to the variable.
   */
  virtual Variable *NewVariable(const std::string &name,
      llvm::Argument *llvmArgument = nullptr) const;

  /**
   * Deletes the variable defined by NewVariable() method.
   * @param[in] var   A pointer to the variable.
   */
  virtual void DeleteVariable(Variable *var) const;

  /**
   * Retrieves the name of the type, e.g. int, float, etc.
   *
   * @return The name of the type.
   */
  virtual const std::string GetName() const = 0;

  /**
   * Return the number of variables of this type defined.
   * @return The number of variables of this type defined.
   */
  int GetVarCount() { return varCount; }

  /**
   * Get the LLVM Type object representing this type. This is used to define
   * variables of this type in LLVM.
   *
   * @return A pointer to the LLVM object representing this type.
   */
  virtual const llvm::Type *GetLlvmType() const { return llvmType; }
  virtual llvm::Type *GetLlvmType()
  {
    return const_cast<llvm::Type*>(
        static_cast<const ValueType*>(this)->GetLlvmType());
  }

  /**
   * Retrieves the default value of this type as an LLVM Constant object.
   * @return The LLVM Constant object representing the default value.
   */
  virtual llvm::Constant *GetDefaultLLVMValue() const = 0;

  /**
   * Retrieves the type specification of this type.
   * @return A pointer to the type specification of this type. This gets
   * automatically freed by the type, hence shouldn't be freed by the caller.
   */
  virtual const ValueTypeSpec *GetValueTypeSpec() const = 0;

  /**
   * Gets a primitive type having the given name.
   *
   * @param[in] typeName  The name of the type (e.g. "Float".)
   *
   * @return A pointer to the type.
   */
  static ValueType *GetPrimitiveType(const std::string &typeName);
};
}

#endif // __ValueType_h__

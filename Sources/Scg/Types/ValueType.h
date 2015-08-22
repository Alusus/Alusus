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

// SCG header files
#include <Expression.h>
#include <typedefs.h>

// LLVM header files
#pragma warning(disable: 4146 4800 4355 4996)

#include <llvm/IR/IRBuilder.h>

#pragma warning(default: 4146 4800 4355 4996)

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
  mutable std::vector<const ValueType *> implicitCastingTargets;
  mutable std::vector<const ValueType *> explicitCastingTargets;
  mutable bool castingTargetsInitialized = false;

protected:
  //! Default constructor
  ValueType() : llvmType(0), varCount(0)
  {
  }

public:
  //! Class destructor
  virtual ~ValueType()
  {
  }

  // Disables copy constructor and assignment.
  ValueType &operator=(const ValueType &source) = delete;

  ValueType(const ValueType &source) = delete;

protected:
  /**
  * Called by getImplicitCastingTargets() getExplicitCastingTargets() to
  * allow the type to initialize implicitCastingTargets and
  * explicitCastingTargets .
  */
  virtual void initCastingTargets() const = 0;

public:
  /**
  * Allocates a new variable of this type.
  * @param[in] name  The name of the variable.
  * @return A pointer to the variable.
  */
  virtual Variable *newVariable(const std::string &name,
                                llvm::Argument *llvmArgument = nullptr) const;

  /**
  * Deletes the variable defined by newVariable() method.
  * @param[in] var   A pointer to the variable.
  */
  virtual void deleteVariable(Variable *var) const;

  /**
  * Retrieves the name of the type, e.g. int, float, etc.
  *
  * @return The name of the type.
  */
  virtual const std::string getName() const = 0;

  /// Get the number of bytes needed to store variables of this type.
public:
  virtual Int getAllocationSize() const;

  /**
  * Return the number of variables of this type defined.
  * @return The number of variables of this type defined.
  */
  int getVarCount() const
  {
    return varCount;
  }

  /**
  * Get the LLVM Type object representing this type. This is used to define
  * variables of this type in LLVM.
  *
  * @return A pointer to the LLVM object representing this type.
  */
  virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  /**
  * Retrieves the default value of this type as an LLVM Constant object.
  * @return The LLVM Constant object representing the default value.
  */
  virtual llvm::Constant *getDefaultLLVMValue() const = 0;

  /**
  * Retrieves the type specification of this type.
  * @return A pointer to the type specification of this type. This gets
  * automatically freed by the type, hence shouldn't be freed by the caller.
  */
  virtual const ValueTypeSpec *getValueTypeSpec() const = 0;

  virtual ValueTypeSpec *getValueTypeSpec()
  {
    return const_cast<ValueTypeSpec *>(
             static_cast<const ValueType *>(this)->getValueTypeSpec());
  }

  /**
  * Casts the given LLVM value to the given target type.
  * @param[in] irb         The IRBuilder object used to create the instruction.
  * @param[in] value       The LLVM value to be casted.
  * @param[in] targetType  The target type.
  * @return The casted LLVM value. This is usually an instruction that does
  * the casting having the given LLVM value as its argument.
  */
  virtual llvm::Value *createCastInst(llvm::IRBuilder<> *irb,
                                      llvm::Value *value, const ValueType *targetType) const
  {
    THROW_NOT_IMPLEMENTED();
  }

  /**
  * Determines whether this type is equal to the given type.
  * @param[in] other The type.
  * @return @c true or @c false.
  */
  virtual bool isEqualTo(const ValueType *other) const = 0;

  /**
  * Determines whether this type can be implicitly casted to the given type.
  * @param[in] type The type.
  * @return @c true or @c false.
  */
  bool isImplicitlyCastableTo(const ValueType *type) const
  {
    // PERFORMANCE: Consider changing getImplicitCastingTargets() to return
    // a set instead of a vector so that we can find elements in O(1).
    for (auto t : getImplicitCastingTargets()) {
      if (t == type) {
        return true;
      }
    }

    return false;
  }

  /**
  * Determines whether this type can be explicitly casted to the given type.
  * @param[in] type The type.
  * @return @c true or @c false.
  */
  bool isExplicitlyCastableTo(const ValueType *type) const
  {
    // PERFORMANCE: Consider changing getImplicitCastingTargets() to return
    // a set instead of a vector so that we can find elements in O(1).
    for (auto t : getExplicitCastingTargets()) {
      if (t == type) {
        return true;
      }
    }

    return false;
  }

  /**
  * Compares this type against the given type.
  * @param[in] other The type to compare against.
  * @return One of the results of Scg::TypeComparisonResult.
  */
  virtual TypeComparisonResult compare(const ValueType *other)
  {
    if (isEqualTo(other)) {
      return TypeComparisonResult::Equivalent;
    } else if (isImplicitlyCastableTo(other)) {
      return TypeComparisonResult::ImplicitlyEquivalent;
    } else if (isExplicitlyCastableTo(other)) {
      return TypeComparisonResult::ExplicitlyEquivalent;
    } else {
      return TypeComparisonResult::NotEquivalent;
    }
  }

  /**
  * Retrieves a list of the types that this type can be implicitly casted to.
  * @return A list of the types that this type can be implicitly casted to.
  */
  virtual const std::vector<const ValueType *> &getImplicitCastingTargets() const
  {
    if (!this->castingTargetsInitialized) {
      this->castingTargetsInitialized = true;
      this->initCastingTargets();
    }

    return this->implicitCastingTargets;
  }

  /**
  * Retrieves a list of the types that this type can be explicitly casted to.
  * @return A list of the types that this type can be explicitly casted to.
  */
  virtual const std::vector<const ValueType *> &getExplicitCastingTargets() const
  {
    if (!this->castingTargetsInitialized) {
      this->castingTargetsInitialized = true;
      this->initCastingTargets();
    }

    return this->explicitCastingTargets;
  }

  /**
  * Gets a primitive type having the given name.
  *
  * @param[in] typeName  The name of the type (e.g. "Float".)
  *
  * @return A pointer to the type.
  */
  static ValueType *getPrimitiveType(const std::string &typeName);

public:
  static void *operator new[](size_t size) = delete;

  static void operator delete[](void *ptr) = delete;

  static void *operator new(size_t size);

  static void operator delete(void *ptr);
};
}

#endif // __ValueType_h__

/**
* @file Scg/Types/ValueType.h
*
* @copyright Copyright (C) 2016 Rafid Khalid Abdullah
*
* @license This file is released under Alusus Public License, Version 1.0.
* For details on usage and copying conditions read the full license in the
* accompanying license file or at <http://alusus.net/alusus_license_1_0>.
*/
//==============================================================================

#ifndef SCG_VALUETYPE_H
#define SCG_VALUETYPE_H

#include <AstNode.h>
#include <typedefs.h>

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
class ValueType : public AstNode
{
  //============================================================================
  // Type Info

  TYPE_INFO(ValueType, AstNode, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The LLVM type representing this type.
  // TODO: Rename to llvmType for consistency.
  // TODO: Can we change the type to llvm::Type& without having to include
  // LLVM header file for the Type class?
  protected: llvm::Type *llvmType;
  //! The number of variables of this type defined.
  protected: mutable int varCount;
  protected: mutable std::vector<const ValueType *> implicitCastingTargets;
  protected: mutable std::vector<const ValueType *> explicitCastingTargets;
  protected: mutable bool castingTargetsInitialized = false;


  //============================================================================
  // Constructors & Destructor

  protected: ValueType() : llvmType(0), varCount(0)
  {
  }

  public: virtual ~ValueType()
  {
  }


  //============================================================================
  // Operators

  // Disables copy constructor and assignment.
  public: ValueType &operator=(const ValueType &source) = delete;

  public: ValueType(const ValueType &source) = delete;


  //============================================================================
  // Member Functions

  /**
   * Called by getImplicitCastingTargets() getExplicitCastingTargets() to
   * allow the type to initialize implicitCastingTargets and
   * explicitCastingTargets .
   */
  protected: virtual void initCastingTargets() const = 0;

  /**
   * Allocates a new variable of this type.
   * @param[in] name  The name of the variable.
   * @return A pointer to the variable.
   */
  public: virtual SharedPtr<Variable> newVariable(std::string const &name,
                                                  llvm::Argument *llvmArgument = nullptr) const;

  /**
   * Deletes the variable defined by newVariable() method.
   * @param[in] var   A pointer to the variable.
   */
  public: virtual void releaseVariable(Variable *var) const;

  /**
   * Retrieves the name of the type, e.g. int, float, etc.
   *
   * @return The name of the type.
   */
  public: virtual std::string const& getName() const = 0;

  /// Get the number of bytes needed to store variables of this type.
  public: virtual Int getAllocationSize() const;

  /**
   * Return the number of variables of this type defined.
   * @return The number of variables of this type defined.
   */
  public: int getVarCount() const
  {
    return varCount;
  }

  /**
   * Get the LLVM Type object representing this type. This is used to define
   * variables of this type in LLVM.
   *
   * @return A pointer to the LLVM object representing this type.
   */
  public: virtual llvm::Type* getLlvmType() const
  {
    return this->llvmType;
  }

  /**
   * Retrieves the default value of this type as an LLVM Constant object.
   * @return The LLVM Constant object representing the default value.
   */
  public: virtual llvm::Constant* getDefaultLLVMValue() const = 0;

  /**
   * Casts the given LLVM value to the given target type.
   * @param[in] irb         The IRBuilder object used to create the instruction.
   * @param[in] value       The LLVM value to be casted.
   * @param[in] targetType  The target type.
   * @return The casted LLVM value. This is usually an instruction that does
   * the casting having the given LLVM value as its argument.
   */
  public: virtual llvm::Value* createCastInst(llvm::IRBuilder<> *irb,
                                              llvm::Value *value,
                                              ValueType const *targetType) const
  {
    THROW_NOT_IMPLEMENTED();
  }

  /**
   * Determines whether this type is equal to the given type.
   * @param[in] other The type.
   * @return @c true or @c false.
   */
  public: virtual bool isEqualTo(ValueType const *other) const = 0;

  /**
   * Determines whether this type can be implicitly casted to the given type.
   * @param[in] type The type.
   * @return @c true or @c false.
   */
  public: bool isImplicitlyCastableTo(ValueType const *type) const
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
  public: bool isExplicitlyCastableTo(ValueType const *type) const
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
  public: virtual TypeComparisonResult compare(ValueType const *other)
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
  public: virtual std::vector<ValueType const*> const& getImplicitCastingTargets() const
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
  public: virtual std::vector<ValueType const*> const& getExplicitCastingTargets() const
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
  public: static ValueType* getPrimitiveType(std::string const &typeName);

}; // class

} // namespace

#endif

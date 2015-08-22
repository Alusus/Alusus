/**
 * @file Scg/Operators/MemberFieldReference.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __MemberFieldReference_h__
#define __MemberFieldReference_h__

// Alusus header files
#include <typedefs.h>
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{
class Block;
class Value;

/**
 * Represents a reference to a variable by name.
 */
class MemberFieldReference : public Expression
{
protected:
  /*! A pointer to the expression evaluating to a structure containing the field
    this pointer instance is pointing to. */
  Expression *expression;
  //! The name of the field this pointer instance is pointing to.
  std::string fieldName;
  //! A pointer to the LLVM pointer object pointing to the structure field.
  llvm::Value *llvmPointer = nullptr;
  //! Storing the value type to avoid fetching it frequently.
  mutable ValueType *valueType = nullptr;

public:
  // TODO: Change the fieldName to StringConst to allow compile-time constants
  // to be used to access member fields. Later we should also consider
  // changing fieldName to an expression to allow compile-time string
  // expressions to be used to access member fields and make our compiler
  // amazing!
  /**
   * Constructs pointer to a field within a structure.
   * @param[in] expression  A pointer to an expression whole evaluation results
   *                        in a structure.
   * @param[in] fieldName   The name of the field to create a pointer to.
   */
  MemberFieldReference(Expression *exp, Char const *fName) :
    expression(exp), fieldName(fName)
  {
    children.push_back(expression);
  }

  //! @copydoc Expression::getValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override;

  //! @copydoc Expression::generateCode()
  virtual CodeGenerationStage generateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __MemberFieldReference_h__

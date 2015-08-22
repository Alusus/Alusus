/**
 * @file Scg/Instructions/Cast.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Cast_h__
#define __Cast_h__

// Scg header files
#include <typedefs.h>
#include <Instructions/Instruction.h>
#include <llvm_fwd.h>

using namespace Core;

namespace Scg
{
class ValueTypeSpec;

/**
 * Define a mutable variable with a given name.
 */
class Cast : public Instruction
{
  //! The specification of the type to cast the expression to.
  const ValueTypeSpec *typeSpec;
  //! The type to cast the expression to (set during code generation.)
  ValueType *type = nullptr;
  //! The expression to cast
  Expression *expr;

public:
  /**
   * Construct a variable definition expression with the given name.
   *
   * @param[in] typeName  The specification of the variable to be defined.
   *                      This should be a pointer to an instance allocated
   *                      in the heap, and will be freed by this object.
   * @param[in] name      The name of the variable to define.
   */
  Cast(Expression *expr, const ValueTypeSpec *typeSpec)
    : typeSpec(typeSpec)
    , expr(expr)
  {
    children.push_back(expr);
  }

  /**
   * Class destructor.
   */
  ~Cast();

  /**
   * Retrieves a pointer to the target type (the type to cast the expression to.)
   *
   * @return A pointer to the target type.
   */
  const ValueType *getTargetType() const
  {
    return type;
  }
  ValueType *getTargetType()
  {
    return const_cast<ValueType*>(
             static_cast<const Cast*>(this)->getTargetType());
  }

  /**
   * Retrieves the name of the type of this variable.
   *
   * @return The name of the type.
   */
  const ValueTypeSpec *getTargetTypeSpec() const
  {
    return typeSpec;
  }

  //! @copydoc Expression::getValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override;

  //! @copydoc Expression::preGenerateCode()
  virtual CodeGenerationStage preGenerateCode();

  //! @copydoc Expression::generateCode()
  virtual CodeGenerationStage generateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __Cast_h__

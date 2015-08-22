/**
 * @file Scg/Instructions/DefineVariable.h
 *
 * @copyright Copyright (C) 2015 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __DefineVariable_h__
#define __DefineVariable_h__

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
class DefineVariable : public Instruction
{
  //! The name of the type of the variable to define.
  const ValueTypeSpec *typeSpec;
  Expression *value;
  //! The name of the variable to define.
  std::string name;
  //! The variable defined by this instruction (set during code generation.)
  Variable *var = nullptr;
  //! The type of the variable to define (set during code generation.)
  ValueType *type = nullptr;

public:
  /**
   * Construct a variable definition expression with the given name.
   *
   * @param[in] typeName  The specification of the variable to be defined.
   *                      This should be a pointer to an instance allocated
   *                      in the heap, and will be freed by this object.
   * @param[in] name      The name of the variable to define.
   */
  DefineVariable(const ValueTypeSpec *typeSpec, Core::Basic::Char const *name)
    : typeSpec(typeSpec)
    , value(nullptr)
    , name(name)
  {
  }

  /**
  *
  */
  DefineVariable(Expression *value, Core::Basic::Char const *name)
    : value(value)
    , typeSpec(nullptr)
    , name(name)
  {
    this->children.push_back(value);
  }

  /**
   * Class destructor.
   */
  ~DefineVariable();

  /**
   * Retrieves a pointer to the type of this variable.
   *
   * @return A pointer to the variable type.
   */
  const ValueType *getVarType() const
  {
    return type;
  }
  ValueType *getVarType()
  {
    return const_cast<ValueType*>(
             static_cast<const DefineVariable*>(this)->getVarType());
  }

  /**
   * Retrieves the name of the variable to be defined.
   *
   * @return The name of the variable.
   */
  const std::string &getVarName() const
  {
    return name;
  }

  /**
   * Retrieves the name of the type of this variable.
   *
   * @return The name of the type.
   */
  const ValueTypeSpec *getVarTypeSpec() const
  {
    return typeSpec;
  }

  //! @copydoc Expression::preGenerateCode()
  virtual CodeGenerationStage preGenerateCode();

  //! @copydoc Expression::postGenerateCode()
  virtual CodeGenerationStage postGenerateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __DefineVariable_h__

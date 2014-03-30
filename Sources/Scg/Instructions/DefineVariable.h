/**
 * @file Scg/Instructions/DefineVariable.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
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
  DefineVariable(const ValueTypeSpec *typeSpec , const std::string &name)
    : typeSpec(typeSpec)
    , name(name)
  {
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
  const ValueType *GetVarType() const { return type; }
  ValueType *GetVarType()
  {
    return const_cast<ValueType*>(
        static_cast<const DefineVariable*>(this)->GetVarType());
  }

  /**
   * Retrieves the name of the variable to be defined.
   *
   * @return The name of the variable.
   */
  const std::string &GetVarName() const { return name; }

  /**
   * Retrieves the name of the type of this variable.
   *
   * @return The name of the type.
   */
  const ValueTypeSpec *GetVarTypeSpec() const { return typeSpec; }

  //! @copydoc Expression::PreGenerateCode()
  virtual CodeGenerationStage PreGenerateCode();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __DefineVariable_h__

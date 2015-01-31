/**
 * @file Scg/Operators/PointerToVariable.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// TODO: Consider renaming the class to PointerToVariableByName.

#ifndef __PointerToVariable_h__
#define __PointerToVariable_h__

namespace Scg
{
class Block;
class Value;
class ValueType;

/**
 * Represents a reference to a variable by name.
 */
class PointerToVariable : public Expression
{
protected:
  //! The name of the variable.
  std::string name;
  //! Storing the value type to avoid fetching it frequently.
  mutable ValueType *valueType = nullptr;

public:
  /**
   * Constructs pointer to the variable having the given name in the scope where
   * this expression is defined.
   * @param[in] name  The name of the variable to point to.
   */
  PointerToVariable(const std::string &name) : name(name) {}

  //! Class destructor.
  ~PointerToVariable();

  /**
   * Gets the name of the variable pointed to by this instance.
   * @return The name of the variable.
   */
  const std::string &GetName() const { return name; }

  //! @copydoc Pointer::GetValueType()
  virtual const ValueType *GetValueType() const;

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationStage GenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __PointerToVariable_h__

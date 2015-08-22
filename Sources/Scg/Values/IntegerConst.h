/**
 * @file Scg/Values/IntegerConst.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __IntegerConst_h__
#define __IntegerConst_h__

// Scg header files
#include <Values/Constant.h>

namespace Scg
{
/**
 * Represent an integral value.
 */
class IntegerConst : public Constant
{
  //! The value of the integer.
  int value;

public:
  /**
   * Constructs a integer with the given value.
   */
  IntegerConst(int value) : value(value)
  {
  }

  /**
   * Retrieves the integral value of this integer constant object.
   *
   * @return The integral value of this integer constant object.
   */
  const int &getValue() const
  {
    return value;
  }

  //! @copydoc Value::getValueTypeSpec()
  virtual const ValueTypeSpec *getValueTypeSpec() const override;

  //! @copydoc Expression::generateCode()
  virtual CodeGenerationStage generateCode();

  //! @copydoc Expression::postGenerateCode()
  virtual CodeGenerationStage postGenerateCode();

  //! @copydoc Expression::toString()
  virtual std::string toString();
};
}

#endif // __Integer_h__

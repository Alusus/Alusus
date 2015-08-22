/**
 * @file Scg/Values/FloatConst.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __FloatConst_h__
#define __FloatConst_h__

// Scg header files
#include <Values/Constant.h>

namespace Scg
{
/**
 * Represent an integral value.
 */
class FloatConst : public Constant
{
  //! The value of the float.
  float value;

public:
  /**
   * Constructs a float with the given value.
   */
  FloatConst(float value) : value(value)
  {
  }

  /**
   * Retrieves the floating value of this float constant object.
   *
   * @return The floating value of this float constant object.
   */
  const float &getValue() const
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

#endif // __FloatConst_h__

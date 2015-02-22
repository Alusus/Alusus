/**
 * @file Scg/Values/DoubleConst.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __DoubleConst_h__
#define __DoubleConst_h__

// Scg header files
#include <Values/Constant.h>

namespace Scg
{
  /**
   * Represent an integral value.
   */
  class DoubleConst : public Constant
  {
    //! The value of the double.
    double value;

  public:
    /**
     * Constructs a double with the given value.
     */
    DoubleConst(double value) : value(value) {}

    /**
     * Retrieves the value of this double constant object.
     *
     * @return The value of this double constant object.
     */
    const double &GetValue() const { return value; }

    //! @copydoc Value::GetValueTypeSpec()
    virtual const ValueTypeSpec *GetValueTypeSpec() const override;

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationStage GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __DoubleConst_h__

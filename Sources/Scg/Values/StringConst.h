/**
 * @file Scg/Values/StringConst.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __StringConst_h__
#define __StringConst_h__

// Scg header files
#include <Values/Constant.h>

namespace Scg
{
  /**
   * Represent a string.
   */
  class StringConst : public Constant
  {
    //! The value of the string.
    std::string value;

  public:
    /**
     * Constructs a string with the given value.
     */
    StringConst(const std::string &value) : value(value) {}

    /**
     * Retrieves the string value of this string constant object.
     *
     * @return The string value of this string constant object.
     */
    const std::string &GetValue() const { return value; }

    //! @copydoc Value::GetValueType()
    virtual ValueType *GetValueType() const;

    //! @copydoc Expression::GenerateCode()
    virtual CodeGenerationStage GenerateCode();

    //! @copydoc Expression::PostGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __StringConst_h__

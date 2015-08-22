/**
 * @file Scg/Values/CharConst.h
 *
 * @copyright Copyright (C) 2015 Hicham OUALI ALAMI
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __CharConst_h__
#define __CharConst_h__

// Scg header files
#include <Values/Constant.h>

namespace Scg
{
/**
 * Represent an integral value.
 */
class CharConst : public Constant
{
  //! The value of the char.
  char value;

public:
  /**
   * Constructs a char with the given value.
   */
  CharConst(char value) : value(value)
  {
  }

  /**
   * Retrieves the character value of this character constant object.
   *
   * @return The character value of this character constant object.
   */
  const char &getValue() const
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

#endif // __CharConst_h__

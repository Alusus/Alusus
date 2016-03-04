/**
 * @file Scg/Values/CharConst.h
 *
 * @copyright Copyright (C) 2016 Hicham OUALI ALAMI
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CHARCONST_H
#define SCG_CHARCONST_H

#include "core.h"
#include <Values/Constant.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represent an integral value.
 */
class CharConst : public Constant
{
  //============================================================================
  // Type Info

  TYPE_INFO(CharConst, Constant, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The value of the char.
  private: char value;


  //============================================================================
  // Cosntructors & Destructor

  /**
   * Constructs a char with the given value.
   */
  public: CharConst(char value) : value(value)
  {
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the character value of this character constant object.
   *
   * @return The character value of this character constant object.
   */
  public: char getValue() const
  {
    return this->value;
  }

  //! @copydoc Value::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();

}; // class

} // namespace

#endif

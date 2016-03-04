/**
 * @file Scg/Values/FloatConst.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_FLOATCONST_H
#define SCG_FLOATCONST_H

#include <Values/Constant.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represent an integral value.
 */
class FloatConst : public Constant
{
  //============================================================================
  // Type Info

  TYPE_INFO(FloatConst, Constant, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The value of the float.
  private: float value;


  //============================================================================
  // Cosntructors & Destructor

  /**
   * Constructs a float with the given value.
   */
  public: FloatConst(float value) : value(value)
  {
  }

  /**
   * Retrieves the floating value of this float constant object.
   *
   * @return The floating value of this float constant object.
   */
  public: float getValue() const
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

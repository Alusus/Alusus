/**
 * @file Scg/Values/IntegerConst.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_INTEGERCONST_H
#define SCG_INTEGERCONST_H

#include <Values/Constant.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represent an integral value.
 */
class IntegerConst : public Constant
{
  //============================================================================
  // Type Info

  TYPE_INFO(IntegerConst, Constant, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The value of the integer.
  private: int value;


  //============================================================================
  // Cosntructors & Destructor

  /**
   * Constructs a integer with the given value.
   */
  public: IntegerConst(int value) : value(value)
  {
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the integral value of this integer constant object.
   *
   * @return The integral value of this integer constant object.
   */
  public: int getValue() const
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
  virtual std::string toString();

}; // class

} // namespace

#endif

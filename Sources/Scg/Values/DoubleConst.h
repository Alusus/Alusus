/**
 * @file Scg/Values/DoubleConst.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_DOUBLECONST_H
#define SCG_DOUBLECONST_H

#include "core.h"
#include <Values/Constant.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represent an integral value.
 */
class DoubleConst : public Constant
{
  //============================================================================
  // Type Info

  TYPE_INFO(DoubleConst, Constant, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The value of the double.
  private: double value;


  //============================================================================
  // Constructors & Destructor

  /**
   * Constructs a double with the given value.
   */
  public: DoubleConst(double value) : value(value)
  {
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the value of this double constant object.
   *
   * @return The value of this double constant object.
   */
  public: double getValue() const
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

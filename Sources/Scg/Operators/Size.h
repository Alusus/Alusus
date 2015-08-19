/**
 * @file Scg/Values/Size.h
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_OPERATORS_SIZE_H
#define SCG_OPERATORS_SIZE_H

// Scg header files
#include <Values/Constant.h>

namespace Scg
{
/**
 * Represent an integral value.
 */
class Size : public Constant
{
  //============================================================================
  // Member Variables

  private: Expression *expression;
  private: llvm::Value *llvmValue;


  //============================================================================
  // Constructor

  public: Size(Expression *exp);

  public: virtual ~Size()
  {
    if (this->expression) delete this->expression;
  }

  //============================================================================
  // Member Functions

  //! @copydoc Value::GetValueTypeSpec()
  virtual const ValueTypeSpec *GetValueTypeSpec() const override;

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationStage GenerateCode();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();

}; // class

} // namespace

#endif

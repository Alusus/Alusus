/**
 * @file Scg/Values/StringConst.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_STRINGCONST_H
#define SCG_STRINGCONST_H

#include <Values/Constant.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represent a string.
 */
class StringConst : public Constant
{
  //============================================================================
  // Type Info

  TYPE_INFO(StringConst, Constant, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The value of the string.
  private: std::string value;


  //============================================================================
  // Constructors & Destructor

  /**
   * Constructs a string with the given value.
   */
  public: StringConst(std::string const& value) : value(value)
  {
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the string value of this string constant object.
   *
   * @return The string value of this string constant object.
   */
  public: std::string const& getValue() const
  {
    return this->value;
  }

  //! @copydoc Value::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const;

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();

}; // class

} // namespace

#endif

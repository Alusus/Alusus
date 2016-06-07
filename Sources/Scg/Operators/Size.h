/**
 * @file Scg/Values/Size.h
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_OPERATORSSIZE_H
#define SCG_OPERATORSSIZE_H

// Scg header files
#include "core.h"
#include <Values/Constant.h>

namespace Scg
{

class CodeGenUnit;

/**
 * Represent an integral value.
 */
class Size : public Constant, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(Size, Constant, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Constant, Core::Data::Container);


  //============================================================================
  // Member Variables

  private: SharedPtr<AstNode> expression;
  private: llvm::Value *llvmValue;


  //============================================================================
  // Constructor & Destructor

  public: Size(SharedPtr<AstNode> const &exp) : expression(exp), llvmValue(nullptr)
  {
    OWN_SHAREDPTR(this->expression);
  }

  public: virtual ~Size()
  {
    DISOWN_SHAREDPTR(this->expression);
  }


  //============================================================================
  // Member Functions

  //! @copydoc Value::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::callGenerateCode()
  public: virtual CodeGenerationStage callGenerateCode(CodeGenUnit *codeGenUnit)
  {
    // We don't need to call generateCode for children.
    return generateCode(codeGenUnit);
  }

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, TiObject *val)
  {
  }

  public: virtual void remove(Int index)
  {
  }

  public: virtual Word getCount() const
  {
    return 1;
  }

  public: virtual TiObject* get(Int index) const
  {
    if (index == 0) return this->expression.get();
    else return 0;
  }

}; // class

} // namespace

#endif

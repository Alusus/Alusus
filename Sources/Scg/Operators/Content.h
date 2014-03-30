/**
 * @file Scg/Operators/Content.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __Content_h__
#define __Content_h__

// Alusus header files
#include <typedefs.h>
#include <llvm_fwd.h>

namespace Scg
{
class Block;
class Value;

/**
 * An operator that retrieves the contents of a pointer. It is derived from
 * the Pointer class because the content of a pointer can also be a pointer.
 */
class Content : public Expression
{
protected:
  //! The pointer to retrieve the content of.
  Expression *expression;
  llvm::Value *pointer = nullptr;
  //! A pointer to the load instruction that extracts the content of the pointer.
  llvm::LoadInst *loadInst = nullptr;

public:
  /**
   * Constructs an operator that retrieves the contents of the given pointer.
   * @param[in] pointer   The pointer to retrieve the contents of.
   */
  Content(Expression *exp) : expression(exp)
  {
    children.push_back(expression);
  }

  llvm::Value *GetLlvmContent()
  {
    return loadInst;
  }

  llvm::Value *GetLlvmPointer()
  {
    return pointer;
  }

  //! @copydoc Expression::GetValueType()
  virtual const ValueType *GetValueType() const;

  //! @copydoc Expression::GenerateCode()
  virtual CodeGenerationResult GenerateCode();

  //! @copydoc Expression::PostGenerateCode()
  virtual CodeGenerationStage  PostGenerateCode();

  //! @copydoc Expression::ToString()
  virtual std::string ToString();
};
}

#endif // __Content_h__

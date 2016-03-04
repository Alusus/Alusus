/**
 * @file Scg/Operators/Content.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CONTENT_H
#define SCG_CONTENT_H

#include "core.h"
#include <typedefs.h>
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{

class Block;
class Value;
class CodeGenUnit;

/**
 * An operator that retrieves the contents of a pointer. It is derived from
 * the Pointer class because the content of a pointer can also be a pointer.
 */
class Content : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(Content, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! The pointer to retrieve the content of.
  protected: SharedPtr<AstNode> expression;
  protected: llvm::Value *pointer = nullptr;
  //! A pointer to the load instruction that extracts the content of the pointer.
  protected: llvm::LoadInst *loadInst = nullptr;


  //============================================================================
  // Constructors & Destructor

  /**
   * Constructs an operator that retrieves the contents of the given pointer.
   * @param[in] pointer   The pointer to retrieve the contents of.
   */
  public: Content(SharedPtr<AstNode> const &exp) : expression(exp)
  {
    OWN_SHAREDPTR(this->expression);
  }

  public: virtual ~Content()
  {
    DISOWN_SHAREDPTR(this->expression);
  }


  //============================================================================
  // Member Functions

  public: llvm::Value* getLlvmContent()
  {
    return loadInst;
  }

  public: llvm::Value* getLlvmPointer()
  {
    return pointer;
  }

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage  postGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();


  //============================================================================
  // Container Implementation

  public: virtual void set(Int index, IdentifiableObject *val)
  {
  }

  public: virtual void remove(Int index)
  {
  }

  public: virtual Word getCount() const
  {
    return 1;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    if (index == 0) return this->expression.get();
    else return 0;
  }

}; // class

} // namespace

#endif

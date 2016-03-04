/**
 * @file Scg/Operators/MemberFieldReference.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_MEMBERFIELDREFERENCE_H
#define SCG_MEMBERFIELDREFERENCE_H

#include "core.h"
#include <AstNode.h>
#include <typedefs.h>
#include <llvm_fwd.h>
#include <Types/ValueTypeSpec.h>

namespace Scg
{

class Block;
class Value;
class CodeGenUnit;

/**
 * Represents a reference to a variable by name.
 */
class MemberFieldReference : public AstNode, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(MemberFieldReference, AstNode, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(AstNode, Core::Data::Container);


  //============================================================================
  // Member Variables

  /*! A pointer to the expression evaluating to a structure containing the field
    this pointer instance is pointing to. */
  protected: SharedPtr<AstNode> expression;
  //! The name of the field this pointer instance is pointing to.
  protected: std::string fieldName;
  //! A pointer to the LLVM pointer object pointing to the structure field.
  protected: llvm::Value *llvmPointer = nullptr;
  //! Storing the value type to avoid fetching it frequently.
  protected: mutable ValueType *valueType = nullptr;


  //============================================================================
  // Constructor & Destructor

  // TODO: Change the fieldName to StringConst to allow compile-time constants
  // to be used to access member fields. Later we should also consider
  // changing fieldName to an expression to allow compile-time string
  // expressions to be used to access member fields and make our compiler
  // amazing!
  /**
   * Constructs pointer to a field within a structure.
   * @param[in] expression  A pointer to an expression whole evaluation results
   *                        in a structure.
   * @param[in] fieldName   The name of the field to create a pointer to.
   */
  public: MemberFieldReference(SharedPtr<AstNode> const &exp, Char const *fName) :
    expression(exp), fieldName(fName)
  {
    OWN_SHAREDPTR(this->expression);
  }

  public: virtual ~MemberFieldReference()
  {
    DISOWN_SHAREDPTR(this->expression);
  }


  //============================================================================
  // Member Functions

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

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

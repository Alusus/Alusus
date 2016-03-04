/**
 * @file Scg/Instructions/Cast.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_CAST_H
#define SCG_CAST_H

#include "core.h"
#include <typedefs.h>
#include <Instructions/Instruction.h>
#include <llvm_fwd.h>

using namespace Core;

namespace Scg
{

class ValueTypeSpec;
class CodeGenUnit;

/**
 * Define a mutable variable with a given name.
 */
class Cast : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(Cast, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! The specification of the type to cast the expression to.
  private: SharedPtr<ValueTypeSpec> typeSpec;
  //! The expression to cast
  private: SharedPtr<AstNode> expr;
  //! The type to cast the expression to (set during code generation.)
  private: ValueType *type = nullptr;


  //============================================================================
  // Constructors & Destructor

  /**
   * Construct a variable definition expression with the given name.
   *
   * @param[in] typeName  The specification of the variable to be defined.
   *                      This should be a pointer to an instance allocated
   *                      in the heap, and will be freed by this object.
   * @param[in] name      The name of the variable to define.
   */
  public: Cast(SharedPtr<AstNode> const &expr, SharedPtr<ValueTypeSpec> const &typeSpec)
    : typeSpec(typeSpec)
    , expr(expr)
  {
    OWN_SHAREDPTR(this->expr);
  }

  public: virtual ~Cast()
  {
    DISOWN_SHAREDPTR(this->expr);
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves a pointer to the target type (the type to cast the expression to.)
   *
   * @return A pointer to the target type.
   */
  public: const ValueType *getTargetType() const
  {
    return type;
  }
  public: ValueType *getTargetType()
  {
    return const_cast<ValueType*>(
             static_cast<const Cast*>(this)->getTargetType());
  }

  /**
   * Retrieves the name of the type of this variable.
   *
   * @return The name of the type.
   */
  public: SharedPtr<ValueTypeSpec> const& getTargetTypeSpec() const
  {
    return this->typeSpec;
  }

  //! @copydoc AstNode::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override
  {
    return this->typeSpec;
  }

  //! @copydoc AstNode::preGenerateCode()
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit);

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
    return this->expr.get();
  }

}; // class

} // namespace

#endif

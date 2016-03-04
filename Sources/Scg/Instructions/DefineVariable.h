/**
 * @file Scg/Instructions/DefineVariable.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_DEFINEVARIABLE_H
#define SCG_DEFINEVARIABLE_H

#include "core.h"
#include <typedefs.h>
#include <Instructions/Instruction.h>
#include <Values/Variable.h>
#include <llvm_fwd.h>

using namespace Core;

namespace Scg
{

class ValueTypeSpec;
class CodeGenUnit;

/**
 * Define a mutable variable with a given name.
 */
class DefineVariable : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(DefineVariable, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! The name of the type of the variable to define.
  private: SharedPtr<ValueTypeSpec> typeSpec;
  private: SharedPtr<AstNode> value;
  //! The name of the variable to define.
  private: std::string name;
  //! The variable defined by this instruction (set during code generation.)
  private: SharedPtr<Variable> var;
  //! The type of the variable to define (set during code generation.)
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
  public: DefineVariable(SharedPtr<ValueTypeSpec> const &typeSpec, Core::Basic::Char const *name)
    : typeSpec(typeSpec)
    , value(nullptr)
    , name(name)
  {
  }

  public: DefineVariable(SharedPtr<AstNode> const &value, Core::Basic::Char const *name)
    : value(value)
    , typeSpec(nullptr)
    , name(name)
  {
    OWN_SHAREDPTR(this->value);
  }

  public: virtual ~DefineVariable()
  {
    DISOWN_SHAREDPTR(this->value);
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves a pointer to the type of this variable.
   *
   * @return A pointer to the variable type.
   */
  public: ValueType const* getVarType() const
  {
    return this->type;
  }
  public: ValueType* getVarType()
  {
    return const_cast<ValueType*>(
             static_cast<const DefineVariable*>(this)->getVarType());
  }

  /**
   * Retrieves the name of the variable to be defined.
   *
   * @return The name of the variable.
   */
  public: std::string const& getVarName() const
  {
    return this->name;
  }

  /**
   * Retrieves the name of the type of this variable.
   *
   * @return The name of the type.
   */
  public: SharedPtr<ValueTypeSpec> const& getVarTypeSpec() const
  {
    return this->typeSpec;
  }

  //! @copydoc AstNode::preGenerateCode()
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::postGenerateCode()
  public: virtual CodeGenerationStage postGenerateCode(CodeGenUnit *codeGenUnit);

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
    return this->value == 0 ? 1 : 2;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    if (this->value == 0) {
      if (index == 0) return this->var.get();
      else return 0;
    } else {
      if (index == 0) return this->value.get();
      else if (index == 1) return this->var.get();
      else return 0;
    }
  }

}; // class

} // namespace

#endif

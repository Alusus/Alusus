/**
 * @file Scg/Operators/IdentifierReference.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

// TODO: Consider renaming the class to IdentifierReferenceByName.

#ifndef SCG_IDENTIFIERREFERENCE_H
#define SCG_IDENTIFIERREFERENCE_H

#include "core.h"
#include <AstNode.h>
#include <Types/ValueTypeSpec.h>

using namespace Core;

namespace Scg
{

class Block;
class Value;
class ValueType;
class CodeGenUnit;

/**
 * Represents a reference to a variable by name.
 */
class IdentifierReference : public AstNode
{
  //============================================================================
  // Type Info

  TYPE_INFO(IdentifierReference, AstNode, "Scg", "Scg", "alusus.net");


  //============================================================================
  // Member Variables

  //! The name of the variable.
  protected: std::string name;
  //! Storing the value type to avoid fetching it frequently.
  protected: mutable ValueType *valueType = nullptr;


  //============================================================================
  // Constructor & Destructor

  /**
   * Constructs pointer to the variable having the given name in the scope where
   * this expression is defined.
   * @param[in] name  The name of the variable to point to.
   */
  public: IdentifierReference(Char const *name) : name(name) {}


  //============================================================================
  // Member Functions

  /**
   * Gets the name of the variable pointed to by this instance.
   * @return The name of the variable.
   */
  public: const std::string& getName() const
  {
    return this->name;
  }

  //! @copydoc Pointer::getValueTypeSpec()
  public: virtual SharedPtr<ValueTypeSpec> const& getValueTypeSpec() const override;

  //! @copydoc AstNode::generateCode()
  public: virtual CodeGenerationStage generateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::toString()
  public: virtual std::string toString();

}; // class

} // namespace

#endif

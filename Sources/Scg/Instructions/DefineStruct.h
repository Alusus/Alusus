/**
 * @file Scg/Instructions/DefineStruct.h
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SCG_DEFINESTRUCT_H
#define SCG_DEFINESTRUCT_H

#include "core.h"
#include <typedefs.h>
#include <Instructions/Instruction.h>
#include <Types/StructType.h>
#include <Types/ValueTypeSpec.h>
#include <llvm_fwd.h>

using namespace Core;

namespace Scg
{

class CodeGenUnit;

/**
 * Represents a call to a function.
 */
class DefineStruct : public Instruction, public virtual Core::Data::Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(DefineStruct, Instruction, "Scg", "Scg", "alusus.net");
  IMPLEMENT_INTERFACES_1(Instruction, Core::Data::Container);


  //============================================================================
  // Member Variables

  //! The name of the structure.
  private: std::string name;
  //! An array of the fields of the structure.
  private: VariableDefinitionArray fields;
  //! The LLVM StructType representing this structure.
  private: SharedPtr<StructType> structType;


  //============================================================================
  // Constructors & Destructor

  /**
   * Defines a new structure.
   * @param[in] name    The name of the structure.
   * @param[in] fields  The fields in the structure.
   */
  public: DefineStruct(Char const *name, VariableDefinitionArray const &fields)
    : name(name), fields(fields)
  {
  }

  public: static SharedPtr<DefineStruct> create(Char const *name, VariableDefinitionArray const &fields)
  {
    return std::make_shared<DefineStruct>(name, fields);
  }


  //============================================================================
  // Member Functions

  /**
   * Retrieves the name of the structure.
   * @return A string containing the name of the structure.
   */
  public: std::string const& getName() const
  {
    return this->name;
  }

  /**
   * Retrieves an array containing the fields of the structure.
   * @return An array containing the fields of the structure.
   */
  public: VariableDefinitionArray const& getFields() const
  {
    return this->fields;
  }

  //! @copydoc AstNode::preGenerateCode()
  public: virtual CodeGenerationStage preGenerateCode(CodeGenUnit *codeGenUnit);

  //! @copydoc AstNode::preGenerateCode()
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
    return 1;
  }

  public: virtual IdentifiableObject* get(Int index) const
  {
    return this->structType.get();
  }

}; // class

} // namespace

#endif

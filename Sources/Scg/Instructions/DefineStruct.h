/**
 * @file Scg/Instructions/DefineStruct.h
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef __DefineStruct_h__
#define __DefineStruct_h__

// Scg header files
#include <typedefs.h>
#include <Instructions/Instruction.h>
#include <Types/ValueTypeSpec.h>

#include <llvm_fwd.h>

namespace Scg
{
  class StructType;

  /**
   * Represents a call to a function.
   */
  class DefineStruct : public Instruction
  {
    //! The name of the structure.
    std::string name;
    //! An array of the fields of the structure.
    VariableDefinitionArray fields;
    //! The LLVM StructType representing this structure.
    StructType *structType;

  public:
    /**
     * Defines a new structure.
     * @param[in] name    The name of the structure.
     * @param[in] fields  The fields in the structure.
     */
    DefineStruct(const std::string &name, const VariableDefinitionArray &fields)
      : name(name), fields(fields), structType(0) { }

    /**
     * Retrieves the name of the structure.
     * @return A string containing the name of the structure.
     */
    const std::string &GetName() const { return name; }

    /**
     * Retrieves an array containing the fields of the structure.
     * @return An array containing the fields of the structure.
     */
    const VariableDefinitionArray &GetFields() const { return fields; }

    //! @copydoc Expression::PreGenerateCode()
    virtual CodeGenerationStage PreGenerateCode();

    //! @copydoc Expression::PreGenerateCode()
    virtual CodeGenerationStage PostGenerateCode();

    //! @copydoc Expression::ToString()
    virtual std::string ToString();
  };
}

#endif // __DefineStruct_h__

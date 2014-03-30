/**
 * @file Scg/Instructions/DefineStruct.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg header files
#include <Containers/Module.h>
#include <Instructions/DefineStruct.h>
#include <Types/StructType.h>

namespace Scg
{
Expression::CodeGenerationStage DefineStruct::PreGenerateCode()
{
  MODULE_CHECK;

  // Creates a new structure type.
  this->structType = new StructType(this->name);
  ((Expression*)this->structType)->SetModule(GetModule());
  ((Expression*)this->structType)->SetFunction(GetFunction());
  ((Expression*)this->structType)->SetBlock(GetBlock());
  GetModule()->GetTypeMap()[this->name] = this->structType;
  this->children.push_back(this->structType);

  // Creates the types of the fields of the structure.
  // TODO: It is highly possibly to have more than one field with the same
  // type. Consider creating one type for each distinct variable type.
  // I think for now the best way is to handle getting value types at the module
  // where we can have a container for all the types needed in the module.
  // In the future, it might be even better to have one container for all
  // modules to reduce duplication further and save memory.
  ValueTypeNameArray fields;
  auto i = 0;
  for (auto field : this->fields)
  {
    fields.push_back(ValueTypeNamePair(field.GetTypeSpec()->ToValueType(*GetModule()), field.GetVariableName()));
    i++;
  }

  // Creates the fields of the structure. In case you are wondering why the
  // constructor doesn't accept the list of fields of the structure to create
  // the structure in one statement, the reason is that we also want to allow
  // the possibility of having a pointer of the structure type itself, e.g.
  // a linked list node. Thus we need to create the structure and register its
  // name, then create the fields.
  // TODO: Don't allow fields of non-pointer type of the structure itself.
  this->structType->SetFields(fields);

  return CodeGenerationStage::CodeGeneration;
}

//------------------------------------------------------------------------------

Expression::CodeGenerationStage DefineStruct::PostGenerateCode()
{
  // TODO: It might be better to have a method Delete() that does this
  // check and delete itself.
  if (this->structType != 0 && this->structType->GetVarCount() != 0)
    // Cannot delete the structure yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;

  // Deletes the structure type we created in the pre-code generation step.
  if (this->structType != 0) {
      delete this->structType;
      this->structType = 0;
  }

  this->children.clear();
  return CodeGenerationStage::None;
}

//------------------------------------------------------------------------------

std::string DefineStruct::ToString()
{
  std::string str;
  str.append("def " + this->name + " : struct\n");
  str.append("{\n");
  for (auto pair : this->fields)
  {
    str.append("  ");
    str.append(pair.GetVariableName());
    str.append(" : ");
    str.append(pair.GetTypeSpec()->ToString());
  }
  str.append("}\n");
  return str;
}
}

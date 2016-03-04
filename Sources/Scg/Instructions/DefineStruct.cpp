/**
 * @file Scg/Instructions/DefineStruct.cpp
 *
 * @copyright Copyright (C) 2016 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>

// Scg header files
#include <CodeGenUnit.h>
#include <Containers/Module.h>
#include <Instructions/DefineStruct.h>
#include <Types/StructType.h>

namespace Scg
{

AstNode::CodeGenerationStage DefineStruct::preGenerateCode(CodeGenUnit *codeGenUnit)
{
  MODULE_CHECK;

  // Creates a new structure type.
  this->structType = std::make_shared<StructType>(this->name);
  this->structType->setOwner(this);
  Module *module = this->findOwner<Module>();
  module->getTypeMap()[this->name] = this->structType.get();

  // Creates the types of the fields of the structure.
  // TODO: It is highly possibly to have more than one field with the same
  // type. Consider creating one type for each distinct variable type.
  // I think for now the best way is to handle getting value types at the module
  // where we can have a container for all the types needed in the module.
  // In the future, it might be even better to have one container for all
  // modules to reduce duplication further and save memory.
  ValueTypeNameArray fields;
  auto i = 0;

  for (auto field : this->fields) {
    fields.push_back(ValueTypeNamePair(field.getTypeSpec()->toValueType(*module), field.getVariableName()));
    i++;
  }

  // Creates the fields of the structure. In case you are wondering why the
  // constructor doesn't accept the list of fields of the structure to create
  // the structure in one statement, the reason is that we also want to allow
  // the possibility of having a pointer of the structure type itself, e.g.
  // a linked list node. Thus we need to create the structure and register its
  // name, then create the fields.
  // TODO: Don't allow fields of non-pointer type of the structure itself.
  this->structType->setFields(fields);

  return CodeGenerationStage::CodeGeneration;
}


AstNode::CodeGenerationStage DefineStruct::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  // TODO: It might be better to have a method Delete() that does this
  // check and delete itself.
  if (this->structType != 0 && this->structType->getVarCount() != 0)
    // Cannot delete the structure yet; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;

  // Deletes the structure type we created in the pre-code generation step.
  if (this->structType != 0) {
    this->structType->setOwner(0);
    this->structType.reset();
  }

  return CodeGenerationStage::None;
}


std::string DefineStruct::toString()
{
  std::string str;
  str.append("def " + this->name + " : struct\n");
  str.append("{\n");

  for (auto pair : this->fields) {
    str.append("  ");
    str.append(pair.getVariableName());
    str.append(" : ");
    str.append(pair.getTypeSpec()->toString());
  }

  str.append("}\n");
  return str;
}

} // namespace

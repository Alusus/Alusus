/**
 * @file Scg/Operators/Size.cpp
 *
 * @copyright Copyright (C) 2016 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <prerequisites.h>
// LLVM header files
#include <llvm/IR/IRBuilder.h>
// Scg files
#include <CodeGenUnit.h>
#include <Operators/IdentifierReference.h>
#include <Containers/Block.h>
#include <Operators/Content.h>
#include <Operators/Size.h>
#include <Types/PointerType.h>
#include <Types/IntegerType.h>

namespace Scg
{

SharedPtr<ValueTypeSpec> const& Size::getValueTypeSpec() const
{
  return IntegerType::get()->getValueTypeSpec();
}


AstNode::CodeGenerationStage Size::generateCode(CodeGenUnit *codeGenUnit)
{
  IdentifierReference *ref = this->expression.tio_cast_get<IdentifierReference>();
  ValueType const *type = 0;

  auto module = this->findOwner<Module>();

  // If the provided expression is just an identifier, then we will lookup the type in the list of
  // varialbes of the current block and the list of types in the current module.
  if (ref != 0) {
    Variable const *var;

    if ((var = this->findOwner<Block>()->getVariable(ref->getName())) != 0) {
      type = PointerType::get(var->getValueTypeSpec()->toValueType(*module));
      // For variables the returned type is actually the pointer to that variable, so we need to get
      // the actual type of the variable, not the type of its pointer.
      auto ptype = dynamic_cast<PointerType const*>(type);

      if (ptype != 0) type = ptype->getContentType();
    } else if ((type = module->tryGetValueTypeByName(ref->getName())) == 0) {
      throw EXCEPTION(UndefinedVariableException,
                      ("Referencing undefined identifier: " + ref->getName()).c_str());
    }
  } else {
    type = this->expression->getValueTypeSpec()->toValueType(*module);
  }

  this->generatedLlvmValue = this->llvmValue = IntegerType::get()->getLlvmConstant(type->getAllocationSize());
  return AstNode::generateCode(codeGenUnit);
}


AstNode::CodeGenerationStage Size::postGenerateCode(CodeGenUnit *codeGenUnit)
{
  if (this->llvmValue == nullptr)
    // Nothing to delete
    return CodeGenerationStage::None;

  if (!this->llvmValue->hasNUses(0))
    // The value is still in use; stay in PostCodeGeneration stage.
    return CodeGenerationStage::PostCodeGeneration;

  // We don't need to delete the constant, it is handled by LLVM.
  this->llvmValue = nullptr;
  return CodeGenerationStage::None;
}


std::string Size::toString()
{
  // TODO: Implement this function.
  return "";
}

} // namespace
